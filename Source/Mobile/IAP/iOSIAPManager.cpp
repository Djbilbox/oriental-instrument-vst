#include "iOSIAPManager.h"

#if JUCE_IOS

iOSIAPManager::iOSIAPManager()
{
}

iOSIAPManager::~iOSIAPManager()
{
}

void iOSIAPManager::initialise()
{
    // StoreKit 2 initialization
    // Query products: @"oriental_pro"
    queryProducts();

    // Restore previous purchases
    // restorePurchases([](auto state, auto id) {});
}

void iOSIAPManager::queryProducts()
{
    // StoreKit 2: SKProduct.request async
    // For now, hardcoded product info pending StoreKit integration
    ProductInfo proProduct;
    proProduct.productId = "oriental_pro";
    proProduct.displayName = "Oriental Instrument Pro";
    proProduct.displayPrice = "$4.99";
    productCache.set("oriental_pro", proProduct);
}

bool iOSIAPManager::isPurchased(const juce::String& productId) const
{
    auto* purchased = purchaseCache.getPointer(productId);
    return purchased ? *purchased : false;
}

juce::String iOSIAPManager::getProductPrice(const juce::String& productId) const
{
    auto* product = productCache.getPointer(productId);
    return product ? product->displayPrice : "N/A";
}

void iOSIAPManager::requestPurchase(const juce::String& productId, PurchaseCallback callback)
{
    currentCallback = callback;
    // StoreKit 2: SKPaymentQueue.default().add(payment)
    // For now, simulate success
    handlePurchaseResult(true, productId);
}

void iOSIAPManager::restorePurchases(PurchaseCallback callback)
{
    currentCallback = callback;
    // StoreKit 2: AppStore.sync() → check receipt
    // Restore all purchases from Apple receipt
}

void iOSIAPManager::handlePurchaseResult(bool success, const juce::String& productId)
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

#endif // JUCE_IOS
