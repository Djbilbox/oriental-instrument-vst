# Oriental Instrument Mobile Build Guide

Build iOS and Android standalone apps from the shared JUCE codebase.

## Prerequisites

### iOS
- macOS 12.0+
- Xcode 14.0+
- iOS 13.0+ SDK

### Android
- Android NDK r24+ (`$ANDROID_NDK` environment variable)
- Android SDK API 30+
- Java 11+ (for Gradle)

## Build Instructions

### iOS

```bash
# Build for device (arm64)
./build-ios.sh Release arm64

# Build for simulator (x86_64)
./build-ios.sh Release x86_64

# Output: build-ios-arm64/Oriental\ Instrument.app
```

Then in Xcode:
1. Open the generated `.xcodeproj`
2. Select target `OrientalInstrumentiOS`
3. Sign with your Apple developer account
4. Archive → Distribute to App Store

### Android

```bash
# Build for arm64-v8a (most devices)
./build-android.sh Release arm64-v8a

# Build for multiple ABIs
for abi in arm64-v8a armeabi-v7a x86_64; do
  ./build-android.sh Release $abi
done

# Output: build-android-arm64-v8a/OrientalInstrument-Release.apk
```

Then:
1. Sign APK with your release keystore
2. Upload to Google Play Console

## Project Structure

```
Source/Mobile/
├── Main.cpp                    # App entry point
├── MobileAudioContext.{h,cpp}  # Audio engine wrapper
├── IAP/                        # In-app purchase
│   ├── IAPManager.h            # Abstract interface
│   ├── iOSIAPManager.*         # StoreKit2 implementation
│   └── AndroidIAPManager.*     # Google Play Billing
├── UI/
│   ├── MobileUIController.*    # Touch UI base
│   └── FreemiumIndicator.*     # Demo watermark + purchase prompt
├── iOS/
│   └── MainViewController.*    # iOS-specific app root
└── Android/
    └── MainActivity.*          # Android-specific app root
```

## Freemium Model

### Demo (Free)
- 3 instruments: Violin, Oud, Ney
- 3 presets per instrument
- Limited effects (Reverb + Compression only)
- Watermark + "Tap to Unlock" overlay

### Pro (In-App Purchase $4.99)
- All 7 instruments
- 10 presets per instrument
- Full 8-effect chain
- No watermark

### Runtime Control

```cpp
PresetManager& pm = processor.getPresetManager();
pm.setProUnlocked(iapManager->isProUnlocked());

// Check preset availability
if (!pm.isPresetAvailable(presetIndex))
    showPurchasePrompt();
```

## IAP Configuration

### iOS (StoreKit 2)

1. **App Store Connect**:
   - Create product ID: `oriental_pro`
   - Set price tier (tier 2 = $1.99 USD, tier 3 = $2.99, etc.)
   - Submit for review

2. **Code**:
   ```cpp
   iOSIAPManager iap;
   iap.initialise();  // Query products + restore purchases
   iap.requestPurchase("oriental_pro", [](auto state, auto id) {
       if (state == PurchaseState::success)
           setProUnlocked(true);
   });
   ```

### Android (Google Play Billing)

1. **Google Play Console**:
   - Create product ID: `oriental_pro`
   - Set price
   - Submit for review

2. **Code**:
   ```cpp
   AndroidIAPManager iap;
   iap.initialise();
   iap.requestPurchase("oriental_pro", [](auto state, auto id) {
       if (state == PurchaseState::success)
           setProUnlocked(true);
   });
   ```

## Testing

### iOS
```bash
# Run on simulator
cmake --build build-ios-x86_64 --target OrientalInstrumentiOS
xcrun simctl install booted build-ios-x86_64/Oriental\ Instrument.app
xcrun simctl launch booted com.djbilbox.orientalinstrument.ios
```

### Android
```bash
# Build & install on connected device
./build-android.sh Release arm64-v8a
adb install -r build-android-arm64-v8a/OrientalInstrument-Release.apk
adb shell am start -n com.djbilbox.orientalinstrument.android/.MainActivity
```

## Troubleshooting

### CMake not finding JUCE
Ensure JUCE is cloned in `JUCE/` directory or set `-DJUCE_DIR=/path/to/JUCE`.

### iOS code signing
Set `DEVELOPMENT_TEAM` in CMakeLists.txt or Xcode project settings.

### Android NDK not found
```bash
export ANDROID_NDK=/path/to/ndk/r24
```

### Audio not working in simulator
- iOS Simulator audio routing is limited; test on device
- Android Emulator: ensure audio output is enabled in AVD settings

## References

- [JUCE Mobile Documentation](https://docs.juce.com/)
- [iOS StoreKit 2 Guide](https://developer.apple.com/storekit/)
- [Google Play Billing Guide](https://developer.android.com/google/play/billing)
