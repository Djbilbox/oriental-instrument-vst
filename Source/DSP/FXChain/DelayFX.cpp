#include "DelayFX.h"

void DelayFX::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    delayLine.prepare(spec);
}

void DelayFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;

    float delaySamples = static_cast<float>(delayTimeMs * sampleRate / 1000.0);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float delayed = delayLine.popSample(channel, delaySamples);
            float input = data[i] + delayed * feedback;
            delayLine.pushSample(channel, input);
            data[i] += delayed * amount;
        }
    }
}

void DelayFX::reset()
{
    delayLine.reset();
}

void DelayFX::setAmount(float amt)
{
    amount = amt;
}

void DelayFX::setDelayTime(float timeMs)
{
    delayTimeMs = timeMs;
}

void DelayFX::setFeedback(float fb)
{
    feedback = juce::jlimit(0.0f, 0.95f, fb);
}
