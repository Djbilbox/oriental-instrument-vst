#pragma once
#include <JuceHeader.h>
#include <array>
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
    void setADSRParameters(const ADSREnvelope::Parameters& params);

    void prepareToPlay(double sampleRate, int samplesPerBlock);

private:
    static constexpr int kUnison = 3;

    // ── Sources ──
    std::array<WavetableOscillator, kUnison> oscillators; // detuned unison stack
    WavetableOscillator subOsc;                            // one octave down (sine)
    WavetableOscillator vibratoLFO;
    float unisonPan[kUnison] = { 0.0f, -1.0f, 1.0f };      // centre, left, right
    float unisonDetune[kUnison] = { 0.0f, -1.0f, 1.0f };   // cents multiplier

    // Noise / breath layer
    juce::Random rng;
    float noiseLP = 0.0f;          // one-pole state for noise colouring
    ADSREnvelope noiseEnv;

    // Envelopes
    ADSREnvelope envelope;         // amplitude
    ADSREnvelope filterEnv;        // cutoff modulation

    // ── Character chain ──
    juce::dsp::StateVariableTPTFilter<float> svfL, svfR;   // resonant multimode filter (stereo)
    juce::dsp::IIR::Filter<float> formant1L, formant1R;    // body resonance
    juce::dsp::IIR::Filter<float> formant2L, formant2R;
    bool hasFormant1 = false, hasFormant2 = false;

    const InstrumentProfile* currentProfile = nullptr;
    MaqamTuning maqamTuning;

    ADSREnvelope::Parameters presetADSR;
    bool hasPresetADSR = false;

    float targetFrequency = 440.0f;
    float currentFrequency = 440.0f;
    float glideRate = 0.0f;
    float velocity = 0.0f;
    float pitchBendFactor = 1.0f;
    float modWheelValue = 0.0f;

    float orientAmount = 0.5f;     // vibrato + detune + movement
    float depthAmount = 0.7f;      // drive / grain intensity

    // Filter macro state (base cutoff/reso from UI)
    float baseCutoff = 4000.0f;
    float baseResonance = 1.0f;

    // Per-note resolved values
    float noteDrive = 1.0f;
    float noteAsym = 0.12f;

    double sampleRate = 44100.0;

    float shape(float x) const;    // asymmetric waveshaper (the "grain")
    void updateFilterCoefficients(float cutoffHz);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalVoice)
};
