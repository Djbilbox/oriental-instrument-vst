#pragma once
#include <JuceHeader.h>
#include "../MobileAudioContext.h"
#include "../IAP/IAPManager.h"
#include "../UI/MobileUIController.h"

class iOSMainComponent : public juce::Component
{
public:
    iOSMainComponent();
    ~iOSMainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<MobileAudioContext> audioContext;
    std::unique_ptr<MobileUIController> uiController;
    std::unique_ptr<IAPManager> iapManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(iOSMainComponent)
};
