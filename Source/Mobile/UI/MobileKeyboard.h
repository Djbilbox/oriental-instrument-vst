#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"

class MobileKeyboard : public juce::Component
{
public:
    MobileKeyboard(OrientalInstrumentProcessor& proc);
    ~MobileKeyboard() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Touch handling
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    // Pitch bend from vertical touch drag
    void setPitchBend(float normalizedValue);

private:
    OrientalInstrumentProcessor& processor;

    struct TouchNote
    {
        int midiNote = -1;
        juce::Rectangle<float> bounds;
        bool isPressed = false;
    };

    std::vector<TouchNote> keys;
    int baseNote = 60;  // Middle C
    int numOctaves = 2;
    int keysPerOctave = 24;  // Maqam: quarter-tone tuning (24 tones)

    juce::Point<float> lastTouchPos;
    float pitchBendAmount = 0.0f;

    int getNoteAtPoint(juce::Point<int> pos);
    void updateKeyLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MobileKeyboard)
};
