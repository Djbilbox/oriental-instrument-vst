#include "PianoComponent.h"
#include "../Utils/Constants.h"
#include "Typography.h"

PianoComponent::PianoComponent(OrientalSynthesiser& synth)
    : synthesiser(synth),
      keyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    keyboardState.addListener(this);

    keyboard.setAvailableRange(36, 96); // C2 to C7 — full span across the bottom
    keyboard.setOctaveForMiddleC(4);
    keyboard.setScrollButtonsVisible(false);

    // Custom colors matching prototype
    keyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xFFF5F0E8));
    keyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xFF222222));
    keyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xFF999999));
    keyboard.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colour(OrientalConstants::Colors::RED));

    addAndMakeVisible(keyboard);
}

PianoComponent::~PianoComponent()
{
    keyboardState.removeListener(this);
}

void PianoComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xF5050402));
    g.fillRect(getLocalBounds());
    g.setColour(juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.drawRect(getLocalBounds(), 1);

    // Dedication — discreet, engraved gold caps in the strip above the keys.
    auto strip = getLocalBounds().removeFromTop(24);
    g.setColour(juce::Colour(OrientalConstants::Colors::GOLD_DIM));
    g.setFont(Typography::display(8.5f, true));
    g.drawText(Typography::tracked(juce::String(juce::CharPointer_UTF8("D\xC3\xA9""dicace \xC3\xA0 Mourad Attalah"))),
               strip, juce::Justification::centred);
}

void PianoComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(24);

    // Stretch the keys so the keyboard fills the full width — no empty gap on
    // the right. Key width = available width / number of white keys in range.
    const int lo = 36, hi = 96;
    int whiteKeys = 0;
    for (int n = lo; n <= hi; ++n)
    {
        const int pc = n % 12;
        if (pc != 1 && pc != 3 && pc != 6 && pc != 8 && pc != 10)
            ++whiteKeys;
    }
    if (whiteKeys > 0)
        keyboard.setKeyWidth(static_cast<float>(bounds.getWidth()) / static_cast<float>(whiteKeys));

    keyboard.setBounds(bounds);
}

void PianoComponent::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    synthesiser.noteOn(midiChannel, midiNoteNumber, velocity);
}

void PianoComponent::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    synthesiser.noteOff(midiChannel, midiNoteNumber, velocity);
}
