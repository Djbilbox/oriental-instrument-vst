#include "MobileAudioContext.h"

MobileAudioContext::MobileAudioContext()
{
    // Configure device manager for mobile audio
    #if JUCE_IOS
    deviceManager.initialiseWithDefaultDevices(0, 2);
    #elif JUCE_ANDROID
    deviceManager.initialiseWithDefaultDevices(0, 2);
    #endif
}

MobileAudioContext::~MobileAudioContext()
{
    shutdown();
}

void MobileAudioContext::initialise(double sampleRate, int blockSize)
{
    if (isInitialised) return;

    auto* device = deviceManager.getCurrentAudioDevice();
    if (!device)
    {
        jassertfalse; // No audio device configured
        return;
    }

    processor.prepareToPlay(sampleRate, blockSize);
    tempBuffer.setSize(2, blockSize);
    isInitialised = true;

    // Start audio
    juce::String error = deviceManager.initialiseWithDefaultDevices(0, 2);
    if (error.isNotEmpty())
        jassertfalse; // Audio init failed
    else
        deviceManager.addAudioCallback(this);
}

void MobileAudioContext::shutdown()
{
    if (!isInitialised) return;

    deviceManager.removeAudioCallback(this);
    processor.releaseResources();
    tempBuffer.setSize(0, 0);
    isInitialised = false;
}

void MobileAudioContext::audioDeviceIOCallback(const float** inputChannelData,
                                              int numInputChannels,
                                              float* const* outputChannelData,
                                              int numOutputChannels,
                                              int numSamples)
{
    tempMidiBuffer.clear();
    midiCollector.removeNextBlockOfMessages(tempMidiBuffer, numSamples);

    tempBuffer.setSize(jmax(numOutputChannels, 2), numSamples, false, false, true);
    tempBuffer.clear();

    juce::AudioBuffer<float> bufferToUse(tempBuffer.getArrayOfWritePointers(),
                                         jmin(numOutputChannels, tempBuffer.getNumChannels()),
                                         numSamples);

    processor.processBlock(bufferToUse, tempMidiBuffer);

    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        if (ch < tempBuffer.getNumChannels())
            juce::FloatVectorOperations::copy(outputChannelData[ch],
                                             tempBuffer.getReadPointer(ch),
                                             numSamples);
        else
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);
    }
}

void MobileAudioContext::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    midiCollector.reset(device->getCurrentSampleRate());
}

void MobileAudioContext::audioDeviceStopped()
{
    tempBuffer.setSize(0, 0);
}
