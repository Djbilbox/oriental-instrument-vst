#include "MainViewController.h"

iOSMainComponent::iOSMainComponent()
{
    audioContext = std::make_unique<MobileAudioContext>();
    audioContext->initialise(44100.0, 512);

    uiController = std::make_unique<MobileUIController>(audioContext->getProcessor());
    addAndMakeVisible(*uiController);

    #if ORIENTAL_DEMO
    // Freemium: limited to 3 instruments, 3 presets in demo
    #endif
}

iOSMainComponent::~iOSMainComponent()
{
    if (audioContext)
        audioContext->shutdown();
}

void iOSMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void iOSMainComponent::resized()
{
    if (uiController)
        uiController->setBounds(getLocalBounds());
}
