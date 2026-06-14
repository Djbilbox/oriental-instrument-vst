#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../DSP/OrientalSynthesiser.h"
#include "../Data/PresetManager.h"

// Standalone audio context for mobile apps. Wraps PluginProcessor in a
// DeviceManager + AudioSourcePlayer for mobile playback.
class MobileAudioContext : public juce::AudioIODeviceCallback
{
public:
    MobileAudioContext();
    ~MobileAudioContext() override;

    void initialise(double sampleRate = 44100.0, int blockSize = 512);
    void shutdown();

    OrientalInstrumentProcessor& getProcessor() { return processor; }
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

    // AudioIODeviceCallback
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float* const* outputChannelData, int numOutputChannels,
                               int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    OrientalInstrumentProcessor processor;
    juce::AudioDeviceManager deviceManager;
    juce::MidiMessageCollector midiCollector;
    juce::AudioBuffer<float> tempBuffer;
    juce::MidiBuffer tempMidiBuffer;
    bool isInitialised = false;
};
