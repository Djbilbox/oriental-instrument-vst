#include "CompressorFX.h"

void CompressorFX::prepare(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    compressor.prepare(spec);
    compressor.setThreshold(-12.0f);
    compressor.setRatio(4.0f);
    compressor.setAttack(10.0f);
    compressor.setRelease(100.0f);
}

void CompressorFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    compressor.process(context);
}

void CompressorFX::reset()
{
    compressor.reset();
}

void CompressorFX::setAmount(float amt)
{
    amount = amt;
    compressor.setThreshold(-6.0f - amt * 18.0f);
    compressor.setRatio(1.0f + amt * 7.0f);
}
