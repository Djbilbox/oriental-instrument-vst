#pragma once
#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include "ADSREnvelope.h"
#include "MaqamTuning.h"
#include "../Data/InstrumentProfiles.h"

class OrientalVoice : public juce::SynthesiserVoice
{
public:
    OrientalVoice();
    ~OrientalVoice() override = default;

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;

    void setInstrumentProfile(const InstrumentProfile& profile);
    void setMaqamTuning(const MaqamTuning& tuning);
    void setGlide(float glideTime);
    void setFilterParams(float cutoff, float resonance);
    void setOrientAmount(float amount);
    void setDepth(float depth);

    void prepareToPlay(double sampleRate, int samplesPerBlock);

private:
    WavetableOscillator oscillator;
    WavetableOscillator vibratoLFO;
    ADSREnvelope envelope;

    const InstrumentProfile* currentProfile = nullptr;
    MaqamTuning maqamTuning;

    float targetFrequency = 440.0f;
    float currentFrequency = 440.0f;
    float glideRate = 0.0f;
    float velocity = 0.0f;
    float pitchBendFactor = 1.0f;
    float modWheelValue = 0.0f;

    // Orient control: scales vibrato depth and maqam micro-tuning
    float orientAmount = 0.5f;

    // Depth control: scales envelope sustain level and harmonic content
    float depthAmount = 0.7f;

    // Filter state
    juce::dsp::IIR::Filter<float> filter;
    float filterCutoff = 4000.0f;
    float filterResonance = 1.0f;

    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalVoice)
};
