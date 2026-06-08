#pragma once
#include <JuceHeader.h>

class KnobComponent : public juce::Component
{
public:
    explicit KnobComponent(const juce::String& label);
    ~KnobComponent() override = default;

    void resized() override;
    juce::Slider& getSlider() { return slider; }

private:
    juce::Slider slider;
    juce::Label nameLabel;
    juce::Label valueLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
