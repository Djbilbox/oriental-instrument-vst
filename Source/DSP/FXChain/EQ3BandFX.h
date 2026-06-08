#pragma once
#include "FXChain.h"

class EQ3BandFX : public FXBase
{
public:
    EQ3BandFX() = default;
    ~EQ3BandFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

    void setLowGain(float gainDb);
    void setMidGain(float gainDb);
    void setHighGain(float gainDb);

private:
    juce::dsp::IIR::Filter<float> lowFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> highFilter;
    double sampleRate = 44100.0;
    float lowGain = 0.0f, midGain = 0.0f, highGain = 0.0f;

    void updateFilters();
};
