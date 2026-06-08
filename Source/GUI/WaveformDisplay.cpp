#include "WaveformDisplay.h"
#include "../Utils/Constants.h"

WaveformDisplay::WaveformDisplay()
{
    startTimerHz(30);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0x4D000000));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);

    float w = static_cast<float>(getWidth());
    float h = static_cast<float>(getHeight());
    float midY = h / 2.0f;

    // Gold waveform
    juce::Path path;
    for (int x = 0; x <= getWidth(); ++x)
    {
        float t = (static_cast<float>(x) / w) * juce::MathConstants<float>::pi * 8.0f + phase;
        float y = midY + std::sin(t) * 5.0f + std::sin(t * 2.3f + 1.0f) * 3.0f + std::sin(t * 0.7f) * 4.0f;
        if (x == 0) path.startNewSubPath(static_cast<float>(x), y);
        else path.lineTo(static_cast<float>(x), y);
    }

    g.setColour(juce::Colour(OrientalConstants::Colors::GOLD));
    g.strokePath(path, juce::PathStrokeType(1.5f));
}

void WaveformDisplay::timerCallback()
{
    phase += 0.038f;
    repaint();
}
