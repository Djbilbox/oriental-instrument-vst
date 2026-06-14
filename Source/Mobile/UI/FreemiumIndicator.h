#pragma once
#include <JuceHeader.h>
#include "../../Mobile/IAP/IAPManager.h"

class FreemiumIndicator : public juce::Component,
                          public juce::Timer
{
public:
    FreemiumIndicator(IAPManager* iapManager);
    ~FreemiumIndicator() override = default;

    void paint(juce::Graphics& g) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void timerCallback() override;

    void showPurchasePrompt();

private:
    IAPManager* iapManager = nullptr;
    bool showWatermark = true;
    bool isPulsing = false;
    float pulsePhase = 0.0f;

    void launchPurchaseFlow();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreemiumIndicator)
};
