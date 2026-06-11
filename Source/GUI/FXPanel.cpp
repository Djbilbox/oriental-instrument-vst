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

    g.setColour(on ? juce::Colour(0x10DC1E1E) : juce::Colour(0x4D000000));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

    g.setColour(on ? juce::Colour(0x4DDC1E1E) : juce::Colour(0xFF222222));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 4.0f, 1.0f);

    // Amount fill bar along the bottom edge (visual level meter).
    auto bar = getLocalBounds().toFloat().reduced(3.0f);
    bar = bar.removeFromBottom(3.0f);
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillRoundedRectangle(bar, 1.5f);
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::GOLD)
                   : juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.fillRoundedRectangle(bar.withWidth(bar.getWidth() * juce::jlimit(0.0f, 1.0f, amt / 100.0f)), 1.5f);

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
    g.setFont(juce::Font(12.5f, juce::Font::bold));
    g.drawText(fxName, 22, 0, getWidth() - 96, getHeight() - 3, juce::Justification::centredLeft);

    // Amount value (between − and +)
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::GOLD_LIGHT)
                   : juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
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
