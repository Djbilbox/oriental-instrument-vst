#pragma once
#include <JuceHeader.h>
#include "../Utils/Constants.h"

class OrientalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OrientalLookAndFeel();
    ~OrientalLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor&) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
