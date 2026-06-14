#pragma once
#include "IAPManager.h"

#if JUCE_IOS
#include <StoreKit/StoreKit.h>

class iOSIAPManager : public IAPManager
{
public:
    iOSIAPManager();
    ~iOSIAPManager() override;

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
};

#endif // JUCE_IOS
