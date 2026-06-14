/*
  ==============================================================================
    AndroidUIAdapter.h - Responsive UI Wrapper for Mobile Screens
  ==============================================================================

    Adapts the desktop-oriented GUI (860x540) to mobile screen sizes.
    - Scales and pans content to fit various screen densities (120-640 dpi)
    - Hides non-essential controls on small screens
    - Makes BackgroundComponent responsive (optimized for portrait)
    - Handles status bar / safe area insets
    - Integrates IAP banner for unlocking full version (freemium)
*/

#pragma once

#include <JuceHeader.h>

class AndroidIAPManager; // Forward declare

/**
    Wraps the main OrientalInstrumentEditor to make it responsive on Android.
    Applies scaling, hiding, and layout adjustments based on screen size.
*/
class AndroidUIAdapter : public juce::Component
{
public:
    /**
        @param editor The main editor (OrientalInstrumentEditor)
        @param iapManager Handles in-app purchase interactions (nullable)
    */
    AndroidUIAdapter(
        juce::Component* editor,
        AndroidIAPManager* iapManager = nullptr);

    ~AndroidUIAdapter() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<juce::Component> editorComponent;
    AndroidIAPManager* iapManager;

    // Screen metrics
    int screenWidth = 0;
    int screenHeight = 0;
    bool isPortrait = true;
    float scale = 1.0f;
    int safeAreaTop = 0;    // status bar height
    int safeAreaBottom = 0; // nav bar height

    // Layout state
    bool showIAPBanner = false;
    bool hideFXPanel = false;
    bool hideWaveform = false;

    void updateScreenMetrics();
    void applyResponsiveLayout();

    /**
        Draw semi-transparent banner at the bottom prompting the user
        to unlock the full version. Only shown in DEMO mode.
    */
    void drawIAPBanner(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AndroidUIAdapter)
};
