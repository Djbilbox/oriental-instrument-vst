#pragma once
#include <JuceHeader.h>
#include "../DSP/OrientalSynthesiser.h"

class PianoComponent : public juce::Component,
                       public juce::MidiKeyboardStateListener
{
public:
    explicit PianoComponent(OrientalSynthesiser& synth);
    ~PianoComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // MidiKeyboardStateListener
    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    OrientalSynthesiser& synthesiser;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoComponent)
};
