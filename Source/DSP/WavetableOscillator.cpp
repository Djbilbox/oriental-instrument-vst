#include "WavetableOscillator.h"
#include "../Utils/Constants.h"

WavetableOscillator::WavetableOscillator()
{
    table.setSize(1, OrientalConstants::WAVETABLE_SIZE);
    generateSineTable();
}

void WavetableOscillator::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updatePhaseIncrement();
}

void WavetableOscillator::setFrequency(float frequency)
{
    currentFrequency = frequency;
    updatePhaseIncrement();
}

void WavetableOscillator::setWavetable(const juce::AudioBuffer<float>& wavetable)
{
    table.makeCopyOf(wavetable);
}

void WavetableOscillator::generateSineTable()
{
    auto* data = table.getWritePointer(0);
    const int size = table.getNumSamples();
    for (int i = 0; i < size; ++i)
        data[i] = std::sin(2.0f * juce::MathConstants<float>::pi * static_cast<float>(i) / static_cast<float>(size));
}

void WavetableOscillator::generateSawtoothTable()
{
    auto* data = table.getWritePointer(0);
    const int size = table.getNumSamples();
    const int numHarmonics = 32;
    std::fill(data, data + size, 0.0f);

    for (int h = 1; h <= numHarmonics; ++h)
    {
        float amp = 1.0f / static_cast<float>(h);
        for (int i = 0; i < size; ++i)
            data[i] += amp * std::sin(2.0f * juce::MathConstants<float>::pi * static_cast<float>(h * i) / static_cast<float>(size));
    }

    // Normalize
    float maxVal = 0.0f;
    for (int i = 0; i < size; ++i)
        maxVal = std::max(maxVal, std::abs(data[i]));
    if (maxVal > 0.0f)
        for (int i = 0; i < size; ++i)
            data[i] /= maxVal;
}

void WavetableOscillator::generateTriangleTable()
{
    auto* data = table.getWritePointer(0);
    const int size = table.getNumSamples();
    const int numHarmonics = 16;
    std::fill(data, data + size, 0.0f);

    for (int h = 0; h < numHarmonics; ++h)
    {
        int n = 2 * h + 1;
        float amp = (h % 2 == 0 ? 1.0f : -1.0f) / static_cast<float>(n * n);
        for (int i = 0; i < size; ++i)
            data[i] += amp * std::sin(2.0f * juce::MathConstants<float>::pi * static_cast<float>(n * i) / static_cast<float>(size));
    }

    float maxVal = 0.0f;
    for (int i = 0; i < size; ++i)
        maxVal = std::max(maxVal, std::abs(data[i]));
    if (maxVal > 0.0f)
        for (int i = 0; i < size; ++i)
            data[i] /= maxVal;
}

void WavetableOscillator::generateSquareTable()
{
    auto* data = table.getWritePointer(0);
    const int size = table.getNumSamples();
    const int numHarmonics = 32;
    std::fill(data, data + size, 0.0f);

    for (int h = 0; h < numHarmonics; ++h)
    {
        int n = 2 * h + 1;
        float amp = 1.0f / static_cast<float>(n);
        for (int i = 0; i < size; ++i)
            data[i] += amp * std::sin(2.0f * juce::MathConstants<float>::pi * static_cast<float>(n * i) / static_cast<float>(size));
    }

    float maxVal = 0.0f;
    for (int i = 0; i < size; ++i)
        maxVal = std::max(maxVal, std::abs(data[i]));
    if (maxVal > 0.0f)
        for (int i = 0; i < size; ++i)
            data[i] /= maxVal;
}

void WavetableOscillator::generateFromHarmonics(const std::vector<float>& harmonics)
{
    auto* data = table.getWritePointer(0);
    const int size = table.getNumSamples();
    std::fill(data, data + size, 0.0f);

    for (size_t h = 0; h < harmonics.size(); ++h)
    {
        float amp = harmonics[h];
        int harmNum = static_cast<int>(h) + 1;
        for (int i = 0; i < size; ++i)
            data[i] += amp * std::sin(2.0f * juce::MathConstants<float>::pi * static_cast<float>(harmNum * i) / static_cast<float>(size));
    }

    float maxVal = 0.0f;
    for (int i = 0; i < size; ++i)
        maxVal = std::max(maxVal, std::abs(data[i]));
    if (maxVal > 0.0f)
        for (int i = 0; i < size; ++i)
            data[i] /= maxVal;
}

float WavetableOscillator::getNextSample()
{
    const int size = table.getNumSamples();
    const auto* data = table.getReadPointer(0);

    // Linear interpolation
    int index0 = static_cast<int>(phase);
    int index1 = (index0 + 1) % size;
    float frac = phase - static_cast<float>(index0);

    float sample = data[index0] + frac * (data[index1] - data[index0]);

    phase += phaseIncrement;
    if (phase >= static_cast<float>(size))
        phase -= static_cast<float>(size);

    return sample;
}

void WavetableOscillator::reset()
{
    phase = 0.0f;
}

void WavetableOscillator::updatePhaseIncrement()
{
    const int size = table.getNumSamples();
    phaseIncrement = static_cast<float>(currentFrequency * static_cast<float>(size) / currentSampleRate);
}
