#pragma once
#include <JuceHeader.h>

class BackgroundComponent : public juce::Component
{
public:
    BackgroundComponent() = default;
    ~BackgroundComponent() override = default;

    void paint(juce::Graphics& g) override;

private:
    void drawDesertGradient(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawStars(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSun(juce::Graphics& g, float cx, float cy);
    void drawPyramids(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawDunes(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawVignette(juce::Graphics& g, juce::Rectangle<float> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackgroundComponent)
};
