#include "MobileUIController.h"

MobileUIController::MobileUIController(OrientalInstrumentProcessor& proc)
    : processor(proc)
{
    startTimer(30); // ~33ms update rate
}

void MobileUIController::paint(juce::Graphics& g)
{
    // Clear background
    g.fillAll(juce::Colours::black);

    // Placeholder desert background (simplified from desktop version)
    auto bounds = getLocalBounds().toFloat();

    // Sky gradient
    juce::ColourGradient skyGradient(
        juce::Colour::fromHSV(0.08f, 0.3f, 0.9f, 1.0f),
        0, 0,
        juce::Colour::fromHSV(0.09f, 0.5f, 0.7f, 1.0f),
        0, bounds.getHeight() * 0.3f,
        false
    );
    g.setGradientFill(skyGradient);
    g.fillRect(bounds.removeFromTop(bounds.getHeight() * 0.3f));

    // Sand/desert
    g.setColour(juce::Colour::fromHSV(0.11f, 0.2f, 0.75f));
    g.fillRect(bounds);
}

void MobileUIController::resized()
{
    auto bounds = getLocalBounds();
    int width = bounds.getWidth();
    int height = bounds.getHeight();
    bool nowPortrait = height > width;

    if (nowPortrait != isPortraitMode)
    {
        isPortraitMode = nowPortrait;
        onOrientationChanged(isPortraitMode);
    }

    if (isPortraitMode)
        layoutPortrait(width, height);
    else
        layoutLandscape(width, height);
}

void MobileUIController::layoutPortrait(int w, int h)
{
    // Portrait layout: keyboard at bottom, controls at top
    presetArea = { 0, 0, w, h / 6 };
    controlsArea = { 0, h / 6, w, h / 2 };
    keyboardArea = { 0, h / 2, w, h / 2 };
}

void MobileUIController::layoutLandscape(int w, int h)
{
    // Landscape: keyboard on left, controls on right
    keyboardArea = { 0, 0, w / 2, h };
    controlsArea = { w / 2, 0, w / 2, h };
    presetArea = { w / 2, 0, w / 2, h / 5 };
}

void MobileUIController::timerCallback()
{
    // Periodic UI update (e.g., animate controls, update displays)
    repaint();
}

void MobileUIController::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.getPosition();

    // Handle keyboard click in keyboardArea
    if (keyboardArea.contains(pos))
    {
        // TODO: map pixel -> MIDI note
    }

    // Handle knob/control drag in controlsArea
    if (controlsArea.contains(pos))
    {
        // TODO: handle parameter dragging
    }
}

void MobileUIController::mouseDrag(const juce::MouseEvent& e)
{
    // Vertical drag adjusts parameter values
}

void MobileUIController::mouseUp(const juce::MouseEvent& e)
{
    // End parameter adjustment
}

void MobileUIController::onOrientationChanged(bool isPortrait)
{
    // Platform-specific handling (audio session, layout hints)
    resized();
}

void MobileUIController::onScreenResized(int width, int height)
{
    setBounds(0, 0, width, height);
}
