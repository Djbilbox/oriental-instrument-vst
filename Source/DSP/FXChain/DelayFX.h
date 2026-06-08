#pragma once
#include "FXChain.h"

class DelayFX : public FXBase
{
public:
    DelayFX() = default;
    ~DelayFX() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void setAmount(float amount) override;

    void setDelayTime(float timeMs);
    void setFeedback(float feedback);

private:
    juce::dsp::DelayLine<float> delayLine { 88200 };
    float delayTimeMs = 350.0f;
    float feedback = 0.4f;
    double sampleRate = 44100.0;
};
