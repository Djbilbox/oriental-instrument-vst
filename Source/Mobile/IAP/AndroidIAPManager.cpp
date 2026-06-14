#include "AndroidIAPManager.h"

#if JUCE_ANDROID

AndroidIAPManager::AndroidIAPManager()
{
}

AndroidIAPManager::~AndroidIAPManager()
{
    if (billingClient)
    {
        // JNI cleanup: release billingClient reference
    }
}

void AndroidIAPManager::initialise()
{
    // Google Play Billing Library 6.0+
    // Initialize BillingClient and query products
    queryProducts();
}

void AndroidIAPManager::queryProducts()
{
    // BillingClient.queryProductDetailsAsync(productList, listener)
    // For now, hardcoded pending full Google Play integration
    ProductInfo proProduct;
    proProduct.productId = "oriental_pro";
    proProduct.displayName = "Oriental Instrument Pro";
    proProduct.displayPrice = "$4.99";
    productCache.set("oriental_pro", proProduct);
}

bool AndroidIAPManager::isPurchased(const juce::String& productId) const
{
    auto* purchased = purchaseCache.getPointer(productId);
    return purchased ? *purchased : false;
}

juce::String AndroidIAPManager::getProductPrice(const juce::String& productId) const
{
    auto* product = productCache.getPointer(productId);
    return product ? product->displayPrice : "N/A";
}

void AndroidIAPManager::requestPurchase(const juce::String& productId, PurchaseCallback callback)
{
    currentCallback = callback;
    // BillingClient.launchBillingFlow(activity, billingFlowParams)
    // Simulate success for now
    handlePurchaseResult(true, productId);
}

void AndroidIAPManager::restorePurchases(PurchaseCallback callback)
{
    currentCallback = callback;
    // BillingClient.queryPurchasesAsync(INAPP, listener)
    // Restore all purchases from Play Store
}

void AndroidIAPManager::handlePurchaseResult(bool success, const juce::String& productId)
{
    if (success)
    {
        purchaseCache.set(productId, true);
        if (currentCallback)
            currentCallback(PurchaseState::success, productId);
    }
    else
    {
        if (currentCallback)
            currentCallback(PurchaseState::failed, productId);
    }
    currentCallback = nullptr;
}

#endif // JUCE_ANDROID
