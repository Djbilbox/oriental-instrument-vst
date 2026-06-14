#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"

// Mobile UI controller. Handles touch, layout, and platform-specific UI events.
class MobileUIController : public juce::Component,
                           public juce::Timer
{
public:
    MobileUIController(OrientalInstrumentProcessor& proc);
    ~MobileUIController() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Touch handling
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    // Screen rotation / layout
    virtual void onOrientationChanged(bool isPortrait);
    virtual void onScreenResized(int width, int height);

    // Virtual methods for platform-specific layout
    virtual void layoutPortrait(int w, int h);
    virtual void layoutLandscape(int w, int h);

protected:
    OrientalInstrumentProcessor& processor;

    juce::Rectangle<int> keyboardArea;
    juce::Rectangle<int> controlsArea;
    juce::Rectangle<int> presetArea;

    bool isPortraitMode = true;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MobileUIController)
};
