#include "ReverbFX.h"

void ReverbFX::prepare(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    reverb.prepare(spec);
}

void ReverbFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void ReverbFX::reset()
{
    reverb.reset();
}

void ReverbFX::setAmount(float amt)
{
    amount = amt;
    params.wetLevel = amt * 0.8f;
    params.dryLevel = 1.0f - amt * 0.5f;
    reverb.setParameters(params);
}

void ReverbFX::setRoomSize(float size)
{
    params.roomSize = size;
    reverb.setParameters(params);
}

void ReverbFX::setDamping(float damping)
{
    params.damping = damping;
    reverb.setParameters(params);
}
