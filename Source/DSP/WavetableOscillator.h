#pragma once
#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator();
    ~WavetableOscillator() = default;

    void prepare(double sampleRate);
    void setFrequency(float frequency);
    void setWavetable(const juce::AudioBuffer<float>& wavetable);

    void generateSineTable();
    void generateSawtoothTable();
    void generateTriangleTable();
    void generateSquareTable();
    void generateFromHarmonics(const std::vector<float>& harmonics);

    float getNextSample();
    void reset();

private:
    juce::AudioBuffer<float> table;
    double currentSampleRate = 44100.0;
    float currentFrequency = 440.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;

    void updatePhaseIncrement();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableOscillator)
};
