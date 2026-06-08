#pragma once
#include <JuceHeader.h>

class WaveformDisplay : public juce::Component, public juce::Timer
{
public:
    WaveformDisplay();
    ~WaveformDisplay() override = default;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    float phase = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
