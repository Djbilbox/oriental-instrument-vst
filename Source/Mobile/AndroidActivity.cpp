/*
  ==============================================================================
    AndroidActivity.cpp - Implementation
  ==============================================================================
*/

#include "AndroidActivity.h"

namespace OrientalAndroid
{
    AndroidActivity::AndroidActivity(OrientalInstrumentProcessor* proc)
        : processor(proc)
    {
        jassert(processor != nullptr);
    }

    AndroidActivity::~AndroidActivity() = default;

    // ─────────────────────────────────────────────────────────────────────
    // Lifecycle
    // ─────────────────────────────────────────────────────────────────────

    void AndroidActivity::onResume()
    {
        active = true;
        juce::Logger::writeToLog("[Android] Activity resumed");
    }

    void AndroidActivity::onPause()
    {
        active = false;
        processor->panicAllNotes();
        juce::Logger::writeToLog("[Android] Activity paused");
    }

    void AndroidActivity::onDestroy()
    {
        active = false;
        audioFocused = false;
        releaseAudioFocus();
        juce::Logger::writeToLog("[Android] Activity destroyed");
    }

    // ─────────────────────────────────────────────────────────────────────
    // Touch Input
    // ─────────────────────────────────────────────────────────────────────

    void AndroidActivity::handleTouchEvent(float touchX, float touchY,
                                           bool isDown, int touchId)
    {
        if (processor == nullptr || !active)
            return;

        juce::ScopedLock lock(touchLock);

        // Map touch Y position to MIDI note (simple linear map, bottom = low notes)
        // Screen height (0 - 1.0) → MIDI notes (36 - 96, 5 octaves)
        int note = 36 + static_cast<int>((1.0f - touchY) * 60.0f);
        note = juce::jlimit(36, 96, note);

        if (isDown)
        {
            // Note on
            auto iter = activeTouches.find(touchId);
            if (iter != activeTouches.end())
            {
                // Already pressed; note off the old note first
                processor->getSynthesiser().noteOff(
                    iter->second, 0.0f, false);
            }

            activeTouches[touchId] = note;
            processor->getSynthesiser().noteOn(note, 0.8f, 0);

            lastTouchNote = note;
        }
        else
        {
            // Note off
            auto iter = activeTouches.find(touchId);
            if (iter != activeTouches.end())
            {
                processor->getSynthesiser().noteOff(
                    iter->second, 0.5f, false);
                activeTouches.erase(iter);
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    // Sensor Input
    // ─────────────────────────────────────────────────────────────────────

    void AndroidActivity::handleAccelerometerData(
        float accelX, float accelY, float accelZ)
    {
        if (processor == nullptr || !active)
            return;

        // Normalize accelerometer to ±1g ≈ ±9.8 m/s²
        // Tilt forward/back (accelY) → pitch bend (±12 semitones)
        updatePitchBend(accelY / 9.8f);
        processor->setPitchBend(pitchBendSmoothed);
    }

    void AndroidActivity::handleGyroscopeData(
        float gyroX, float gyroY, float gyroZ)
    {
        if (processor == nullptr || !active)
            return;

        // Gyro rotation around Y-axis → mod wheel
        // ±200°/s → full mod wheel sweep (0 - 1)
        updateModWheel(gyroY / 200.0f);
        processor->setModWheel(modWheelSmoothed);
    }

    void AndroidActivity::updatePitchBend(float accelX)
    {
        // 1st-order low-pass filter: 0.1 = fast response, 0.95 = very smooth
        const float alpha = 0.15f;
        float target = juce::jlimit(-1.0f, 1.0f, accelX);
        pitchBendSmoothed = pitchBendSmoothed * (1.0f - alpha)
                            + target * alpha;
    }

    void AndroidActivity::updateModWheel(float gyroY)
    {
        // Mod wheel is normalized 0 - 1, centered at 0.5
        const float alpha = 0.1f;
        float target = 0.5f + juce::jlimit(-0.5f, 0.5f, gyroY);
        modWheelSmoothed = modWheelSmoothed * (1.0f - alpha)
                           + target * alpha;
    }

    // ─────────────────────────────────────────────────────────────────────
    // Audio Focus & Permissions
    // ─────────────────────────────────────────────────────────────────────

    void AndroidActivity::requestAudioFocus()
    {
        audioFocused = true;
        juce::Logger::writeToLog("[Android] Audio focus granted");
        // In a real app, call Java via JNI to request system audio focus
    }

    void AndroidActivity::releaseAudioFocus()
    {
        audioFocused = false;
        juce::Logger::writeToLog("[Android] Audio focus released");
    }

    void AndroidActivity::requestMicrophonePermission()
    {
        juce::Logger::writeToLog("[Android] Microphone permission requested");
        // In a real app, call Java via JNI to request android.permission.RECORD_AUDIO
    }

    // ─────────────────────────────────────────────────────────────────────
    // Screen Orientation
    // ─────────────────────────────────────────────────────────────────────

    void AndroidActivity::setPreferredOrientation(bool landscape)
    {
        // Call Java via JNI to set ActivityInfo.SCREEN_ORIENTATION
        juce::Logger::writeToLog(
            juce::String("[Android] Orientation set to ")
            + (landscape ? "landscape" : "portrait"));
    }

} // namespace OrientalAndroid
