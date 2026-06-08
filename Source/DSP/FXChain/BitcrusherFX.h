#pragma once
#include "FXChain.h"

class BitcrusherFX : public FXBase
{
public:
    BitcrusherFX() = default;
    ~BitcrusherFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

private:
    float bitDepth = 16.0f;
    float rateReduction = 1.0f;
    float holdSample = 0.0f;
    int holdCounter = 0;
};
