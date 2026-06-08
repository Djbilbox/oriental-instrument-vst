#pragma once
#include <JuceHeader.h>
#include "../Data/PresetManager.h"

class PresetBrowser : public juce::Component,
                      public juce::ListBoxModel
{
public:
    PresetBrowser(PresetManager& pm, juce::AudioProcessorValueTreeState& apvts);
    ~PresetBrowser() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    void setInstrumentFilter(OrientalConstants::Instrument inst);

private:
    PresetManager& presetManager;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ListBox listBox;
    juce::TextEditor searchBox;
    std::array<juce::TextButton, 7> instrumentTabs;

    OrientalConstants::Instrument currentInstrument = OrientalConstants::Instrument::Violin;
    juce::String searchFilter;

    std::vector<int> filteredIndices;
    void updateFilteredList();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowser)
};
