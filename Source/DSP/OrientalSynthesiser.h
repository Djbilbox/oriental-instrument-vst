#pragma once
#include <JuceHeader.h>
#include "OrientalVoice.h"
#include "MaqamTuning.h"
#include "../Data/InstrumentProfiles.h"
#include "../Utils/Constants.h"

class OrientalSynthesiser
{
public:
    OrientalSynthesiser();
    ~OrientalSynthesiser() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                         int startSample, int numSamples);

    void setInstrument(OrientalConstants::Instrument instrument);
    void setMaqam(MaqamTuning::Maqam maqam);
    void setRootNote(int midiNote);
    void setBaseTuning(float tuningHz);

    // Macro controls from UI
    void setGlide(float glideTime);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float resonance);
    void setOrientAmount(float amount);
    void setDepth(float depth);

    // Wheels
    void setPitchWheel(int pitchWheelValue);
    void setModWheel(float normalizedValue);

    void noteOn(int channel, int midiNote, float velocity);
    void noteOff(int channel, int midiNote, float velocity);
    void allNotesOff();

private:
    juce::Synthesiser synth;
    MaqamTuning maqamTuning;
    OrientalConstants::Instrument currentInstrument = OrientalConstants::Instrument::Violin;

    double currentSampleRate = 44100.0;
    float currentFilterCutoff = 4000.0f;
    float currentFilterQ = 1.0f;

    void updateVoices();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalSynthesiser)
};
