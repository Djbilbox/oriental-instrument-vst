#pragma once
#include <JuceHeader.h>
#include "PresetData.h"

class PresetManager
{
public:
    PresetManager();
    ~PresetManager() = default;

    int getNumPresets() const;
    int getNumPresetsForInstrument(OrientalConstants::Instrument inst) const;
    const PresetData& getPreset(int index) const;
    const PresetData& getPreset(OrientalConstants::Instrument inst, int index) const;

    void selectPreset(int index);
    void selectPreset(OrientalConstants::Instrument inst, int index);
    int getCurrentPresetIndex() const;
    const PresetData& getCurrentPreset() const;

    void saveUserPreset(const PresetData& preset, const juce::File& file);
    PresetData loadUserPreset(const juce::File& file);

private:
    void loadFactoryPresets();
    void loadExtendedPresets();

    std::vector<PresetData> factoryPresets;
    int currentPresetIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
