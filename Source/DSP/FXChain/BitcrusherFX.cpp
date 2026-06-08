#include "BitcrusherFX.h"

void BitcrusherFX::prepare(double, int) {}

void BitcrusherFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;

    int rateDiv = static_cast<int>(1.0f + rateReduction * 30.0f);
    float levels = std::pow(2.0f, bitDepth);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            if (holdCounter == 0)
            {
                holdSample = std::round(data[i] * levels) / levels;
                holdCounter = rateDiv;
            }
            holdCounter--;
            data[i] = holdSample;
        }
    }
}

void BitcrusherFX::reset()
{
    holdSample = 0.0f;
    holdCounter = 0;
}

void BitcrusherFX::setAmount(float amt)
{
    amount = amt;
    bitDepth = 16.0f - amt * 12.0f; // 16-bit down to 4-bit
    rateReduction = amt;
}
