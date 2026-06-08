#include "PhaserFX.h"

void PhaserFX::prepare(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    phaser.prepare(spec);
    phaser.setRate(0.5f);
    phaser.setDepth(0.5f);
    phaser.setCentreFrequency(1000.0f);
    phaser.setFeedback(0.4f);
    phaser.setMix(0.5f);
}

void PhaserFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    phaser.process(context);
}

void PhaserFX::reset()
{
    phaser.reset();
}

void PhaserFX::setAmount(float amt)
{
    amount = amt;
    phaser.setMix(amt);
    phaser.setDepth(amt * 0.8f);
}
