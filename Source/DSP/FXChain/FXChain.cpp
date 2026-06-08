#include "FXChain.h"
#include "ReverbFX.h"
#include "DelayFX.h"
#include "ChorusFX.h"
#include "DistortionFX.h"
#include "CompressorFX.h"
#include "EQ3BandFX.h"
#include "PhaserFX.h"
#include "BitcrusherFX.h"

FXChain::FXChain()
{
    effects.push_back(std::make_unique<ReverbFX>());
    effects.push_back(std::make_unique<DelayFX>());
    effects.push_back(std::make_unique<ChorusFX>());
    effects.push_back(std::make_unique<DistortionFX>());
    effects.push_back(std::make_unique<CompressorFX>());
    effects.push_back(std::make_unique<EQ3BandFX>());
    effects.push_back(std::make_unique<PhaserFX>());
    effects.push_back(std::make_unique<BitcrusherFX>());
}

void FXChain::prepare(double sampleRate, int samplesPerBlock)
{
    for (auto& fx : effects)
        fx->prepare(sampleRate, samplesPerBlock);
}

void FXChain::process(juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    for (auto& fx : effects)
    {
        if (fx->isEnabled())
            fx->process(buffer);
    }

    // Dry/Wet mix
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = dryBuffer.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            wet[i] = dry[i] * (1.0f - dryWetMix) + wet[i] * dryWetMix;
    }
}

void FXChain::reset()
{
    for (auto& fx : effects)
        fx->reset();
}

void FXChain::setDryWetMix(float mix)
{
    dryWetMix = juce::jlimit(0.0f, 1.0f, mix);
}
