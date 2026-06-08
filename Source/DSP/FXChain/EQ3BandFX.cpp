#include "EQ3BandFX.h"

void EQ3BandFX::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;
    lowFilter.prepare(spec);
    midFilter.prepare(spec);
    highFilter.prepare(spec);
    updateFilters();
}

void EQ3BandFX::process(juce::AudioBuffer<float>& buffer)
{
    if (!active) return;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float sample = data[i];
            sample = lowFilter.processSample(sample);
            sample = midFilter.processSample(sample);
            sample = highFilter.processSample(sample);
            data[i] = sample;
        }
    }
}

void EQ3BandFX::reset()
{
    lowFilter.reset();
    midFilter.reset();
    highFilter.reset();
}

void EQ3BandFX::setAmount(float amt)
{
    amount = amt;
}

void EQ3BandFX::setLowGain(float gainDb)
{
    lowGain = gainDb;
    updateFilters();
}

void EQ3BandFX::setMidGain(float gainDb)
{
    midGain = gainDb;
    updateFilters();
}

void EQ3BandFX::setHighGain(float gainDb)
{
    highGain = gainDb;
    updateFilters();
}

void EQ3BandFX::updateFilters()
{
    *lowFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 250.0f, 0.707f, juce::Decibels::decibelsToGain(lowGain));
    *midFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 1200.0f, 1.0f, juce::Decibels::decibelsToGain(midGain));
    *highFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 4000.0f, 0.707f, juce::Decibels::decibelsToGain(highGain));
}
