#pragma once
#include "FXChain.h"

class PhaserFX : public FXBase
{
public:
    PhaserFX() = default;
    ~PhaserFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

private:
    juce::dsp::Phaser<float> phaser;
};
