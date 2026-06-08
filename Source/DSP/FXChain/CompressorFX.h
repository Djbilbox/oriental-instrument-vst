#pragma once
#include "FXChain.h"

class CompressorFX : public FXBase
{
public:
    CompressorFX() = default;
    ~CompressorFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

private:
    juce::dsp::Compressor<float> compressor;
};
