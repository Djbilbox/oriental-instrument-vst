/*
  ==============================================================================
    AndroidIAPManager.h - Google Play Billing Integration
  ==============================================================================

    Wraps Google Play Billing Library (GPBL) to handle in-app purchases.
    Allows users to upgrade from DEMO (freemium) to FULL version.

    Product IDs:
    - com.djbilbox.orientalinstrument.full   → unlocks all 7 instruments + 70 presets + 8 effects
    - (future) .monthly, .yearly subscriptions for premium content

    Uses JNI to bridge with Java/Kotlin BillingClient on the Android side.
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <vector>

/**
    Manages in-app purchase state and billing flow.
*/
class AndroidIAPManager
{
public:
    AndroidIAPManager();
    ~AndroidIAPManager();

    // ─ Initialization ─
    /**
        Initialize the IAP manager and connect to Google Play.
        Must be called from the UI thread.
    */
    void initialize();

    /**
        Shut down billing client gracefully.
    */
    void shutdown();

    // ─ Product Queries ─
    /**
        Fetch available products from Google Play Console.
        Async; results delivered via onProductsRetrieved callback.
    */
    void queryProducts();

    /**
        Get the list of cached products (populated after queryProducts).
    */
    const std::vector<std::string>& getAvailableProducts() const
    {
        return availableProducts;
    }

    // ─ Purchase Flow ─
    /**
        Initiate a purchase flow for the given product ID.
        @param productId e.g., "com.djbilbox.orientalinstrument.full"
    */
    void startPurchaseFlow(const std::string& productId);

    /**
        Handle the result of a purchase flow (called from Java via JNI).
        @param productId The purchased product
        @param purchaseToken JWT token from Google
    */
    void handlePurchaseSuccess(const std::string& productId,
                              const std::string& purchaseToken);

    void handlePurchaseError(const std::string& errorMessage);
    void handlePurchaseCancelled();

    // ─ State Queries ─
    bool isFullVersionUnlocked() const { return fullVersionUnlocked; }

    bool isInitialized() const { return initialized; }

    // ─ Callbacks (override in subclass or use std::function) ─
    virtual void onProductsRetrieved(
        const std::vector<std::string>& productIds) { }

    virtual void onPurchaseSuccess(const std::string& productId) { }

    virtual void onPurchaseError(const std::string& error) { }

    virtual void onPurchaseCancelled() { }

private:
    std::atomic<bool> initialized { false };
    std::atomic<bool> fullVersionUnlocked { false };

    std::vector<std::string> availableProducts;
    juce::CriticalSection productLock;

    // JNI helper (implemented in platform-specific code)
    void initializeJNI();
    void connectBillingClient();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AndroidIAPManager)
};
