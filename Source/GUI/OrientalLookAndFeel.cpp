#include "OrientalLookAndFeel.h"

using namespace OrientalConstants;

OrientalLookAndFeel::OrientalLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(Colors::GOLD));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(Colors::KNOB_RING));
    setColour(juce::Label::textColourId, juce::Colour(Colors::GOLD));

    // TextEditor (search box)
    setColour(juce::TextEditor::backgroundColourId, juce::Colour(Colors::SEARCH_BG));
    setColour(juce::TextEditor::outlineColourId, juce::Colour(Colors::SEARCH_BORDER));
    setColour(juce::TextEditor::textColourId, juce::Colour(0xFFCCCCCC));
    setColour(juce::TextEditor::highlightColourId, juce::Colour(Colors::GOLD).withAlpha(0.3f));

    // Buttons (tabs)
    setColour(juce::TextButton::buttonColourId, juce::Colour(Colors::TAB_BG));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(Colors::TAB_ON_BG));
    setColour(juce::TextButton::textColourOffId, juce::Colour(Colors::TAB_NAME_OFF));
    setColour(juce::TextButton::textColourOnId, juce::Colour(Colors::TAB_NAME_ON));
}

void OrientalLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                            juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.0f);
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float centreX = bounds.getCentreX();
    float centreY = bounds.getCentreY();
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    bool isRed = (slider.getName().containsIgnoreCase("FX"));
    juce::Colour accent = isRed ? juce::Colour(Colors::RED) : juce::Colour(Colors::GOLD);
    juce::Colour glow = accent.withAlpha(Design::KNOB_GLOW_ALPHA);

    // Outer ring track: stroke-width="5" fill="#1a1a1a"
    g.setColour(juce::Colour(Colors::KNOB_RING));
    g.fillEllipse(centreX - radius - 5.0f, centreY - radius - 5.0f,
                  (radius + 5.0f) * 2.0f, (radius + 5.0f) * 2.0f);

    // Knob body: radialGradient cx=38% cy=32% — #3c3530 → #100d08
    juce::ColourGradient bodyGrad(juce::Colour(Colors::KNOB_HIGHLIGHT),
                                   centreX - radius * 0.24f, centreY - radius * 0.36f,
                                   juce::Colour(Colors::KNOB_SHADOW),
                                   centreX + radius * 0.5f, centreY + radius * 0.5f, true);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    // Knob outline: stroke="#2a2520" stroke-width="1.5"
    g.setColour(juce::Colour(Colors::KNOB_OUTLINE));
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // Highlight arc (top reflection): from SVG path opacity 0.07
    g.setColour(juce::Colours::white.withAlpha(0.07f));
    juce::Path highlight;
    highlight.addCentredArc(centreX, centreY, radius * 0.8f, radius * 0.8f,
                            0.0f, -2.4f, -0.7f, true);
    g.strokePath(highlight, juce::PathStrokeType(1.8f));

    // Active arc: stroke-width="2.8" with glow filter
    float arcRadius = radius + 3.0f;
    juce::Path arcPath;
    arcPath.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                          0.0f, rotaryStartAngle, angle, true);

    // Glow (simulate drop-shadow via thicker stroke behind)
    g.setColour(glow);
    g.strokePath(arcPath, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    // Main arc
    g.setColour(accent);
    g.strokePath(arcPath, juce::PathStrokeType(Design::KNOB_ARC_WIDTH,
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    // Pointer line: stroke-width="2.3" with glow
    float pLen = radius - 9.0f;
    float pAngle = angle - juce::MathConstants<float>::halfPi;
    float px1 = centreX + pLen * 0.4f * std::cos(pAngle);
    float py1 = centreY + pLen * 0.4f * std::sin(pAngle);
    float px2 = centreX + pLen * std::cos(pAngle);
    float py2 = centreY + pLen * std::sin(pAngle);

    g.setColour(glow);
    g.drawLine(px1, py1, px2, py2, 4.0f);
    g.setColour(accent);
    g.drawLine(px1, py1, px2, py2, Design::KNOB_POINTER_WIDTH);

    // Center dot: r=3 black, r=1.4 accent 55%
    g.setColour(juce::Colour(0xFF0A0A0A));
    g.fillEllipse(centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
    g.setColour(accent.withAlpha(0.55f));
    g.fillEllipse(centreX - 1.4f, centreY - 1.4f, 2.8f, 2.8f);
}

void OrientalLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(juce::Font("Inter", 10.0f, juce::Font::bold));
    g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
}

void OrientalLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor&)
{
    // Search box: border-radius: 5px, border: 1px solid var(--gold-dim)
    g.setColour(juce::Colour(Colors::SEARCH_BORDER));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f,
                           static_cast<float>(height) - 1.0f, Design::BORDER_RADIUS_LG, 1.0f);
}

void OrientalLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour&,
                                                bool shouldDrawButtonAsHighlighted,
                                                bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    bool isOn = button.getToggleState();

    // Tab style: glassmorphism with gold border when active
    if (isOn || shouldDrawButtonAsDown)
    {
        g.setColour(juce::Colour(Colors::TAB_ON_BG));
        g.fillRoundedRectangle(bounds, Design::BORDER_RADIUS_LG);
        g.setColour(juce::Colour(Colors::TAB_ON_BORDER));
        g.drawRoundedRectangle(bounds, Design::BORDER_RADIUS_LG, 1.0f);
    }
    else
    {
        g.setColour(juce::Colour(Colors::TAB_BG));
        g.fillRoundedRectangle(bounds, Design::BORDER_RADIUS_LG);
        g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(Colors::GOLD_DIM) : juce::Colour(Colors::TAB_BORDER));
        g.drawRoundedRectangle(bounds, Design::BORDER_RADIUS_LG, 1.0f);
    }
}

void OrientalLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                          bool, bool)
{
    bool isOn = button.getToggleState();
    g.setColour(isOn ? juce::Colour(Colors::TAB_NAME_ON) : juce::Colour(Colors::TAB_NAME_OFF));
    g.setFont(juce::Font("Cinzel", 9.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 1);
}
