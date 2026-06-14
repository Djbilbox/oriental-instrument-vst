#pragma once

#ifdef JUCE_IOS
    #import <StoreKit/StoreKit.h>
#endif

#include <JuceHeader.h>

class OrientalInstrumentProcessor;

/*
    iOSIAPManager.h — In-App Purchase manager using StoreKit2 (iOS 15.0+).

    Freemium model:
    - Demo tier (free): 3 instruments (Violin, Oud, Qanun), 3 presets each, 2 effects (Reverb, Delay)
    - Full tier (IAP): All 7 instruments, 10 presets each, all 8 effects

    Product identifiers:
    - "com.djbilbox.oriental.instruments" — unlock all instruments
    - "com.djbilbox.oriental.full" — unlock full feature set (future)

    Manages:
    - Product fetching from App Store
    - Purchase flow (request, validation, completion)
    - Entitlement verification via local cache + server validation (optional)
    - Restore purchases
*/

#ifdef JUCE_IOS

@interface OrientalInstrumentiOSIAPManager : NSObject

@property (nonatomic, assign, readonly) BOOL isFullVersionPurchased;
@property (nonatomic, assign, readonly) BOOL isLoadingProducts;
@property (nonatomic, copy) void (^purchaseCompletionHandler)(BOOL success, NSString* _Nullable error);

- (instancetype)initWithProcessor:(class OrientalInstrumentProcessor*)processor;

// Product management
- (void)loadProducts;
- (nullable SKProduct*)productForIdentifier:(NSString*)identifier;
- (NSArray<SKProduct*>*)availableProducts;

// Purchase flow
- (void)purchaseProductWithIdentifier:(NSString*)productID;
- (void)restorePurchases;

// Entitlement checking
- (BOOL)isProductUnlocked:(NSString*)productID;
- (void)updateEntitlements;

// Debug helpers
- (void)logEntitlementStatus;
- (void)simulatePurchaseForTesting:(NSString*)productID;

@end

#endif
