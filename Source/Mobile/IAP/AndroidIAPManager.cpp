/*
  ==============================================================================
    AndroidIAPManager.cpp - Implementation
  ==============================================================================

    In a production build, this would use JNI to call into Java:
    - com.djbilbox.orientalinstrument.BillingManager (Kotlin class)
    - Uses Google Play Billing Library v5+

    For this build, we provide a stub implementation that logs operations
    and allows manual testing. The full JNI bridge would be added once
    the Java/Kotlin Activity and gradle build are in place.
*/

#include "AndroidIAPManager.h"

AndroidIAPManager::AndroidIAPManager() = default;

AndroidIAPManager::~AndroidIAPManager()
{
    shutdown();
}

void AndroidIAPManager::initialize()
{
    juce::Logger::writeToLog("[IAP] Initializing Google Play Billing...");

    // In a real build with JNI:
    // initializeJNI();
    // connectBillingClient();

    // For now, just mark as initialized
    initialized = true;
    queryProducts();

    juce::Logger::writeToLog("[IAP] IAP Manager initialized");
}

void AndroidIAPManager::shutdown()
{
    if (initialized)
    {
        juce::Logger::writeToLog("[IAP] Shutting down...");
        initialized = false;
        // In a real build: disconnect BillingClient via JNI
    }
}

void AndroidIAPManager::queryProducts()
{
    if (!initialized)
        return;

    juce::Logger::writeToLog("[IAP] Querying available products...");

    {
        juce::ScopedLock lock(productLock);
        availableProducts.clear();
        availableProducts.push_back("com.djbilbox.orientalinstrument.full");
        // Future subscriptions:
        // availableProducts.push_back("com.djbilbox.orientalinstrument.monthly");
        // availableProducts.push_back("com.djbilbox.orientalinstrument.yearly");
    }

    // In a real build, this would be async; for now call immediately
    onProductsRetrieved(availableProducts);

    juce::Logger::writeToLog(
        juce::String("[IAP] Found ")
        + juce::String(static_cast<int>(availableProducts.size()))
        + " product(s)");
}

void AndroidIAPManager::startPurchaseFlow(const std::string& productId)
{
    if (!initialized)
    {
        juce::Logger::writeToLog("[IAP] ERROR: Not initialized");
        onPurchaseError("IAP not initialized");
        return;
    }

    juce::Logger::writeToLog(
        juce::String("[IAP] Starting purchase flow for: ")
        + productId.c_str());

    // In a real build, call Java via JNI to launch the purchase dialog
    // For now, this is a stub
}

void AndroidIAPManager::handlePurchaseSuccess(
    const std::string& productId,
    const std::string& purchaseToken)
{
    juce::Logger::writeToLog(
        juce::String("[IAP] Purchase SUCCESS: ")
        + productId.c_str()
        + " (token: "
        + purchaseToken.substr(0, 20).c_str()
        + "...)");

    if (productId == "com.djbilbox.orientalinstrument.full")
    {
        fullVersionUnlocked = true;
        juce::Logger::writeToLog("[IAP] Full version UNLOCKED!");
        // Notify processor to reload all 7 instruments, 70 presets, 8 effects
        // (instead of DEMO: 3 instruments, 9 presets, 2 effects)
    }

    onPurchaseSuccess(productId);
}

void AndroidIAPManager::handlePurchaseError(const std::string& errorMessage)
{
    juce::Logger::writeToLog(
        juce::String("[IAP] Purchase ERROR: ")
        + errorMessage.c_str());

    onPurchaseError(errorMessage);
}

void AndroidIAPManager::handlePurchaseCancelled()
{
    juce::Logger::writeToLog("[IAP] Purchase cancelled by user");
    onPurchaseCancelled();
}

void AndroidIAPManager::initializeJNI()
{
    // Stub for JNI initialization
    // In a real build:
    // - Call JNI_OnLoad() to attach VM
    // - Get reference to Java BillingManager class
    // - Cache method IDs for queryProducts(), startPurchase(), etc.
}

void AndroidIAPManager::connectBillingClient()
{
    // Stub for connecting to Google Play Billing Library
    // In a real build:
    // - Call Java BillingManager.connectAsync()
    // - Set up purchase update listener
    // - Register JNI callbacks for purchase results
}
