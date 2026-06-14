#include "MainActivity.h"

AndroidMainComponent::AndroidMainComponent()
{
    audioContext = std::make_unique<MobileAudioContext>();
    audioContext->initialise(44100.0, 512);

    uiController = std::make_unique<MobileUIController>(audioContext->getProcessor());
    addAndMakeVisible(*uiController);

    #if ORIENTAL_DEMO
    // Freemium: limited to 3 instruments, 3 presets in demo
    #endif
}

AndroidMainComponent::~AndroidMainComponent()
{
    if (audioContext)
        audioContext->shutdown();
}

void AndroidMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void AndroidMainComponent::resized()
{
    if (uiController)
        uiController->setBounds(getLocalBounds());
}
