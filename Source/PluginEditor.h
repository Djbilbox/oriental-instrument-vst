#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/OrientalLookAndFeel.h"
#include "GUI/KnobComponent.h"
#include "GUI/PresetBrowser.h"
#include "GUI/PianoComponent.h"
#include "GUI/FXPanel.h"
#include "GUI/WaveformDisplay.h"
#include "GUI/WheelComponent.h"
#include "GUI/BackgroundComponent.h"

class OrientalInstrumentEditor : public juce::AudioProcessorEditor
{
public:
    explicit OrientalInstrumentEditor(OrientalInstrumentProcessor&);
    ~OrientalInstrumentEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OrientalInstrumentProcessor& processorRef;
    OrientalLookAndFeel orientalLnf;

    BackgroundComponent background;
    PresetBrowser presetBrowser;
    FXPanel fxPanel;
    PianoComponent piano;
    WaveformDisplay waveform;

    // Wheels
    WheelComponent pitchWheel;
    WheelComponent modWheel;

    // Macro knobs
    KnobComponent knobGlide, knobSpace, knobFilter, knobOrient, knobReso, knobDepth, knobFxMix;

    // APVTS attachments for macro knobs
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> glideAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> orientAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resoAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxMixAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalInstrumentEditor)
};
