#include "iOSIAPManager.h"
#include "PluginProcessor.h"

#ifdef JUCE_IOS

// Product identifiers (must match App Store configuration)
static NSString* const kProductUnlockInstruments = @"com.djbilbox.oriental.instruments";
static NSString* const kProductUnlockFull = @"com.djbilbox.oriental.full";

// User defaults keys for entitlement caching
static NSString* const kEntitlementCacheKey = @"OrientalInstrument_Entitlements";

@interface OrientalInstrumentiOSIAPManager () <SKPaymentTransactionObserver, SKProductsRequestDelegate>
{
    OrientalInstrumentProcessor* audioProcessor;
    NSMutableDictionary<NSString*, SKProduct*>* products;
    NSMutableSet<NSString*>* unlockedProducts;
    BOOL loadingProducts;
}
@end

@implementation OrientalInstrumentiOSIAPManager

- (instancetype)initWithProcessor:(OrientalInstrumentProcessor*)processor
{
    self = [super init];
    if (self)
    {
        audioProcessor = processor;
        products = [[NSMutableDictionary alloc] init];
        unlockedProducts = [[NSMutableSet alloc] init];
        loadingProducts = NO;

        // Load cached entitlements
        [self loadCachedEntitlements];

        // Register as payment transaction observer
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];

        NSLog(@"[Oriental IAP] Manager initialized");
    }
    return self;
}

- (void)dealloc
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

#pragma mark - Product Management

- (void)loadProducts
{
    if (loadingProducts)
        return;

    _isLoadingProducts = YES;
    loadingProducts = YES;

    NSSet<NSString*>* productIdentifiers = [NSSet setWithObjects:
        kProductUnlockInstruments,
        kProductUnlockFull,
        nil];

    SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    request.delegate = self;
    [request start];

    NSLog(@"[Oriental IAP] Loading products...");
}

- (nullable SKProduct*)productForIdentifier:(NSString*)identifier
{
    return products[identifier];
}

- (NSArray<SKProduct*>*)availableProducts
{
    return [products allValues];
}

#pragma mark - SKProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest*)request
     didReceiveResponse:(SKProductsResponse*)response
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [products removeAllObjects];

        for (SKProduct* product in response.products)
        {
            products[product.productIdentifier] = product;

            NSNumberFormatter* formatter = [[NSNumberFormatter alloc] init];
            formatter.numberStyle = NSNumberFormatterCurrencyStyle;
            formatter.locale = product.priceLocale;
            NSString* priceString = [formatter stringFromNumber:product.price];

            NSLog(@"[Oriental IAP] Product loaded: %@ — %@ (%@)",
                  product.productIdentifier,
                  product.localizedTitle,
                  priceString);
        }

        if (response.invalidProductIdentifiers.count > 0)
        {
            NSLog(@"[Oriental IAP] Invalid product IDs: %@", response.invalidProductIdentifiers);
        }

        _isLoadingProducts = NO;
        loadingProducts = NO;
    });
}

- (void)request:(SKRequest*)request didFailWithError:(NSError*)error
{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSLog(@"[Oriental IAP] Product request failed: %@", error.localizedDescription);
        _isLoadingProducts = NO;
        loadingProducts = NO;

        if (self.purchaseCompletionHandler)
        {
            self.purchaseCompletionHandler(NO, error.localizedDescription);
        }
    });
}

#pragma mark - Purchase Flow

- (void)purchaseProductWithIdentifier:(NSString*)productID
{
    SKProduct* product = products[productID];
    if (!product)
    {
        NSLog(@"[Oriental IAP] Product not found: %@", productID);
        if (self.purchaseCompletionHandler)
        {
            self.purchaseCompletionHandler(NO, @"Product not found");
        }
        return;
    }

    SKMutablePayment* payment = [SKMutablePayment paymentWithProduct:product];
    payment.quantity = 1;
    [[SKPaymentQueue defaultQueue] addPayment:payment];

    NSLog(@"[Oriental IAP] Purchase initiated for %@", productID);
}

- (void)restorePurchases
{
    NSLog(@"[Oriental IAP] Restore purchases initiated");
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

#pragma mark - SKPaymentTransactionObserver

- (void)paymentQueue:(SKPaymentQueue*)queue
 updatedTransactions:(NSArray<SKPaymentTransaction*>*)transactions
{
    for (SKPaymentTransaction* transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self handlePurchaseSuccess:transaction];
                break;

            case SKPaymentTransactionStateFailed:
                [self handlePurchaseFailure:transaction];
                break;

            case SKPaymentTransactionStateRestored:
                [self handlePurchaseRestored:transaction];
                break;

            case SKPaymentTransactionStateDeferred:
            case SKPaymentTransactionStatePending:
                NSLog(@"[Oriental IAP] Purchase pending for %@", transaction.payment.productIdentifier);
                break;
        }
    }
}

- (void)handlePurchaseSuccess:(SKPaymentTransaction*)transaction
{
    NSLog(@"[Oriental IAP] Purchase successful: %@", transaction.payment.productIdentifier);

    [self unlockProduct:transaction.payment.productIdentifier];
    [self saveCachedEntitlements];

    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];

    if (self.purchaseCompletionHandler)
    {
        self.purchaseCompletionHandler(YES, nil);
    }
}

- (void)handlePurchaseFailure:(SKPaymentTransaction*)transaction
{
    NSLog(@"[Oriental IAP] Purchase failed: %@ — %@",
          transaction.payment.productIdentifier,
          transaction.error.localizedDescription);

    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];

    if (self.purchaseCompletionHandler)
    {
        self.purchaseCompletionHandler(NO, transaction.error.localizedDescription);
    }
}

- (void)handlePurchaseRestored:(SKPaymentTransaction*)transaction
{
    NSLog(@"[Oriental IAP] Purchase restored: %@", transaction.payment.productIdentifier);

    [self unlockProduct:transaction.original.payment.productIdentifier];
    [self saveCachedEntitlements];

    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

#pragma mark - Entitlement Management

- (void)unlockProduct:(NSString*)productID
{
    [unlockedProducts addObject:productID];
    NSLog(@"[Oriental IAP] Product unlocked: %@", productID);

    // Notify processor of feature unlock (reload preset manager, etc.)
    if (audioProcessor)
    {
        // This would trigger UI updates in the editor
        // audioProcessor->onIAPUnlock(productID);
    }
}

- (BOOL)isProductUnlocked:(NSString*)productID
{
    return [unlockedProducts containsObject:productID];
}

- (BOOL)isFullVersionPurchased
{
    return [self isProductUnlocked:kProductUnlockInstruments];
}

- (void)updateEntitlements
{
    [self loadCachedEntitlements];
    [self logEntitlementStatus];
}

#pragma mark - Entitlement Caching

- (void)saveCachedEntitlements
{
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSArray* entitlementsArray = [unlockedProducts allObjects];
    [defaults setObject:entitlementsArray forKey:kEntitlementCacheKey];
    [defaults synchronize];

    NSLog(@"[Oriental IAP] Entitlements cached: %@", entitlementsArray);
}

- (void)loadCachedEntitlements
{
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSArray* entitlementsArray = [defaults arrayForKey:kEntitlementCacheKey];

    [unlockedProducts removeAllObjects];
    if (entitlementsArray)
    {
        [unlockedProducts addObjectsFromArray:entitlementsArray];
    }

    NSLog(@"[Oriental IAP] Entitlements loaded: %@", [unlockedProducts allObjects]);
}

#pragma mark - Debug & Testing

- (void)logEntitlementStatus
{
    NSLog(@"[Oriental IAP] ═══ Entitlement Status ═══");
    NSLog(@"  Full Version: %@", _isFullVersionPurchased ? @"UNLOCKED" : @"LOCKED");
    NSLog(@"  Instruments: %@", [self isProductUnlocked:kProductUnlockInstruments] ? @"UNLOCKED" : @"LOCKED");
    NSLog(@"  Full Set: %@", [self isProductUnlocked:kProductUnlockFull] ? @"UNLOCKED" : @"LOCKED");
    NSLog(@"════════════════════════════════════");
}

- (void)simulatePurchaseForTesting:(NSString*)productID
{
    NSLog(@"[Oriental IAP] SIMULATOR: Simulating purchase of %@", productID);
    [self unlockProduct:productID];
    [self saveCachedEntitlements];
    [self logEntitlementStatus];
}

@end

#endif
