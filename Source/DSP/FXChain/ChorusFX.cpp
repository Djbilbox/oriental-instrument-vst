#include "ChorusFX.h"

void ChorusFX::prepare(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    chorus.prepare(spec);
    chorus.setRate(0.8f);
    chorus.setDepth(0.3f);
    chorus.setCentreDelay(7.0f);
    chorus.setFeedback(0.2f);
    chorus.setMix(0.5f);
}

void ChorusFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    chorus.process(context);
}

void ChorusFX::reset()
{
    chorus.reset();
}

void ChorusFX::setAmount(float amt)
{
    amount = amt;
    chorus.setMix(amt);
}

void ChorusFX::setRate(float rate)
{
    chorus.setRate(rate);
}

void ChorusFX::setDepth(float depth)
{
    chorus.setDepth(depth);
}
