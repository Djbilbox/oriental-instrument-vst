#include "FreemiumIndicator.h"

FreemiumIndicator::FreemiumIndicator(IAPManager* iapMgr)
    : iapManager(iapMgr)
{
    if (!iapManager || iapManager->isProUnlocked())
        showWatermark = false;
    else
        startTimer(100); // Pulse animation
}

void FreemiumIndicator::paint(juce::Graphics& g)
{
    if (!showWatermark)
        return;

    auto bounds = getLocalBounds().toFloat();

    // Watermark: "DEMO" + pulse
    float alpha = 0.3f + 0.15f * std::sin(pulsePhase);
    g.setColour(juce::Colours::red.withAlpha(alpha));

    juce::Font font("Arial", 28, juce::Font::bold);
    g.setFont(font);
    g.drawText("DEMO", bounds, juce::Justification::topRight, false);

    // Tap hint
    g.setFont(14);
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawText("tap to unlock", bounds.reduced(10).withBottom(bounds.getBottom() - 5),
               juce::Justification::bottomRight, false);
}

void FreemiumIndicator::mouseUp(const juce::MouseEvent& e)
{
    if (showWatermark && iapManager)
        launchPurchaseFlow();
}

void FreemiumIndicator::timerCallback()
{
    pulsePhase += 0.1f;
    if (pulsePhase > juce::MathConstants<float>::twoPi)
        pulsePhase -= juce::MathConstants<float>::twoPi;

    repaint();
}

void FreemiumIndicator::launchPurchaseFlow()
{
    if (!iapManager) return;

    iapManager->requestPurchase("oriental_pro", [this](IAPManager::PurchaseState state, const juce::String& productId)
    {
        if (state == IAPManager::PurchaseState::success)
        {
            showWatermark = false;
            stopTimer();
            repaint();
        }
    });
}

void FreemiumIndicator::showPurchasePrompt()
{
    launchPurchaseFlow();
}
