#pragma once
#include "IAPManager.h"

#if JUCE_ANDROID
#include <jni.h>

class AndroidIAPManager : public IAPManager
{
public:
    AndroidIAPManager();
    ~AndroidIAPManager() override;

    void initialise() override;
    bool isPurchased(const juce::String& productId) const override;
    juce::String getProductPrice(const juce::String& productId) const override;
    void requestPurchase(const juce::String& productId, PurchaseCallback callback) override;
    void restorePurchases(PurchaseCallback callback) override;

private:
    struct ProductInfo
    {
        juce::String productId;
        juce::String displayName;
        juce::String displayPrice;
    };

    juce::HashMap<juce::String, ProductInfo> productCache;
    juce::HashMap<juce::String, bool> purchaseCache;
    PurchaseCallback currentCallback;

    void queryProducts();
    void handlePurchaseResult(bool success, const juce::String& productId);

    // JNI helpers for Google Play Billing
    jobject billingClient = nullptr;
};

#endif // JUCE_ANDROID
