#pragma once
#include "FXChain.h"

class DistortionFX : public FXBase
{
public:
    DistortionFX() = default;
    ~DistortionFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

private:
    float drive = 1.0f;
};
