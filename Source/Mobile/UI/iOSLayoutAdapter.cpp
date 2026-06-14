#include "iOSLayoutAdapter.h"

#ifdef JUCE_IOS
    #include <UIKit/UIKit.h>
#endif

iOSLayoutAdapter::Orientation iOSLayoutAdapter::getCurrentOrientation()
{
#ifdef JUCE_IOS
    UIDeviceOrientation deviceOrientation = [[UIDevice currentDevice] orientation];
    if (UIDeviceOrientationIsLandscape(deviceOrientation))
        return Orientation::Landscape;
    return Orientation::Portrait;
#else
    return Orientation::Portrait;
#endif
}

bool iOSLayoutAdapter::isPortrait()
{
    return getCurrentOrientation() == Orientation::Portrait;
}

bool iOSLayoutAdapter::isLandscape()
{
    return getCurrentOrientation() == Orientation::Landscape;
}

float iOSLayoutAdapter::getScaleFactor(juce::Rectangle<int> screenBounds)
{
    float screenAspect = static_cast<float>(screenBounds.getWidth()) / screenBounds.getHeight();

    // In portrait: screen is tall (aspect < 1), desktop is wide (aspect > 1)
    // Scale to fit width
    if (isPortrait())
    {
        return screenBounds.getWidth() / 980.0f;
    }
    else // landscape
    {
        // In landscape: screen is wider, but still may be smaller than 980x640
        // Scale to fit height (since height becomes constrained)
        return screenBounds.getHeight() / 640.0f;
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getPresetBrowserBounds(juce::Rectangle<int> screenBounds,
                                                               Orientation orientation)
{
    int safeLeft = 8;
    int safeTop = 8;
    int safeRight = 8;
    int safeBottom = 8;

    if (orientation == Orientation::Portrait)
    {
        // Portrait: preset browser takes left column
        int width = (screenBounds.getWidth() - safeLeft - safeRight) / 2;
        int height = screenBounds.getHeight() - safeTop - safeBottom;
        return juce::Rectangle<int>(safeLeft, safeTop, width, height);
    }
    else
    {
        // Landscape: preset browser takes top row
        int width = screenBounds.getWidth() - safeLeft - safeRight;
        int height = (screenBounds.getHeight() - safeTop - safeBottom) / 3;
        return juce::Rectangle<int>(safeLeft, safeTop, width, height);
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getFXPanelBounds(juce::Rectangle<int> screenBounds,
                                                         Orientation orientation)
{
    int safeLeft = 8;
    int safeTop = 8;
    int safeRight = 8;
    int safeBottom = 8;

    if (orientation == Orientation::Portrait)
    {
        // Portrait: FX panel on right side
        int x = screenBounds.getWidth() / 2 + safeLeft;
        int width = screenBounds.getWidth() / 2 - safeLeft - safeRight;
        int height = screenBounds.getHeight() - safeTop - safeBottom;
        return juce::Rectangle<int>(x, safeTop, width, height);
    }
    else
    {
        // Landscape: FX panel below preset browser
        int width = screenBounds.getWidth() - safeLeft - safeRight;
        int y = (screenBounds.getHeight() - safeTop - safeBottom) / 3 + safeTop;
        int height = (screenBounds.getHeight() - safeTop - safeBottom) * 2 / 3;
        return juce::Rectangle<int>(safeLeft, y, width, height);
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getWheelsBounds(juce::Rectangle<int> screenBounds,
                                                        Orientation orientation)
{
    // Wheels (pitch + mod) should be centered and visible
    // In portrait: below or alongside other controls
    // In landscape: might be in a side panel

    if (orientation == Orientation::Portrait)
    {
        int size = std::min(200, screenBounds.getWidth() / 3);
        int x = (screenBounds.getWidth() - size * 2 - 16) / 2;
        int y = screenBounds.getHeight() - size - 50;
        return juce::Rectangle<int>(x, y, size * 2 + 16, size + 20);
    }
    else
    {
        int size = std::min(150, screenBounds.getHeight() / 2);
        int x = screenBounds.getWidth() - size * 2 - 24;
        int y = (screenBounds.getHeight() - size) / 2;
        return juce::Rectangle<int>(x, y, size * 2 + 16, size + 20);
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getPianoBounds(juce::Rectangle<int> screenBounds,
                                                       Orientation orientation)
{
    int safeLeft = 8;
    int safeRight = 8;
    int safeBottom = 8;

    if (orientation == Orientation::Portrait)
    {
        // Piano occupies bottom quarter in portrait
        int y = screenBounds.getHeight() - 120 - safeBottom;
        int width = screenBounds.getWidth() - safeLeft - safeRight;
        return juce::Rectangle<int>(safeLeft, y, width, 120);
    }
    else
    {
        // Piano on the side in landscape (narrow vertical strip)
        int x = screenBounds.getWidth() - 100;
        int height = screenBounds.getHeight();
        return juce::Rectangle<int>(x, 0, 100, height);
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getKnobsBounds(juce::Rectangle<int> screenBounds,
                                                       Orientation orientation)
{
    int safeLeft = 8;
    int safeRight = 8;
    int safeTop = 8;

    if (orientation == Orientation::Portrait)
    {
        // Knobs in a grid below the main display
        int width = screenBounds.getWidth() - safeLeft - safeRight;
        int y = (screenBounds.getHeight() * 2) / 3;
        int height = screenBounds.getHeight() / 3 - safeTop - 50;
        return juce::Rectangle<int>(safeLeft, y, width, height);
    }
    else
    {
        // Knobs in a vertical column on the left in landscape
        int width = 100;
        int height = screenBounds.getHeight() - safeTop - 8;
        return juce::Rectangle<int>(safeLeft, safeTop, width, height);
    }
}

juce::Rectangle<int> iOSLayoutAdapter::getSafeArea(juce::Rectangle<int> screenBounds)
{
#ifdef JUCE_IOS
    // iOS 11+ safe area insets (notch, home indicator, dynamic island)
    // For now, apply conservative 8px margins
    // In a production app, query UIView's safeAreaInsets directly
    return screenBounds.reduced(8);
#else
    return screenBounds.reduced(8);
#endif
}
