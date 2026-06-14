/*
  ==============================================================================
    AndroidActivity.h - Android Activity JNI Interface
  ==============================================================================

    Bridges Java/Kotlin AndroidActivity with C++ OrientalInstrument processor.
    Handles:
    - Touch input → MIDI note on/off (piano or custom gesture)
    - Accelerometer/gyro → pitch bend / modulation wheel
    - Audio focus / lifecycle callbacks
    - Screen orientation hints
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

namespace OrientalAndroid
{
    /**
        Manages the Android Activity lifecycle and sensor input for the app.
        Called from Java via JNI (attached to the main Java Activity).
    */
    class AndroidActivity
    {
    public:
        explicit AndroidActivity(OrientalInstrumentProcessor* proc);
        ~AndroidActivity();

        // ─ Lifecycle ─
        void onResume();
        void onPause();
        void onDestroy();

        // ─ Touch Input (piano or touch pad) ─
        /**
            Maps a touch event to a MIDI note.
            @param touchX       normalized x (0.0 - 1.0) across screen width
            @param touchY       normalized y (0.0 - 1.0) across screen height
            @param isDown       true = note on, false = note off
            @param touchId      unique touch identifier for multi-touch
        */
        void handleTouchEvent(float touchX, float touchY, bool isDown, int touchId);

        // ─ Sensor Input ─
        /**
            Accelerometer callback: maps device tilt to pitch bend.
            @param accelX, accelY, accelZ   raw acceleration values (m/s²)
        */
        void handleAccelerometerData(float accelX, float accelY, float accelZ);

        /**
            Gyroscope callback: maps device rotation to modulation wheel.
            @param gyroX, gyroY, gyroZ   raw rotation values (rad/s)
        */
        void handleGyroscopeData(float gyroX, float gyroY, float gyroZ);

        // ─ Audio Focus & Permissions ─
        void requestAudioFocus();
        void releaseAudioFocus();
        void requestMicrophonePermission(); // Future: for recording

        // ─ Screen Orientation ─
        void setPreferredOrientation(bool landscape);

        // ─ Getters ─
        bool isAudioFocused() const { return audioFocused; }
        bool isActive() const { return active; }

    private:
        OrientalInstrumentProcessor* processor = nullptr;

        // Sensor smoothing & state
        std::atomic<float> pitchBendSmoothed { 0.0f };
        std::atomic<float> modWheelSmoothed { 0.0f };
        float pitchBendVelocity = 0.0f;
        float modWheelVelocity = 0.0f;

        // Touch tracking
        juce::CriticalSection touchLock;
        std::unordered_map<int, int> activeTouches; // touchId -> MIDI note
        int lastTouchNote = -1;

        // State
        std::atomic<bool> active { false };
        std::atomic<bool> audioFocused { false };

        // Sensor filtering (1st-order low-pass)
        void updatePitchBend(float accelX);
        void updateModWheel(float gyroY);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AndroidActivity)
    };

} // namespace OrientalAndroid
