#include "KnobComponent.h"
#include "../Utils/Constants.h"
#include "Typography.h"

using namespace OrientalConstants;

KnobComponent::KnobComponent(const juce::String& label)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setRange(0.0, 100.0, 1.0);
    slider.setName(label); // used by LookAndFeel to detect FX MIX → red accent
    addAndMakeVisible(slider);

    // Label: font-family: Cinzel, font-size: 10px, font-weight: 600, color: #ccc
    nameLabel.setText(Typography::tracked(label), juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFCCCCCC));
    nameLabel.setFont(Typography::knobLabel());
    addAndMakeVisible(nameLabel);

    // Value: tabular gold readout
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, juce::Colour(Colors::GOLD));
    valueLabel.setFont(Typography::value());
    addAndMakeVisible(valueLabel);

    slider.onValueChange = [this]()
    {
        valueLabel.setText(juce::String(static_cast<int>(slider.getValue())), juce::dontSendNotification);
    };
}

void KnobComponent::resized()
{
    auto bounds = getLocalBounds().reduced(2);

    // Layout: knob centered, label below, value below label
    auto labelArea = bounds.removeFromBottom(13);
    auto valueArea = bounds.removeFromBottom(14);

    nameLabel.setBounds(labelArea);
    valueLabel.setBounds(valueArea);

    // Knob area: constrain to --knob-size: 54px
    auto knobArea = bounds;
    int knobSize = static_cast<int>(Design::KNOB_SIZE);
    int xOffset = (knobArea.getWidth() - knobSize) / 2;
    int yOffset = (knobArea.getHeight() - knobSize) / 2;
    slider.setBounds(knobArea.getX() + xOffset, knobArea.getY() + yOffset, knobSize, knobSize);
}
