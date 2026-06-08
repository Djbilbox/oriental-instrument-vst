#pragma once
#include "FXChain.h"

class ReverbFX : public FXBase
{
public:
    ReverbFX() = default;
    ~ReverbFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

    void setRoomSize(float size);
    void setDamping(float damping);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params;
};
