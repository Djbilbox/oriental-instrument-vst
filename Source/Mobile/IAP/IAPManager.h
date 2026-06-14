#pragma once
#include <JuceHeader.h>
#include <functional>

// Abstract IAP interface. Platform-specific implementations (iOS/Android) override.
class IAPManager
{
public:
    virtual ~IAPManager() = default;

    enum class PurchaseState
    {
        notStarted,
        pending,
        success,
        failed,
        cancelled,
        restored
    };

    using PurchaseCallback = std::function<void(PurchaseState state, const juce::String& productId)>;

    // Start IAP initialization (query product info, restore purchases)
    virtual void initialise() = 0;

    // Check if a product was purchased
    virtual bool isPurchased(const juce::String& productId) const = 0;

    // Get product info (name, price, description)
    virtual juce::String getProductPrice(const juce::String& productId) const = 0;

    // Initiate purchase flow
    virtual void requestPurchase(const juce::String& productId, PurchaseCallback callback) = 0;

    // Restore purchases (iOS) / Restore from Play Store (Android)
    virtual void restorePurchases(PurchaseCallback callback) = 0;

    // Get unlock state
    bool isProUnlocked() const { return isPurchased("oriental_pro"); }

protected:
    // Common product IDs
    static constexpr const char* PRO_PRODUCT_ID = "oriental_pro";
};
