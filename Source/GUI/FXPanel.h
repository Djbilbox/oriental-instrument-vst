#pragma once
#include <JuceHeader.h>
#include "../DSP/FXChain/FXChain.h"

class FXPanel : public juce::Component
{
public:
    FXPanel(FXChain& chain, juce::AudioProcessorValueTreeState& apvts);
    ~FXPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    FXChain& fxChain;
    juce::AudioProcessorValueTreeState& apvts;

    struct FXSlot : public juce::Component
    {
        FXSlot(const juce::String& name, const juce::String& paramOnId,
               const juce::String& paramAmtId, juce::AudioProcessorValueTreeState& apvts);
        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent&) override;

        juce::String fxName;
        juce::String onParamId;
        juce::String amtParamId;
        juce::AudioProcessorValueTreeState& state;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttach;
        juce::ToggleButton toggleBtn;
    };

    juce::OwnedArray<FXSlot> slots;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXPanel)
};
