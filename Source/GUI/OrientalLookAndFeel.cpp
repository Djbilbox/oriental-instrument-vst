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
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float centreX = bounds.getCentreX();
    float centreY = bounds.getCentreY();
    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Premium look: brushed-metal dome with a glowing RED indicator (matches the
    // reference). Gold stays for the thin value arc; red drives the pointer/cap.
    const juce::Colour red(Colors::RED);
    const juce::Colour gold(Colors::GOLD);

    const float d = radius * 2.0f;
    const juce::Rectangle<float> body(centreX - radius, centreY - radius, d, d);

    // ── Outer drop shadow ──
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.fillEllipse(body.translated(0.0f, 3.5f).expanded(3.0f));

    // ── Outer machined ring (dark bezel with a bright top rim) ──
    g.setColour(juce::Colour(0xFF161310));
    g.fillEllipse(body.expanded(4.5f));
    juce::ColourGradient rim(juce::Colour(0xFF6A5E4E), centreX, body.getY() - 3.0f,
                             juce::Colour(0xFF050403), centreX, body.getBottom() + 3.0f, false);
    g.setGradientFill(rim);
    g.drawEllipse(body.expanded(3.0f), 2.0f);

    // ── Metal dome body — strong top-left light → near-black bottom-right ──
    juce::ColourGradient dome(juce::Colour(0xFF5A4F42),
                              centreX - radius * 0.35f, centreY - radius * 0.50f,
                              juce::Colour(0xFF0B0907),
                              centreX + radius * 0.55f, centreY + radius * 0.70f, true);
    dome.addColour(0.55, juce::Colour(0xFF211C16));
    g.setGradientFill(dome);
    g.fillEllipse(body);

    // Fine concentric brushed sheen, clipped to the disc
    {
        juce::Graphics::ScopedSaveState save(g);
        juce::Path disc; disc.addEllipse(body);
        g.reduceClipRegion(disc);
        for (int i = 0; i < 24; ++i)
        {
            float a = juce::MathConstants<float>::twoPi * (static_cast<float>(i) / 24.0f);
            g.setColour(juce::Colours::white.withAlpha(i % 2 == 0 ? 0.04f : 0.02f));
            g.drawLine(centreX, centreY,
                       centreX + std::cos(a) * radius, centreY + std::sin(a) * radius, 0.7f);
        }
    }

    // Inner bevel
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(body.reduced(1.0f), 1.4f);

    // Top specular glint
    juce::ColourGradient spec(juce::Colours::white.withAlpha(0.28f),
                              centreX - radius * 0.35f, centreY - radius * 0.55f,
                              juce::Colours::transparentWhite, centreX, centreY, true);
    g.setGradientFill(spec);
    g.fillEllipse(body.reduced(radius * 0.16f).translated(-radius * 0.05f, -radius * 0.12f));

    // ── Thin gold value arc (subtle) ──
    float arcRadius = radius + 3.5f;
    juce::Path track;
    track.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xFF211B12));
    g.strokePath(track, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path arcPath;
    arcPath.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(gold.withAlpha(0.85f));
    g.strokePath(arcPath, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // ── RED indicator: glowing pointer from centre to rim ──
    float pAngle = angle - juce::MathConstants<float>::halfPi;
    float pInner = radius * 0.30f;
    float pOuter = radius * 0.86f;
    float px1 = centreX + pInner * std::cos(pAngle);
    float py1 = centreY + pInner * std::sin(pAngle);
    float px2 = centreX + pOuter * std::cos(pAngle);
    float py2 = centreY + pOuter * std::sin(pAngle);

    g.setColour(red.withAlpha(0.30f));            // glow
    g.drawLine(px1, py1, px2, py2, 5.5f);
    g.setColour(red.brighter(0.25f));             // core
    g.drawLine(px1, py1, px2, py2, 2.4f);

    // Glowing red tip dot
    g.setColour(red.withAlpha(0.35f));
    g.fillEllipse(px2 - 4.0f, py2 - 4.0f, 8.0f, 8.0f);
    g.setColour(red.brighter(0.4f));
    g.fillEllipse(px2 - 2.0f, py2 - 2.0f, 4.0f, 4.0f);

    // ── Centre cap (dark with red core) ──
    g.setColour(juce::Colour(0xFF0A0908));
    g.fillEllipse(centreX - radius * 0.20f, centreY - radius * 0.20f, radius * 0.40f, radius * 0.40f);
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawEllipse(centreX - radius * 0.20f, centreY - radius * 0.20f, radius * 0.40f, radius * 0.40f, 1.0f);
    g.setColour(red.withAlpha(0.85f));
    g.fillEllipse(centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
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

    // Per-button accent (header transport chips colour-code by name)
    auto name = button.getName();
    juce::Colour accent = juce::Colour(Colors::TAB_ON_BORDER); // gold default
    if (name == "REC")        accent = juce::Colour(Colors::RED);
    else if (name == "PANIC") accent = juce::Colour(0xFFFF6644);

    // Tab style: glassmorphism with accent border when active/pressed
    if (isOn || shouldDrawButtonAsDown)
    {
        g.setColour(accent.withAlpha(0.15f));
        g.fillRoundedRectangle(bounds, Design::BORDER_RADIUS_LG);
        g.setColour(accent);
        g.drawRoundedRectangle(bounds, Design::BORDER_RADIUS_LG, 1.0f);
    }
    else
    {
        g.setColour(juce::Colour(Colors::TAB_BG));
        g.fillRoundedRectangle(bounds, Design::BORDER_RADIUS_LG);
        g.setColour(shouldDrawButtonAsHighlighted ? accent.withAlpha(0.6f) : juce::Colour(Colors::TAB_BORDER));
        g.drawRoundedRectangle(bounds, Design::BORDER_RADIUS_LG, 1.0f);
    }
}

void OrientalLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                          bool, bool)
{
    bool isOn = button.getToggleState();
    auto name = button.getName();
    juce::Colour onCol = juce::Colour(Colors::TAB_NAME_ON); // gold
    if (name == "REC")        onCol = juce::Colour(Colors::RED);
    else if (name == "PANIC") onCol = juce::Colour(0xFFFF6644);

    g.setColour((isOn || name == "PANIC") ? onCol : juce::Colour(Colors::TAB_NAME_OFF));
    g.setFont(Typography::tab());
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 1);
}
