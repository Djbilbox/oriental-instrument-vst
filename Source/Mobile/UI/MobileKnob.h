#pragma once
#include <JuceHeader.h>

class MobileKnob : public juce::Component
{
public:
    MobileKnob(const juce::String& labelText = "", float initialValue = 0.5f);
    ~MobileKnob() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    void setValue(float v);
    float getValue() const { return value; }

    void setLabel(const juce::String& text) { label = text; }
    void setValueFormatter(std::function<juce::String(float)> formatter) { formatValue = formatter; }

    using ValueChangeCallback = std::function<void(float)>;
    void setValueChangedCallback(ValueChangeCallback cb) { onValueChanged = cb; }

private:
    float value = 0.5f;
    juce::String label;
    std::function<juce::String(float)> formatValue;
    ValueChangeCallback onValueChanged;

    juce::Point<float> lastMousePos;
    float dragSensitivity = 0.01f;
    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MobileKnob)
};
