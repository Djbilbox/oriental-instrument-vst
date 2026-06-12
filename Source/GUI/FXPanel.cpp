#include "FXPanel.h"
#include "../Utils/Constants.h"

FXPanel::FXSlot::FXSlot(const juce::String& name, const juce::String& paramOnId,
                         const juce::String& paramAmtId, juce::AudioProcessorValueTreeState& apvts)
    : fxName(name), onParamId(paramOnId), amtParamId(paramAmtId), state(apvts)
{
    toggleBtn.setClickingTogglesState(true);
    addAndMakeVisible(toggleBtn);
    toggleBtn.setVisible(false); // hidden, used only for attachment

    buttonAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, paramOnId, toggleBtn);

    // − / + amount nudge buttons (the missing "increase/reduce" controls).
    auto styleNudge = [this](juce::TextButton& b)
    {
        b.setColour(juce::TextButton::buttonColourId, juce::Colour(0x55000000));
        b.setColour(juce::TextButton::textColourOffId, juce::Colour(OrientalConstants::Colors::GOLD_LIGHT));
        b.setColour(juce::ComboBox::outlineColourId, juce::Colour(OrientalConstants::Colors::GOLD_DIM));
        addAndMakeVisible(b);
    };
    styleNudge(minusBtn);
    styleNudge(plusBtn);
    minusBtn.onClick = [this] { nudge(-5.0f); };
    plusBtn.onClick  = [this] { nudge(+5.0f); };

    startTimerHz(8); // keep the % in sync with preset loads / host automation
}

FXPanel::FXSlot::~FXSlot() { stopTimer(); }

void FXPanel::FXSlot::nudge(float deltaPercent)
{
    if (auto* p = state.getParameter(amtParamId))
    {
        const float cur = state.getRawParameterValue(amtParamId)->load();
        const float nv  = juce::jlimit(0.0f, 100.0f, cur + deltaPercent);
        p->setValueNotifyingHost(p->convertTo0to1(nv));
        repaint();
    }
}

void FXPanel::FXSlot::timerCallback()
{
    const float amt = state.getRawParameterValue(amtParamId)->load();
    if (std::abs(amt - lastShownAmt) > 0.5f)
    {
        lastShownAmt = amt;
        repaint();
    }
}

void FXPanel::FXSlot::resized()
{
    auto r = getLocalBounds().reduced(2);
    const int btn = juce::jmin(20, r.getHeight());
    plusBtn.setBounds(r.removeFromRight(btn).withSizeKeepingCentre(btn, btn));
    r.removeFromRight(34);                 // reserve room for the % readout
    minusBtn.setBounds(r.removeFromRight(btn).withSizeKeepingCentre(btn, btn));
}

void FXPanel::FXSlot::paint(juce::Graphics& g)
{
    const bool on = toggleBtn.getToggleState();
    const float amt = state.getRawParameterValue(amtParamId)->load();

    // Brushed-metal row with a subtle top sheen.
    auto rb = getLocalBounds().toFloat();
    juce::ColourGradient rowg(juce::Colour(on ? 0xFF241A18u : 0xFF15110Eu), rb.getX(), rb.getY(),
                              juce::Colour(0xFF080605), rb.getX(), rb.getBottom(), false);
    g.setGradientFill(rowg);
    g.fillRoundedRectangle(rb, 4.0f);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillRoundedRectangle(rb.reduced(1.0f).withHeight(rb.getHeight() * 0.45f), 3.0f);
    g.setColour(on ? juce::Colour(0x66DC1E1E) : juce::Colour(0xFF26211B));
    g.drawRoundedRectangle(rb.reduced(0.5f), 4.0f, 1.0f);

    // Segmented VU-style amount meter along the bottom edge.
    {
        auto bar = getLocalBounds().toFloat().reduced(4.0f, 2.0f).removeFromBottom(4.0f);
        const int segs = 14;
        const float lit = juce::jlimit(0.0f, 1.0f, amt / 100.0f) * segs;
        const float sw = bar.getWidth() / segs;
        for (int i = 0; i < segs; ++i)
        {
            auto seg = juce::Rectangle<float>(bar.getX() + i * sw, bar.getY(), sw - 1.0f, bar.getHeight());
            const float t = static_cast<float>(i) / (segs - 1);
            juce::Colour c = t < 0.6f ? juce::Colour(OrientalConstants::Colors::GOLD)
                           : t < 0.85f ? juce::Colour(0xFFE08A2A)
                                       : juce::Colour(OrientalConstants::Colors::RED);
            const bool litSeg = on && (static_cast<float>(i) < lit);
            g.setColour(litSeg ? c : juce::Colour(0xFF1C1814));
            g.fillRect(seg);
        }
    }

    // LED indicator
    auto ledBounds = juce::Rectangle<float>(7.0f, (getHeight() - 9.0f) / 2.0f - 2.0f, 9.0f, 9.0f);
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::RED) : juce::Colour(0xFF333333));
    g.fillEllipse(ledBounds);
    if (on)
    {
        g.setColour(juce::Colour(OrientalConstants::Colors::RED).withAlpha(0.4f));
        g.fillEllipse(ledBounds.expanded(2.0f));
    }

    // FX name (bigger, readable)
    g.setColour(on ? juce::Colour(0xFFE8E8E8) : juce::Colour(0xFF999999));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText(fxName, 22, 0, getWidth() - 96, getHeight() - 3, juce::Justification::centredLeft);

    // Amount value (between − and +)
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::GOLD_LIGHT)
                   : juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(juce::String(static_cast<int>(amt)) + "%",
               getWidth() - 22 - 34, 0, 34, getHeight() - 3, juce::Justification::centred);
}

void FXPanel::FXSlot::mouseDown(const juce::MouseEvent& e)
{
    // Toggle on/off only when the click lands on the LED / name area — the
    // right edge belongs to the − / + nudge buttons.
    if (e.x < getWidth() - 80)
    {
        toggleBtn.setToggleState(!toggleBtn.getToggleState(), juce::sendNotification);
        repaint();
    }
}

FXPanel::FXPanel(FXChain& chain, juce::AudioProcessorValueTreeState& state)
    : fxChain(chain), apvts(state)
{
    struct FXDef { const char* name; const char* onId; const char* amtId; };
    FXDef defs[] = {
        { "Reverb",    "reverbOn",   "reverbAmt" },
        { "Echo",      "delayOn",    "delayAmt" },
        { "Chorus",    "chorusOn",   "chorusAmt" },
        { "Distort",   "distOn",     "distAmt" },
        { "Comprs",    "compOn",     "compAmt" },
        { "3-Band EQ", "eqOn",       "eqAmt" },
        { "Phaser",    "phaserOn",   "phaserAmt" },
        { "Bitcrsh",   "bitcrushOn", "bitcrushAmt" },
    };

    for (auto& def : defs)
    {
        auto* slot = new FXSlot(def.name, def.onId, def.amtId, apvts);
        slots.add(slot);
        addAndMakeVisible(slot);
    }
}

void FXPanel::paint(juce::Graphics& g)
{
    // Titre rouge avec glow — design HTML de référence
    auto titleArea = getLocalBounds().removeFromTop(20);
    juce::Colour red(OrientalConstants::Colors::RED);
    // Red glow (shadow layers)
    g.setColour(red.withAlpha(0.25f));
    g.setFont(juce::Font("Cinzel", 11.0f, juce::Font::bold));
    for (int dx = -2; dx <= 2; ++dx)
        for (int dy = -2; dy <= 2; ++dy)
            if (dx != 0 || dy != 0)
                g.drawText("FX CHAIN", titleArea.translated(dx, dy), juce::Justification::centred);
    g.setColour(red);
    g.drawText("FX CHAIN", titleArea, juce::Justification::centred);
}

void FXPanel::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(24);

    int slotHeight = bounds.getHeight() / 8;
    for (auto* slot : slots)
        slot->setBounds(bounds.removeFromTop(slotHeight).reduced(2, 1));
}
