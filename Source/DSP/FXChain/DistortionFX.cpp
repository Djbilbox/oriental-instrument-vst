#include "DistortionFX.h"

void DistortionFX::prepare(double, int) {}

void DistortionFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = data[i] * drive;
            data[i] = std::tanh(input);
        }
    }
}

void DistortionFX::reset() {}

void DistortionFX::setAmount(float amt)
{
    amount = amt;
    drive = 1.0f + amt * 10.0f;
}
