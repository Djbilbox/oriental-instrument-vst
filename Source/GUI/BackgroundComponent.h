#pragma once
#include <JuceHeader.h>

// Renders the full backdrop: a cached desert scene, an animated sun halo
// (30 fps Timer), frosted glass panels, and the window chrome (header, section
// labels, dividers). It sits behind every control, so all "glass" relief lives
// here rather than in the editor's paint().
class BackgroundComponent : public juce::Component,
                            private juce::Timer
{
public:
    BackgroundComponent();
    ~BackgroundComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    // Cache management
    void rebuildCaches();
    juce::Image cachedDesert;   // sharp scene (no live halo)
    juce::Image cachedFrosted;  // blurred scene for glass panels
    juce::Rectangle<int> cachedSize;

    // Animation
    float haloPhase = 0.0f;

    // Scene painters (into the cache)
    void drawDesertGradient(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawStars(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSun(juce::Graphics& g, float cx, float cy, float intensity);
    void drawPyramids(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawDunes(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawVignette(juce::Graphics& g, juce::Rectangle<float> bounds);

    // Live compositing
    void drawGlassPanel(juce::Graphics& g, juce::Rectangle<int> r, bool topEdge, bool leftEdge);
    void drawGlassPanels(juce::Graphics& g);
    void drawChrome(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackgroundComponent)
};
