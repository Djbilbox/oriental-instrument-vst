#pragma once
#include "FXChain.h"

class ChorusFX : public FXBase
{
public:
    ChorusFX() = default;
    ~ChorusFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

    void setRate(float rate);
    void setDepth(float depth);

private:
    juce::dsp::Chorus<float> chorus;
};
