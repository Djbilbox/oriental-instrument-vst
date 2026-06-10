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
}

void FXPanel::FXSlot::paint(juce::Graphics& g)
{
    bool on = toggleBtn.getToggleState();

    g.setColour(on ? juce::Colour(0x10DC1E1E) : juce::Colour(0x4D000000));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

    g.setColour(on ? juce::Colour(0x4DDC1E1E) : juce::Colour(0xFF222222));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 4.0f, 1.0f);

    // LED indicator
    auto ledBounds = juce::Rectangle<float>(8.0f, (getHeight() - 8.0f) / 2.0f, 8.0f, 8.0f);
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::RED) : juce::Colour(0xFF333333));
    g.fillEllipse(ledBounds);
    if (on)
    {
        g.setColour(juce::Colour(OrientalConstants::Colors::RED).withAlpha(0.4f));
        g.fillEllipse(ledBounds.expanded(2.0f));
    }

    // FX name
    g.setColour(on ? juce::Colour(0xFFCCCCCC) : juce::Colour(0xFF888888));
    g.setFont(11.0f);
    g.drawText(fxName, 22, 0, getWidth() - 60, getHeight(), juce::Justification::centredLeft);

    // Amount value
    float amt = state.getRawParameterValue(amtParamId)->load();
    g.setColour(on ? juce::Colour(OrientalConstants::Colors::GOLD) : juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.setFont(10.0f);
    g.drawText(juce::String(static_cast<int>(amt)) + "%", getWidth() - 40, 0, 36, getHeight(), juce::Justification::centredRight);
}

void FXPanel::FXSlot::mouseDown(const juce::MouseEvent&)
{
    toggleBtn.setToggleState(!toggleBtn.getToggleState(), juce::sendNotification);
    repaint();
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
    g.setFont(juce::Font("Cinzel", 9.0f, juce::Font::bold));
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
