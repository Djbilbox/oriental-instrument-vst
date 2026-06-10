#include "OrientalLookAndFeel.h"
#include "Typography.h"

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

    const float d = radius * 2.0f;
    const juce::Rectangle<float> body(centreX - radius, centreY - radius, d, d);

    // ── LAYER 1 — outer drop shadow (offset, soft) ──
    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.fillEllipse(body.translated(0.0f, 3.0f).expanded(2.0f));

    // Outer ring track
    g.setColour(juce::Colour(Colors::KNOB_RING));
    g.fillEllipse(centreX - radius - 4.0f, centreY - radius - 4.0f,
                  (radius + 4.0f) * 2.0f, (radius + 4.0f) * 2.0f);

    // ── LAYER 2 — brushed-metal body ──
    juce::ColourGradient bodyGrad(juce::Colour(Colors::KNOB_HIGHLIGHT).brighter(0.15f),
                                   centreX - radius * 0.30f, centreY - radius * 0.45f,
                                   juce::Colour(Colors::KNOB_SHADOW),
                                   centreX + radius * 0.55f, centreY + radius * 0.65f, true);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(body);

    // Brushed streaks — fine radial striations, clipped to the knob disc
    {
        juce::Graphics::ScopedSaveState save(g);
        juce::Path disc;
        disc.addEllipse(body);
        g.reduceClipRegion(disc);
        for (int i = 0; i < 18; ++i)
        {
            float a = juce::MathConstants<float>::twoPi * (static_cast<float>(i) / 18.0f);
            float alpha = (i % 2 == 0) ? 0.05f : 0.025f;
            g.setColour(juce::Colours::white.withAlpha(alpha));
            g.drawLine(centreX, centreY,
                       centreX + std::cos(a) * radius, centreY + std::sin(a) * radius, 0.7f);
        }
    }

    // Inner bevel outline
    g.setColour(juce::Colour(Colors::KNOB_OUTLINE));
    g.drawEllipse(body, 1.5f);
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.drawEllipse(body.reduced(1.5f), 1.0f);

    // ── LAYER 3 — specular highlight (top-left glint) ──
    juce::ColourGradient spec(juce::Colours::white.withAlpha(0.22f),
                              centreX - radius * 0.4f, centreY - radius * 0.55f,
                              juce::Colours::transparentWhite,
                              centreX, centreY, true);
    g.setGradientFill(spec);
    g.fillEllipse(body.reduced(radius * 0.18f));

    juce::Path glint;
    glint.addCentredArc(centreX, centreY, radius * 0.78f, radius * 0.78f, 0.0f, -2.4f, -0.7f, true);
    g.setColour(juce::Colours::white.withAlpha(0.10f));
    g.strokePath(glint, juce::PathStrokeType(1.6f));

    // ── Active value arc with layered glow ──
    float arcRadius = radius + 3.0f;
    juce::Path arcPath;
    arcPath.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);

    for (int layer = 0; layer < 3; ++layer)
    {
        float widths[3] = { 8.0f, 5.5f, 3.5f };
        float alphas[3] = { 0.12f, 0.28f, 0.6f };
        g.setColour(accent.withAlpha(alphas[layer]));
        g.strokePath(arcPath, juce::PathStrokeType(widths[layer], juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }
    g.setColour(accent);
    g.strokePath(arcPath, juce::PathStrokeType(Design::KNOB_ARC_WIDTH, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    // ── Pointer ──
    float pLen = radius - 9.0f;
    float pAngle = angle - juce::MathConstants<float>::halfPi;
    float px1 = centreX + pLen * 0.4f * std::cos(pAngle);
    float py1 = centreY + pLen * 0.4f * std::sin(pAngle);
    float px2 = centreX + pLen * std::cos(pAngle);
    float py2 = centreY + pLen * std::sin(pAngle);

    g.setColour(accent.withAlpha(0.4f));
    g.drawLine(px1, py1, px2, py2, 4.5f);
    g.setColour(accent.brighter(0.3f));
    g.drawLine(px1, py1, px2, py2, Design::KNOB_POINTER_WIDTH);

    // Center cap
    g.setColour(juce::Colour(0xFF0A0A0A));
    g.fillEllipse(centreX - 3.5f, centreY - 3.5f, 7.0f, 7.0f);
    g.setColour(accent.withAlpha(0.6f));
    g.fillEllipse(centreX - 1.5f, centreY - 1.5f, 3.0f, 3.0f);
}

void OrientalLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(Typography::knobLabel());
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
    g.setFont(Typography::tab());
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 1);
}
