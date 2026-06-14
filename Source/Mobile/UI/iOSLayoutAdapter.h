#pragma once

#include <JuceHeader.h>

/*
    iOSLayoutAdapter.h — Responsive layout helper for portrait/landscape orientations.

    Automatically rescales and repositions GUI components based on device orientation.
    Desktop VST layout (980x640) is portrait-oriented.
    iOS supports both portrait (e.g., 390x844 on iPhone 14) and landscape (844x390).

    Strategy:
    - Portrait: Use original 980x640 layout scaled to fit screen
    - Landscape: Stack controls vertically or use a modified carousel

    The BackgroundComponent (desert scene) is resolution-independent via percentages,
    so it adapts naturally. Control panels (presets, FX, wheels) reposition via this helper.
*/

class iOSLayoutAdapter
{
public:
    enum class Orientation
    {
        Portrait,
        Landscape
    };

    static Orientation getCurrentOrientation();
    static bool isPortrait();
    static bool isLandscape();

    // Scaling factor to fit current screen bounds
    static float getScaleFactor(juce::Rectangle<int> screenBounds);

    // Repositioned bounds for a GUI section
    static juce::Rectangle<int> getPresetBrowserBounds(juce::Rectangle<int> screenBounds,
                                                        Orientation orientation);
    static juce::Rectangle<int> getFXPanelBounds(juce::Rectangle<int> screenBounds,
                                                   Orientation orientation);
    static juce::Rectangle<int> getWheelsBounds(juce::Rectangle<int> screenBounds,
                                                 Orientation orientation);
    static juce::Rectangle<int> getPianoBounds(juce::Rectangle<int> screenBounds,
                                                Orientation orientation);
    static juce::Rectangle<int> getKnobsBounds(juce::Rectangle<int> screenBounds,
                                                Orientation orientation);

    // Safe area insets (notch, home indicator, etc.)
    static juce::Rectangle<int> getSafeArea(juce::Rectangle<int> screenBounds);

private:
    static constexpr float kDesktopAspect = 980.0f / 640.0f; // ~1.53
};
