# Oriental Instrument iOS Standalone App — Build Guide

## Overview

This document describes how to build Oriental Instrument as a native iOS app using JUCE + CMake + Xcode.

**System Requirements:**
- macOS 12.0+ with Xcode 13.0+
- iOS 14.0+ deployment target
- JUCE library (C++ v17 or later)
- CMake 3.22+

## Target Configuration

The iOS app (`OrientalInstrumentiOS`) is built from the existing C++ JUCE codebase:
- **DSP**: 7 instruments, maqam tuning system, 70 presets
- **Effects**: 8-effect chain (Reverb, Delay, Chorus, Distortion, Compressor, EQ, Phaser, Bitcrusher)
- **UI**: Touch-optimized version of desktop interface (portrait + landscape support)
- **Audio**: Metal rendering, AVAudioEngine integration, StoreKit2 IAP

### Freemium Model

Set `-DORIENTAL_DEMO_IOS=ON` to enable the demo version:
- **Instruments**: 3 (Violin, Oud, Qanun)
- **Presets per instrument**: 3
- **Effects**: 2 (Reverb, Delay)
- **IAP unlock**: Full 7 instruments, 10 presets each, all 8 effects

## Build Steps (macOS)

### 1. Prepare Build Directory

```bash
cd /path/to/Oriental\ VST/DJBILBOX\ VST
mkdir -p build-ios
cd build-ios
```

### 2. Configure with CMake

**Full Version:**
```bash
cmake -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -GXcode \
      ..
```

**Demo Version (Freemium):**
```bash
cmake -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DORIENTAL_DEMO_IOS=ON \
      -GXcode \
      ..
```

### 3. Build with Xcode

```bash
# Build Debug
cmake --build . --config Debug

# Build Release
cmake --build . --config Release --parallel 4
```

Or open the generated Xcode project:
```bash
open OrientalInstrument.xcodeproj
```

Then select the `OrientalInstrumentiOS` scheme and build.

### 4. Install to Device / Simulator

**Via Xcode:**
1. Select target: `OrientalInstrumentiOS`
2. Select destination: your iOS device or simulator
3. Product → Run (Cmd+R)

**Via command line:**
```bash
# List available devices
xcrun simctl list devices available

# Install to simulator
cmake --build . --config Debug --target OrientalInstrumentiOS -- -destination 'generic/platform=iOS Simulator'
```

## Project Structure

```
Source/
├── PluginProcessor.cpp/h     ← Synth audio engine (shared with VST)
├── PluginEditor.cpp/h         ← GUI (adapted for touch)
├── DSP/                        ← Pure synthesis DSP
│   ├── OrientalSynthesiser.cpp
│   ├── MaqamTuning.cpp
│   ├── FXChain/                ← All 8 effects
│   └── ...
├── GUI/                        ← JUCE UI components
│   ├── BackgroundComponent.cpp ← Responsive desert scene
│   ├── WheelComponent.cpp      ← Pitch/mod wheels (touch)
│   └── ...
├── Data/                       ← Presets + instrument profiles
│   ├── PresetManager.cpp       ← Freemium preset limit
│   └── InstrumentProfiles.cpp
└── Mobile/                     ← iOS-specific code
    ├── iOSStandalone.cpp       ← Main app entry point
    ├── UI/
    │   ├── iOSViewController.mm ← Touch + orientation handling
    │   └── iOSLayoutAdapter.cpp ← Responsive layout
    └── IAP/
        └── iOSIAPManager.mm    ← StoreKit2 integration

CMakeLists.txt                  ← Main build config (includes iOS target)
CMakeLists-iOS.txt              ← iOS toolchain setup (optional separate script)
```

## Audio Configuration

The iOS app automatically initializes AVAudioSession:

```
Category: PlayAndRecord (synth output + MIDI input)
Options: DuckOthers, DefaultToSpeaker, MixWithOthers
Mode: Measurement (optimized for synthesis)
```

This allows:
- Synth audio playback through speakers or headphones
- MIDI input from external controllers (via Bluetooth or USB)
- Simultaneous audio from other apps (music playback, calls)

## Touch & MIDI Input

### Touch Gestures

- **Pitch wheel**: Vertical drag on left side
- **Mod wheel**: Vertical drag on right side
- **Knobs**: Circular drag (angle → value)
- **Piano**: Tap/slide to play notes
- **Preset browser**: Tap presets, swipe to navigate

### MIDI Input

Connect via:
- **Bluetooth MIDI**: Wireless keyboards, controllers
- **USB MIDI**: Wired controller (with USB adapter / Camera Connection Kit)

Events are forwarded to `OrientalInstrumentProcessor` and trigger synthesis.

## Device Orientation

The app supports:
- **Portrait**: Default layout (keyboard at bottom, controls on sides)
- **Landscape (left/right)**: Controls reposition (wheels on sides, keyboard vertical)

Layout adaptation is handled by `iOSLayoutAdapter` (responsive bounds calculation).

## In-App Purchases (IAP)

The demo version includes a purchase button (configurable).

Product IDs:
- `com.djbilbox.oriental.instruments` — Unlock all instruments
- `com.djbilbox.oriental.full` — Unlock full feature set (future)

### IAP Testing

**Sandbox Testing:**
1. Create test user account in App Store Connect
2. On device, sign out of App Store
3. Launch app → try to purchase
4. Sign in with sandbox credentials when prompted
5. Purchase is recorded but not charged

**Development (Simulator):**
- IAP is stubbed; purchases automatically succeed if registered in scheme
- Use `iOSIAPManager::simulatePurchaseForTesting()` for testing

### Entitlement Caching

Purchased entitlements are cached in `NSUserDefaults` with key:
```
"OrientalInstrument_Entitlements"
```

On app launch, the cache is loaded and pre-features are unlocked.

## Compile Definitions

### iOS-Specific Flags

```cmake
ORIENTAL_IOS=1              # Enable iOS code paths
JUCE_USE_METAL=1            # Metal rendering (GPU)
JUCE_COREGRAPHICS_DRAW_ASYNC=1  # Async UI rendering
```

### Freemium Flags

```cmake
ORIENTAL_DEMO_IOS=1         # Enable demo mode
ORIENTAL_DEMO=1             # (Inherited) Limit presets to 5 per instrument
```

## Troubleshooting

### Build Fails: "JUCE modules not found"

Ensure JUCE is in your `$PATH` or set:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/JUCE/cmake ..
```

### Build Fails: "Undefined references to UIKit"

Add to CMakeLists.txt:
```cmake
target_link_libraries(OrientalInstrumentiOS PRIVATE "-framework UIKit")
```

(Already included in the main CMakeLists.txt.)

### Audio Not Playing on Simulator

- Simulator audio is routed to Mac speakers (check system volume)
- Some effects may be CPU-heavy on simulator; reduce effect amounts
- Test on real device for true performance

### Orientation Not Rotating

Ensure `AppDelegate` sets `shouldAutorotate = YES` and `supportedInterfaceOrientations`.
(Already configured in `iOSViewController.mm`.)

### Presets Not Loading (Demo Mode)

Demo mode limits `PresetManager` to first 3 instruments, 3 presets per instrument.
Check compile definition: `-DORIENTAL_DEMO_IOS=ON`

## Performance Optimization

### CPU Usage

- Metal rendering offloads UI to GPU
- Async coreg-graphics reduces main-thread stalls
- DSP is optimized for real-time (no allocations in audio thread)

### Memory

- Preset data is lazy-loaded
- Sample engine pre-allocates buffers at session start
- Background component caches rendered scenes

### Battery

- Idle synth consumes ~5-10% CPU
- Effects chain only processes if enabled
- App respects background audio limitations

## Publishing to App Store

### Requirements

1. **Code Signing**
   - App ID: `com.djbilbox.orientalinstrument.ios` (hardcoded in CMakeLists.txt)
   - Certificate: iOS App Development or Distribution

2. **App Store Connect**
   - Create app bundle
   - Configure IAP products
   - Build artifacts in Xcode → Product → Organizer

3. **Review Submission**
   - Screenshots (various device sizes)
   - Privacy policy (mention MIDI input, no data collection)
   - Category: Music Production
   - Content rating: 4+ (no objectionable content)

4. **Testing**
   - Use TestFlight for beta testing
   - Verify IAP flow with sandbox users

## References

- [JUCE iOS Documentation](https://docs.juce.com/master/iOS.html)
- [StoreKit2 Guide](https://developer.apple.com/documentation/storekit)
- [AVAudioSession Best Practices](https://developer.apple.com/documentation/avfoundation/avaudiosession)
- [Metal Best Practices](https://developer.apple.com/documentation/metal/best_practices)

## Support

For issues or questions:
- Check build logs: `cmake --build . -- --verbose`
- Debug in Xcode: Xcode → Product → Scheme → Edit Scheme → Run → Diagnostics
- Profile: Xcode → Product → Profile → Instruments (select "System Trace" or "Metal System Trace")
