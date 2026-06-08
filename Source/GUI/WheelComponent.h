#pragma once
#include <JuceHeader.h>

class WheelComponent : public juce::Component
{
public:
    enum class Type { PitchBend, ModWheel };

    WheelComponent(Type type, const juce::String& label);
    ~WheelComponent() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    float getValue() const { return currentValue; }
    std::function<void(float)> onValueChange;

private:
    Type wheelType;
    juce::String labelText;
    float currentValue = 0.0f;
    float dragStartValue = 0.0f;
    int dragStartY = 0;

    void springReturn();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WheelComponent)
};
