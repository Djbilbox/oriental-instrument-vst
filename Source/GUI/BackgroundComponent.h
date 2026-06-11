#pragma once
#include <JuceHeader.h>

// Renders the full backdrop: a cached desert scene (sky, sun, pyramids, sphinx,
// dunes, Gnawa musicians, camel, eagles, palms, heat haze), an animated sun halo
// (30 fps Timer), frosted glass panels, and the window chrome (header, title,
// transport chips, section labels, dividers). It sits behind every control, so
// all "glass" relief lives here rather than in the editor's paint().
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
    juce::Image cachedDesert;   // full sharp scene
    juce::Image cachedFrosted;  // blurred scene for glass panels
    juce::Rectangle<int> cachedSize;

    // Animation
    float haloPhase = 0.0f;

    // Scene painters (into the cache). sx/sy map SVG 980x640 coords to bounds.
    void drawDesertGradient(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawStars(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawSun(juce::Graphics& g, float cx, float cy, float intensity);
    void drawHalo(juce::Graphics& g, float cx, float cy, float alpha);
    void drawPyramids(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawMonument(juce::Graphics& g, float sx, float sy);      // Maqam Echahid, Alger
    void drawAlgerianFlag(juce::Graphics& g, float sx, float sy);
    void drawSphinx(juce::Graphics& g, float sx, float sy);
    void drawDunes(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawMusicians(juce::Graphics& g, float sx, float sy);
    void drawCamel(juce::Graphics& g, float sx, float sy);
    void drawEagles(juce::Graphics& g, float sx, float sy);
    void drawPalms(juce::Graphics& g, float sx, float sy);
    void drawHeatHaze(juce::Graphics& g, float sx, float sy);
    void drawVignette(juce::Graphics& g, juce::Rectangle<float> bounds);

    // Live compositing
    void drawGlassPanel(juce::Graphics& g, juce::Rectangle<int> r, bool topEdge, bool leftEdge);
    void drawGlassPanels(juce::Graphics& g);
    void drawChrome(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackgroundComponent)
};
