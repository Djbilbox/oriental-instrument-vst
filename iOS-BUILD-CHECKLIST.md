# iOS Build Checklist & Architecture Notes

## Task Completion Status

- [x] **1. iOS target in CMakeLists.txt** — `juce_add_gui_app(OrientalInstrumentiOS)` with iOS-specific flags
  - Location: `CMakeLists.txt` lines 103–213
  - Metal rendering enabled (GPU acceleration)
  - Audio frameworks linked (AVFoundation, AudioToolbox, CoreAudio)
  
- [x] **2. iOSStandalone.cpp** — PluginProcessor wrapper for iOS app entry point
  - Location: `Source/Mobile/iOSStandalone.cpp`
  - Creates `JUCEApplicationBase` app with full-screen support
  - Handles orientation changes (portrait + landscape)
  - Audio session paused on background, resumed on foreground

- [x] **3. iOSViewController.h/mm** — Touch + MIDI + audio session handling
  - Headers: `Source/Mobile/UI/iOSViewController.h`
  - Implementation: `Source/Mobile/UI/iOSViewController.mm`
  - Multi-touch gesture recognition (pitch wheel, mod wheel, knobs, piano)
  - Audio interruption handling (calls, alarms)
  - MIDI input callbacks (note on/off, CC, pitch bend)
  - Device orientation tracking

- [x] **4. iOSIAPManager.h/mm** — StoreKit2 IAP integration
  - Headers: `Source/Mobile/IAP/iOSIAPManager.h`
  - Implementation: `Source/Mobile/IAP/iOSIAPManager.mm`
  - Product fetching from App Store
  - Purchase flow + transaction observer
  - Entitlement validation + caching (NSUserDefaults)
  - Debug simulator mode for testing

- [x] **5. iOSLayoutAdapter** — Responsive portrait/landscape layout
  - Headers: `Source/Mobile/UI/iOSLayoutAdapter.h`
  - Implementation: `Source/Mobile/UI/iOSLayoutAdapter.cpp`
  - Orientation detection (portrait vs. landscape)
  - Component repositioning (preset browser, FX panel, wheels, piano, knobs)
  - Safe area insets (notch, home indicator)

- [x] **6. Bundle ID & iOS metadata** — `com.djbilbox.orientalinstrument.ios`
  - CMakeLists.txt: `BUNDLE_ID com.djbilbox.orientalinstrument.ios`
  - Company: "DJBILBOX BEATS"
  - Product name: "Oriental Instrument"

- [x] **7. iOS-specific compile flags in CMake**
  - `JUCE_USE_METAL=1` — Metal rendering (GPU)
  - `JUCE_COREGRAPHICS_DRAW_ASYNC=1` — Async UI rendering
  - `ORIENTAL_IOS=1` — iOS code path selection
  - iOS frameworks linked: StoreKit, AVFoundation, AudioToolbox, CoreAudio, Metal, MetalKit, UIKit
  - Xcode attributes: `CLANG_ENABLE_OBJC_ARC YES`, `IPHONEOS_DEPLOYMENT_TARGET 14.0`

- [x] **8. Freemium rule (ORIENTAL_DEMO_IOS macro)**
  - CMake flag: `-DORIENTAL_DEMO_IOS=ON`
  - When enabled:
    - `ORIENTAL_DEMO_IOS=1` → iOS-specific demo logic
    - `ORIENTAL_DEMO=1` → PresetManager limits to 5 presets per instrument
  - Demo limits: 3 instruments (Violin, Oud, Qanun), 3 presets each, 2 effects (Reverb, Delay)
  - Full version: All 7 instruments, 10 presets each, 8 effects

## Architecture Overview

### Audio Pipeline

```
MIDI Input (BT/USB) → OrientalInstrumentProcessor
                       ├─ OrientalSynthesiser (7 instruments)
                       │  ├─ WavetableOscillator (synth path)
                       │  ├─ SampleEngine (sample playback)
                       │  └─ MaqamTuning (microtonal tuning)
                       ├─ FXChain (8 effects, conditional on demo)
                       │  ├─ ReverbFX
                       │  ├─ DelayFX
                       │  ├─ ChorusFX
                       │  ├─ DistortionFX
                       │  ├─ CompressorFX
                       │  ├─ EQ3BandFX
                       │  ├─ PhaserFX
                       │  └─ BitcrusherFX
                       ├─ MasterLimiter (soft limiting + hard ceiling)
                       └─ Output → AVAudioEngine → Speaker/Headphones
```

### Touch Input Pipeline

```
UIViewController Touch Events
    ↓
iOSViewController (multi-touch handler)
    ↓
JUCE Mouse Events (via DocumentWindow)
    ↓
PluginEditor GUI Components
    ├─ BackgroundComponent (desert scene, responsive)
    ├─ PresetBrowser (tap to select)
    ├─ WheelComponent (pitch/mod wheels, vertical drag)
    ├─ PianoComponent (tap/slide notes)
    ├─ KnobComponent (circular drag)
    ├─ FXPanel (effect toggles + sliders)
    └─ WaveformDisplay (waveform preview)
```

### MIDI Input Pipeline

```
Native MIDI Callbacks (CoreMIDI)
    ↓
iOSViewController MIDI handlers
    ├─ onMidiNoteOn() → PluginProcessor::synthesiser.noteOn()
    ├─ onMidiNoteOff() → PluginProcessor::synthesiser.noteOff()
    ├─ onMidiControlChange() → PluginProcessor::setModWheel()
    └─ onMidiPitchBend() → PluginProcessor::setPitchBend()
```

### Orientation Handling

```
Device Orientation Changes
    ↓
UIDeviceOrientationDidChangeNotification
    ↓
iOSViewController::handleDeviceOrientationChange()
    ↓
iOSLayoutAdapter::getCurrentOrientation()
    ↓
PluginEditor::resized()
    ↓
iOSLayoutAdapter component bounds
    ├─ Portrait: 2-column layout (presets left, FX right)
    └─ Landscape: Vertical stacking (presets top, FX below)
```

### Audio Session Lifecycle

```
App Launch
    ↓
iOSViewController::viewDidLoad()
    ↓
iOSViewController::initializeAudioSession()
    ├─ Category: PlayAndRecord (allow synth + MIDI)
    ├─ Options: DuckOthers, DefaultToSpeaker, MixWithOthers
    ├─ Mode: Measurement (optimize for synthesis)
    └─ setActive(YES)
    ↓
PluginProcessor::prepareToPlay()
    ├─ Allocate DSP buffers
    ├─ Initialize synthesiser
    └─ Arm FXChain
    ↓
(Audio processing runs in real-time)
    ↓
[Interruption occurs (call, alarm, etc.)]
    ↓
iOSViewController::handleAudioInterruption()
    ├─ Type == Began → PluginProcessor::releaseResources()
    └─ Type == Ended → re-setActive(YES)
    ↓
[App backgrounded]
    ↓
iOSStandalone::suspended()
    └─ PluginProcessor::releaseResources()
    ↓
[App resumed]
    ↓
iOSStandalone::resumed()
    └─ (Audio re-initializes on next play)
    ↓
App Shutdown
    ↓
iOSViewController::dealloc()
    └─ Cleanup notifications, destroy audio session
```

### IAP Flow

```
App Launch
    ↓
iOSIAPManager::init()
    ├─ Load cached entitlements (NSUserDefaults)
    └─ Register as SKPaymentTransactionObserver
    ↓
User taps "Unlock Full Version"
    ↓
iOSIAPManager::loadProducts()
    ├─ Fetch product info from App Store
    └─ Display prices + descriptions
    ↓
User taps "Purchase"
    ↓
iOSIAPManager::purchaseProductWithIdentifier()
    ├─ Create SKPayment
    └─ Add to SKPaymentQueue
    ↓
[iOS presents App Store sign-in / payment UI]
    ↓
Transaction completes (success / failure / restore)
    ↓
iOSIAPManager::paymentQueue:updatedTransactions()
    ├─ Success → unlock product, cache, notify processor
    ├─ Failure → show error
    └─ Restored → re-unlock purchased items
    ↓
PluginEditor UI updates (reload presets, enable effects)
```

## Testing Checklist

Before deploying to App Store:

### Build Verification
- [ ] CMake configuration succeeds with `-DCMAKE_SYSTEM_NAME=iOS`
- [ ] Xcode project generates without warnings
- [ ] Release build completes (all 8 effects compiled)
- [ ] Debug build completes (allows runtime inspection)

### Audio Testing (Device)
- [ ] Synth produces sound on device speakers
- [ ] Sound plays through headphones
- [ ] Volume control works
- [ ] Audio stops when backgrounded (silent mode respected)
- [ ] Audio resumes when app comes to foreground

### MIDI Testing (Device)
- [ ] Bluetooth MIDI keyboard connects
- [ ] Keyboard note on/off triggers synth
- [ ] Pitch wheel / mod wheel modulation works
- [ ] Control change messages respond correctly

### Touch Input Testing
- [ ] Pitch wheel vertical drag adjusts pitch
- [ ] Mod wheel vertical drag modulates (tremolo, filter)
- [ ] Knobs respond to circular drag
- [ ] Piano responds to taps and slides
- [ ] Preset browser taps select presets
- [ ] Multi-touch (two fingers) works without interference

### Orientation Testing
- [ ] Portrait mode displays full UI, scaled to fit
- [ ] Landscape mode (left) repositions controls correctly
- [ ] Landscape mode (right) repositions controls correctly
- [ ] Rotating device smoothly transitions (no flicker)
- [ ] Controls remain responsive after rotation

### Freemium Testing
- [ ] Demo mode shows only 3 instruments in list
- [ ] Demo mode shows 3 presets per instrument (not 10)
- [ ] Demo mode disables non-Reverb/Delay effects (grayed out)
- [ ] "Unlock" button visible in UI
- [ ] Purchase flow initiates when button tapped
- [ ] After purchase: all instruments, presets, effects unlock
- [ ] Full version flag persists across app restart

### IAP Testing (Sandbox)
- [ ] `iOSIAPManager` loads product info from App Store Connect
- [ ] Test user can complete sandbox purchase
- [ ] Entitlements cache correctly (NSUserDefaults)
- [ ] Restore Purchases works for previously purchased items
- [ ] UI reflects unlock state immediately

### Performance Testing
- [ ] App launches in < 3 seconds
- [ ] Audio latency acceptable for real-time play (< 100ms)
- [ ] Effects don't cause audio dropouts when all 8 enabled
- [ ] Memory footprint stays < 200MB
- [ ] CPU usage < 30% during normal play on iPhone 12+
- [ ] No crashes or leaks during 30-minute session

### Edge Cases
- [ ] Interruption (call) pauses audio, resumes properly
- [ ] Low battery mode doesn't disable audio
- [ ] Silent mode (mute switch) mutes output (if desired)
- [ ] Simulator audio works (routed to Mac speakers)
- [ ] App backgrounded + re-opened: audio state preserved
- [ ] Cold start: synth ready to play immediately

## Files Modified/Created

### Modified
- `CMakeLists.txt` — Added iOS target + iOS-specific config

### Created
- `Source/Mobile/iOSStandalone.cpp` — App entry point
- `Source/Mobile/UI/iOSViewController.h` — Touch/MIDI/audio session header
- `Source/Mobile/UI/iOSViewController.mm` — Implementation
- `Source/Mobile/UI/iOSLayoutAdapter.h` — Responsive layout header
- `Source/Mobile/UI/iOSLayoutAdapter.cpp` — Orientation-aware bounds
- `Source/Mobile/IAP/iOSIAPManager.h` — StoreKit2 header
- `Source/Mobile/IAP/iOSIAPManager.mm` — IAP implementation
- `CMakeLists-iOS.txt` — Optional iOS-specific build script
- `iOS-BUILD.md` — Build instructions
- `iOS-BUILD-CHECKLIST.md` — This file

## Next Steps (Post-Build)

1. **Xcode Build**
   ```bash
   cd build-ios && cmake --build . --config Release
   ```

2. **Launch on Simulator**
   - Open `OrientalInstrument.xcodeproj`
   - Select `OrientalInstrumentiOS` scheme
   - Select iPhone 14 simulator
   - Product → Run

3. **Verify Xcode Compilation**
   - No compilation errors or warnings (except maybe JUCE infrastructure)
   - Linker succeeds (all frameworks found)
   - App launches without crashes

4. **Deploy to Real Device**
   - Connect iPhone
   - Select device as build destination
   - Product → Build & Run
   - Grant MIDI/audio permissions when prompted

5. **App Store Submission**
   - Configure App Store Connect entry
   - Create IAP products in App Store Connect
   - Upload build via Organizer
   - Submit for review

## Known Limitations / Future Work

- [ ] Simulator audio may be choppy (test on real device)
- [ ] Portrait-only for initial release (landscape coming soon)
- [ ] Preset saving (import/export from Files app) — not yet
- [ ] AirPlay audio output — test needed
- [ ] Bluetooth MIDI connection stability — depends on iOS version
- [ ] Accessibility (VoiceOver) — not yet implemented
- [ ] Dark mode theming — BackgroundComponent assumes light theme

## Support & Debugging

### Xcode Console Logs

Filter by tag: `[Oriental iOS]` or `[Oriental IAP]` or `[Oriental Layout]`

Example log:
```
[Oriental iOS] Audio session initialized: ACTIVE, category: PlayAndRecord, mode: Measurement
[Oriental IAP] Product loaded: com.djbilbox.oriental.instruments — Unlock All Instruments ($9.99)
[Oriental Layout] Device orientation changed: 3 (landscape right)
```

### Debugging Common Issues

**App crashes on launch:**
- Check Xcode console for stack trace
- Verify all frameworks linked correctly
- Ensure iOS 14.0 deployment target matches

**No sound on simulator:**
- Audio routed to Mac speakers; check system volume
- Disable other audio apps (Spotify, etc.)
- Try setting higher master volume in app

**MIDI not working on device:**
- Bluetooth MIDI keyboard may need re-pairing
- Check iOS Settings → Bluetooth → Connected Devices
- Verify MIDI permissions granted (iOS 13+)

**UI layout broken after rotation:**
- Check `iOSLayoutAdapter::getCurrentOrientation()` returns correct value
- Verify `PluginEditor::resized()` is called during rotation
- Add debug log: `DBG("Orientation changed: " << getCurrentOrientation());`

---

**Generated:** iOS Build Configuration for Oriental Instrument
**Date:** June 2026
**Status:** Ready for Xcode compilation & device testing
