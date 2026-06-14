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

    // Mobile freemium support
    void setProUnlocked(bool unlocked);
    bool isProUnlocked() const { return proUnlocked; }
    bool isPresetAvailable(int index) const;
    int getMaxPresetsPerInstrument() const;

private:
    void loadFactoryPresets();
    void loadExtendedPresets();

    bool proUnlocked = true;  // Full version by default; mobile demo sets to false
    static constexpr int DEMO_PRESETS_PER_INSTRUMENT = 3;
    static constexpr int DEMO_NUM_INSTRUMENTS = 3;  // Violin, Oud, Ney only

    std::vector<PresetData> factoryPresets;
    int currentPresetIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
