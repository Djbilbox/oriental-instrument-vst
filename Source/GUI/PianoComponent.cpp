#include "PianoComponent.h"
#include "../Utils/Constants.h"

PianoComponent::PianoComponent(OrientalSynthesiser& synth)
    : synthesiser(synth),
      keyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    keyboardState.addListener(this);

    keyboard.setAvailableRange(48, 84); // C3 to C6
    keyboard.setOctaveForMiddleC(4);

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
}

void PianoComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(24);
    keyboard.setBounds(bounds.reduced(54, 0));
}

void PianoComponent::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    synthesiser.noteOn(midiChannel, midiNoteNumber, velocity);
}

void PianoComponent::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    synthesiser.noteOff(midiChannel, midiNoteNumber, velocity);
}
