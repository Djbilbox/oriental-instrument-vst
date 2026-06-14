/*
  ==============================================================================
    AndroidUIAdapter.cpp - Implementation
  ==============================================================================
*/

#include "AndroidUIAdapter.h"
#include "Mobile/IAP/AndroidIAPManager.h"

AndroidUIAdapter::AndroidUIAdapter(
    juce::Component* editor,
    AndroidIAPManager* iap)
    : editorComponent(editor), iapManager(iap)
{
    jassert(editor != nullptr);

    if (editorComponent)
        addAndMakeVisible(*editorComponent);

    updateScreenMetrics();
    applyResponsiveLayout();
}

AndroidUIAdapter::~AndroidUIAdapter() = default;

void AndroidUIAdapter::updateScreenMetrics()
{
    auto bounds = getLocalBounds();
    screenWidth = bounds.getWidth();
    screenHeight = bounds.getHeight();

    isPortrait = screenHeight > screenWidth;

    // Safe area insets (typical Android)
    safeAreaTop = 24;    // Status bar (mdpi baseline)
    safeAreaBottom = 48; // Navigation bar

    // Scale content based on screen size
    if (screenWidth < 480)
    {
        scale = 0.75f; // Small phones
    }
    else if (screenWidth < 600)
    {
        scale = 0.85f; // Normal phones
    }
    else if (screenWidth < 800)
    {
        scale = 0.95f; // Large phones
    }
    else
    {
        scale = 1.0f; // Tablets
    }

    // Hide non-essential UI on small screens
    hideFXPanel = screenWidth < 480;
    hideWaveform = screenWidth < 400;
    showIAPBanner = true; // Always show for freemium
}

void AndroidUIAdapter::applyResponsiveLayout()
{
    auto bounds = getLocalBounds();

    if (editorComponent)
    {
        // Reserve space for IAP banner at bottom
        int iapBannerHeight = showIAPBanner ? 56 : 0;
        auto editorBounds = bounds.withBottom(bounds.getBottom() - iapBannerHeight);

        // Center the editor with safe area insets
        editorBounds = editorBounds.reduced(4).withTrimmedTop(safeAreaTop);

        // Apply scaling around center if needed
        if (scale < 1.0f)
        {
            int scaledWidth = juce::roundToInt(editorBounds.getWidth() * scale);
            int scaledHeight = juce::roundToInt(editorBounds.getHeight() * scale);

            auto centeredBounds = editorBounds.withSizeKeepingCentre(
                scaledWidth, scaledHeight);

            editorComponent->setBounds(centeredBounds);
        }
        else
        {
            editorComponent->setBounds(editorBounds);
        }
    }
}

void AndroidUIAdapter::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xFF0A0A0A));

    // Draw IAP banner at bottom
    if (showIAPBanner)
    {
        drawIAPBanner(g);
    }
}

void AndroidUIAdapter::resized()
{
    updateScreenMetrics();
    applyResponsiveLayout();
}

void AndroidUIAdapter::drawIAPBanner(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    int bannerHeight = 56;
    auto bannerRect = bounds.withTop(bounds.getBottom() - bannerHeight);

    // Semi-transparent background
    g.setColour(juce::Colour(0xFF1A1A1A).withAlpha(0.95f));
    g.fillRect(bannerRect);

    // Border
    g.setColour(juce::Colour(0xFFC8922A).withAlpha(0.5f));
    g.drawLine(
        static_cast<float>(bannerRect.getX()),
        static_cast<float>(bannerRect.getY()),
        static_cast<float>(bannerRect.getRight()),
        static_cast<float>(bannerRect.getY()),
        2.0f);

    // Text + button
    g.setColour(juce::Colour(0xFFCCCCCC));
    g.setFont(juce::Font(14.0f).boldened());

    juce::String bannerText = "DEMO MODE: Tap to unlock full version";
    g.drawText(bannerText, bannerRect.reduced(12), juce::Justification::centredLeft);

    // Unlock button on right side
    auto btnRect = bannerRect.removeFromRight(80).reduced(6);
    g.setColour(juce::Colour(0xFFDC1E1E));
    g.fillRoundedRectangle(
        static_cast<float>(btnRect.getX()),
        static_cast<float>(btnRect.getY()),
        static_cast<float>(btnRect.getWidth()),
        static_cast<float>(btnRect.getHeight()),
        4.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(12.0f).boldened());
    g.drawText("UNLOCK", btnRect, juce::Justification::centred);
}
