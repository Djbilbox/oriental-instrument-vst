#pragma once
#include <JuceHeader.h>
#include "../../Data/PresetManager.h"
#include "../../PluginProcessor.h"

class MobilePresetBrowser : public juce::Component,
                            public juce::ScrollBar::Listener
{
public:
    MobilePresetBrowser(OrientalInstrumentProcessor& proc, PresetManager& mgr);
    ~MobilePresetBrowser() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void scrollBarMoved(juce::ScrollBar* scrollBar, double newRangeStart) override;

    // Instrument selector
    void setInstrument(int instIndex);
    int getCurrentInstrument() const { return currentInstrument; }

private:
    OrientalInstrumentProcessor& processor;
    PresetManager& presetManager;

    int currentInstrument = 0;
    int scrollOffset = 0;
    int selectedPresetIndex = -1;

    struct PresetItem
    {
        int index;
        juce::String name;
        juce::Rectangle<int> bounds;
        bool available = true;
    };

    std::vector<PresetItem> visiblePresets;

    std::unique_ptr<juce::ScrollBar> scrollBar;

    void updatePresetList();
    int getPresetAtPoint(juce::Point<int> pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MobilePresetBrowser)
};
