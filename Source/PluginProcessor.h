#pragma once
#include <JuceHeader.h>
#include "DSP/OrientalSynthesiser.h"
#include "DSP/FXChain/FXChain.h"
#include "DSP/MasterLimiter.h"
#include "Data/PresetManager.h"
#include "Utils/Constants.h"

class OrientalInstrumentProcessor : public juce::AudioProcessor,
                                     public juce::AudioProcessorValueTreeState::Listener
{
public:
    OrientalInstrumentProcessor();
    ~OrientalInstrumentProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // APVTS listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Public access for Editor
    OrientalSynthesiser& getSynthesiser() { return synthesiser; }
    FXChain& getFXChain() { return fxChain; }
    PresetManager& getPresetManager() { return presetManager; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Pitch/mod wheel from GUI
    void setPitchBend(float normalizedValue);
    void setModWheel(float normalizedValue);

    // Transport / voice-mode actions from the header buttons
    void panicAllNotes();
    void setMonoMode(bool mono);
    void setLegato(bool legato);

private:
    OrientalSynthesiser synthesiser;
    FXChain fxChain;
    MasterLimiter masterLimiter;   // final output protection (soft limiter + hard ceiling)
    PresetManager presetManager;

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Cached parameter pointers for real-time access
    std::atomic<float>* glideParam  = nullptr;
    std::atomic<float>* spaceParam  = nullptr;
    std::atomic<float>* filterParam = nullptr;
    std::atomic<float>* orientParam = nullptr;
    std::atomic<float>* resoParam   = nullptr;
    std::atomic<float>* depthParam  = nullptr;
    std::atomic<float>* fxMixParam  = nullptr;
    std::atomic<float>* instrumentParam = nullptr;
    std::atomic<float>* presetParam = nullptr;

    // ADSR parameter pointers
    std::atomic<float>* attackParam  = nullptr;
    std::atomic<float>* decayParam   = nullptr;
    std::atomic<float>* sustainParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;

    // FX parameter pointers
    std::atomic<float>* reverbAmtParam = nullptr;
    std::atomic<float>* delayAmtParam  = nullptr;
    std::atomic<float>* chorusAmtParam = nullptr;
    std::atomic<float>* distAmtParam   = nullptr;
    std::atomic<float>* compAmtParam   = nullptr;
    std::atomic<float>* phaserAmtParam = nullptr;
    std::atomic<float>* bitcrushAmtParam = nullptr;
    std::atomic<float>* reverbOnParam  = nullptr;
    std::atomic<float>* delayOnParam   = nullptr;
    std::atomic<float>* chorusOnParam  = nullptr;
    std::atomic<float>* distOnParam    = nullptr;
    std::atomic<float>* compOnParam    = nullptr;
    std::atomic<float>* eqOnParam       = nullptr;
    std::atomic<float>* eqAmtParam      = nullptr;
    std::atomic<float>* phaserOnParam  = nullptr;
    std::atomic<float>* bitcrushOnParam = nullptr;

    int lastInstrument = -1;
    int lastPreset = -1;

    void applyPresetToParameters(const PresetData& preset);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrientalInstrumentProcessor)
};
