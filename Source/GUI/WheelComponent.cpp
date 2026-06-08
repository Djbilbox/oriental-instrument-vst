#include "WheelComponent.h"
#include "../Utils/Constants.h"

using namespace OrientalConstants;

WheelComponent::WheelComponent(Type type, const juce::String& label)
    : wheelType(type), labelText(label)
{
}

void WheelComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background: same as piano area rgba(5,4,2,0.96)
    g.setColour(juce::Colour(0xF5050402));
    g.fillRect(bounds);
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.drawRect(bounds, 1.0f);

    // Top label: font-size 8px, color var(--gold-dim), letter-spacing .2em
    auto topLabelArea = bounds.removeFromTop(14.0f);
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.setFont(juce::Font("Cinzel", 8.0f, juce::Font::plain));
    g.drawText(labelText, topLabelArea, juce::Justification::centred);

    // Track area: border: 1px solid #2a2a2a, border-radius: 3px, background: #111
    auto trackBounds = bounds.reduced(8.0f, 4.0f);
    g.setColour(juce::Colour(Colors::WHEEL_TRACK));
    g.fillRoundedRectangle(trackBounds, 3.0f);
    g.setColour(juce::Colour(Colors::WHEEL_TRACK_BORDER));
    g.drawRoundedRectangle(trackBounds, 3.0f, 1.0f);

    // Track texture: repeating lines (from CSS ::before)
    g.setColour(juce::Colours::white.withAlpha(0.04f));
    for (float y = trackBounds.getY() + 6.0f; y < trackBounds.getBottom(); y += 7.0f)
        g.fillRect(trackBounds.getX() + 1.0f, y, trackBounds.getWidth() - 2.0f, 1.0f);

    // Center line for pitch bend (red, opacity 0.7)
    if (wheelType == Type::PitchBend)
    {
        float centerY = trackBounds.getCentreY();
        g.setColour(juce::Colour(Colors::RED).withAlpha(0.7f));
        g.fillRect(trackBounds.getX(), centerY - 1.0f, trackBounds.getWidth(), 2.0f);
    }

    // Fill zone
    float normalizedPos;
    if (wheelType == Type::PitchBend)
        normalizedPos = 0.5f - currentValue * 0.4f;
    else
        normalizedPos = 1.0f - currentValue * 0.8f;

    // Mod wheel fill: linear-gradient gold from bottom
    if (wheelType == Type::ModWheel && currentValue > 0.01f)
    {
        float fillHeight = trackBounds.getHeight() * currentValue * 0.8f;
        auto fillBounds = trackBounds.withTop(trackBounds.getBottom() - fillHeight);
        juce::ColourGradient fillGrad(juce::Colour(Colors::GOLD).withAlpha(0.5f), 0, fillBounds.getBottom(),
                                       juce::Colour(Colors::GOLD).withAlpha(0.15f), 0, fillBounds.getY(), false);
        g.setGradientFill(fillGrad);
        g.fillRoundedRectangle(fillBounds.reduced(1.0f, 0), 2.0f);
    }

    // Pitch bend fill
    if (wheelType == Type::PitchBend && std::abs(currentValue) > 0.01f)
    {
        float centerY = trackBounds.getCentreY();
        if (currentValue > 0.0f)
        {
            float fillH = trackBounds.getHeight() * 0.4f * currentValue;
            auto fillBounds = juce::Rectangle<float>(trackBounds.getX() + 1.0f, centerY - fillH,
                                                      trackBounds.getWidth() - 2.0f, fillH);
            g.setColour(juce::Colour(Colors::GOLD).withAlpha(0.3f));
            g.fillRect(fillBounds);
        }
        else
        {
            float fillH = trackBounds.getHeight() * 0.4f * (-currentValue);
            auto fillBounds = juce::Rectangle<float>(trackBounds.getX() + 1.0f, centerY,
                                                      trackBounds.getWidth() - 2.0f, fillH);
            g.setColour(juce::Colour(Colors::RED).withAlpha(0.3f));
            g.fillRect(fillBounds);
        }
    }

    // Thumb: linear-gradient(180deg, #3a3a3a, #1a1a1a), border: 1px solid #555, border-radius: 3px
    // box-shadow: 0 2px 6px rgba(0,0,0,0.7)
    float thumbY = trackBounds.getY() + normalizedPos * (trackBounds.getHeight() - 17.0f);
    auto thumbBounds = juce::Rectangle<float>(trackBounds.getX() + 3.0f, thumbY,
                                               trackBounds.getWidth() - 6.0f, 17.0f);

    // Shadow
    g.setColour(juce::Colour(0xB3000000));
    g.fillRoundedRectangle(thumbBounds.translated(0, 2.0f), 3.0f);

    // Thumb body
    juce::ColourGradient thumbGrad(juce::Colour(Colors::WHEEL_THUMB_TOP), 0, thumbBounds.getY(),
                                    juce::Colour(Colors::WHEEL_THUMB_BOT), 0, thumbBounds.getBottom(), false);
    g.setGradientFill(thumbGrad);
    g.fillRoundedRectangle(thumbBounds, 3.0f);
    g.setColour(juce::Colour(Colors::WHEEL_THUMB_BORDER));
    g.drawRoundedRectangle(thumbBounds, 3.0f, 1.0f);

    // Thumb lines (3 gold lines, width 18px, height 1.5px)
    g.setColour(juce::Colour(Colors::GOLD).withAlpha(0.9f));
    float lineW = 18.0f;
    float lineX = thumbBounds.getCentreX() - lineW * 0.5f;
    for (int i = -1; i <= 1; ++i)
    {
        float lineY = thumbBounds.getCentreY() + static_cast<float>(i) * 3.0f;
        g.fillRoundedRectangle(lineX, lineY - 0.75f, lineW, 1.5f, 1.0f);
    }

    // Bottom value + label
    auto bottomArea = bounds.removeFromBottom(24.0f);
    g.setColour(juce::Colour(Colors::GOLD));
    g.setFont(10.0f);
    int displayVal = static_cast<int>(currentValue * 100.0f);
    g.drawText(juce::String(displayVal), bottomArea.removeFromTop(12.0f), juce::Justification::centred);

    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.setFont(juce::Font("Cinzel", 8.0f, juce::Font::plain));
    juce::String bottomLabel = (wheelType == Type::PitchBend) ? "BEND" : "WHEEL";
    g.drawText(bottomLabel, bottomArea, juce::Justification::centred);
}

void WheelComponent::mouseDown(const juce::MouseEvent& e)
{
    dragStartY = e.y;
    dragStartValue = currentValue;
}

void WheelComponent::mouseDrag(const juce::MouseEvent& e)
{
    float delta = static_cast<float>(dragStartY - e.y) / static_cast<float>(getHeight()) * 1.2f;

    if (wheelType == Type::PitchBend)
        currentValue = juce::jlimit(-1.0f, 1.0f, dragStartValue + delta * 2.0f);
    else
        currentValue = juce::jlimit(0.0f, 1.0f, dragStartValue + delta);

    if (onValueChange)
        onValueChange(currentValue);

    repaint();
}

void WheelComponent::mouseUp(const juce::MouseEvent&)
{
    if (wheelType == Type::PitchBend)
        springReturn();
}

void WheelComponent::springReturn()
{
    currentValue = 0.0f;
    if (onValueChange)
        onValueChange(currentValue);
    repaint();
}
