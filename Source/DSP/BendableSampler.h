#pragma once
#include <JuceHeader.h>
#include <cmath>

// ════════════════════════════════════════════════════════════════════════════
//  A multisample player that actually responds to the pitch + mod wheels.
//
//  juce::SamplerVoice ignores pitchWheelMoved/controllerMoved, so on sampled
//  presets the wheels did nothing. These classes reimplement sample playback
//  with: real pitch bend (+/- 2 semitones from the pitch wheel) and a mod-wheel
//  vibrato (small ~5.5 Hz pitch LFO). One sound = one wav mapped over a key zone.
// ════════════════════════════════════════════════════════════════════════════

class BendableSound : public juce::SynthesiserSound
{
public:
    BendableSound(const juce::String& nm, juce::AudioFormatReader& reader,
                  const juce::BigInteger& notes, int rootMidiNote,
                  double attackSecs, double releaseSecs, double maxSampleSecs)
        : name(nm), midiNotes(notes), rootNote(rootMidiNote),
          sourceSampleRate(reader.sampleRate)
    {
        const int len = (int) juce::jmin((juce::int64) (maxSampleSecs * sourceSampleRate),
                                         reader.lengthInSamples);
        length = juce::jmax(1, len);
        data.setSize((int) juce::jmin(2u, reader.numChannels), length + 4, false, true, true);
        reader.read(&data, 0, length, 0, true, true);

        adsr.attack  = (float) attackSecs;
        adsr.release = (float) releaseSecs;
        adsr.sustain = 1.0f;
        adsr.decay   = 0.0f;
    }

    bool appliesToNote(int note) override    { return midiNotes[note]; }
    bool appliesToChannel(int) override      { return true; }

    juce::AudioBuffer<float> data;
    juce::String   name;
    juce::BigInteger midiNotes;
    int    rootNote        = 60;
    int    length          = 0;
    double sourceSampleRate = 44100.0;
    juce::ADSR::Parameters adsr;
};

class BendableVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* s) override
    {
        return dynamic_cast<BendableSound*>(s) != nullptr;
    }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound* s, int currentPitchWheel) override
    {
        if (auto* sound = dynamic_cast<BendableSound*>(s))
        {
            playing = sound;
            pos = 0.0;
            level = velocity * 0.8f + 0.1f;

            baseRatio = sound->sourceSampleRate / getSampleRate()
                        * std::pow(2.0, (midiNote - sound->rootNote) / 12.0);

            pitchWheelMoved(currentPitchWheel);
            adsr.setSampleRate(getSampleRate());
            adsr.setParameters(sound->adsr);
            adsr.noteOn();
        }
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
            playing = nullptr;
        }
    }

    void pitchWheelMoved(int newValue) override
    {
        // +/- 2 semitones over the 14-bit wheel range.
        const double semis = (newValue - 8192) / 8192.0 * 2.0;
        bendFactor = std::pow(2.0, semis / 12.0);
    }

    void controllerMoved(int controller, int newValue) override
    {
        if (controller == 1) // CC1 = mod wheel → vibrato depth
            modDepth = newValue / 127.0f;
    }

    void renderNextBlock(juce::AudioBuffer<float>& out, int startSample, int numSamples) override
    {
        if (playing == nullptr)
            return;

        auto& src = playing->data;
        const int srcCh = src.getNumChannels();
        const int outCh = out.getNumChannels();
        const int len   = playing->length;

        const double vibInc = juce::MathConstants<double>::twoPi * 5.5 / getSampleRate();

        for (int i = 0; i < numSamples; ++i)
        {
            const int p0 = (int) pos;
            if (p0 >= len - 1)
            {
                clearCurrentNote();
                adsr.reset();
                playing = nullptr;
                break;
            }
            const float frac = (float) (pos - p0);
            const float env  = adsr.getNextSample();

            for (int ch = 0; ch < outCh; ++ch)
            {
                const float* in = src.getReadPointer(juce::jmin(ch, srcCh - 1));
                const float s = in[p0] + frac * (in[p0 + 1] - in[p0]);
                out.addSample(ch, startSample + i, s * env * level);
            }

            // Mod-wheel vibrato: small pitch LFO around the bent ratio.
            const double vib = 1.0 + std::sin(vibPhase) * (modDepth * 0.03);
            vibPhase += vibInc;
            if (vibPhase > juce::MathConstants<double>::twoPi)
                vibPhase -= juce::MathConstants<double>::twoPi;

            pos += baseRatio * bendFactor * vib;

            if (! adsr.isActive())
            {
                clearCurrentNote();
                playing = nullptr;
                break;
            }
        }
    }

private:
    BendableSound* playing = nullptr;
    juce::ADSR adsr;
    double pos = 0.0;
    double baseRatio = 1.0;
    double bendFactor = 1.0;
    double vibPhase = 0.0;
    float  level = 1.0f;
    float  modDepth = 0.0f;
};
