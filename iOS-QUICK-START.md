# iOS Quick Start — Oriental Instrument

## 30-Second Build

```bash
cd /path/to/ORIENTAL\ VST/DJBILBOX\ VST
mkdir -p build-ios && cd build-ios

# Configure (full version)
cmake -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -GXcode ..

# Build
cmake --build . --config Release --parallel 4
```

## Or: Demo/Freemium Version

```bash
cmake -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
      -DCMAKE_OSX_SYSROOT=iphoneos \
      -DORIENTAL_DEMO_IOS=ON \
      -GXcode ..

cmake --build . --config Release
```

## Launch in Xcode

```bash
open OrientalInstrument.xcodeproj
```

Select:
- **Target:** `OrientalInstrumentiOS`
- **Device:** iPhone 14 Pro (simulator) or your device
- **Product → Run** (Cmd+R)

## What You Get

**Full Version:**
- 7 instruments (Violin, Oud, Qanun, Ney, Ud, Qanun+, Rabab)
- 70 presets (10 per instrument)
- 8 effects (Reverb, Delay, Chorus, Distortion, Compressor, EQ, Phaser, Bitcrusher)

**Demo Version (with `-DORIENTAL_DEMO_IOS=ON`):**
- 3 instruments (Violin, Oud, Qanun)
- 9 presets (3 per instrument)
- 2 effects (Reverb, Delay)
- "Unlock Full" button triggers IAP

## File Map

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | iOS target config + Metal/StoreKit linking |
| `Source/Mobile/iOSStandalone.cpp` | App entry point, lifecycle |
| `Source/Mobile/UI/iOSViewController.mm` | Touch + MIDI + audio session |
| `Source/Mobile/UI/iOSLayoutAdapter.cpp` | Portrait/landscape layout |
| `Source/Mobile/IAP/iOSIAPManager.mm` | StoreKit2 purchases |

## Key Features

✓ **Audio** — AVAudioEngine, Metal GPU rendering  
✓ **Touch** — Multi-touch wheels, knobs, keyboard, presets  
✓ **MIDI** — Bluetooth/USB keyboards + controllers  
✓ **Orientation** — Portrait + landscape (auto-scale)  
✓ **IAP** — Freemium unlock via StoreKit2  
✓ **Offline** — No network needed (purchases cached)  

## Test Checklist

- [ ] Simulator launches without crash
- [ ] Audio plays (check Mac system volume)
- [ ] Touch input works (swipe, tap, drag)
- [ ] Device orientation rotates smoothly
- [ ] Presets load and sound different
- [ ] All 8 effects toggle on/off (or 2 if demo)

## Troubleshooting

**Simulator no audio?**
```
System Preferences → Sound → Output → Built-in Speaker (check Mac volume)
```

**Build fails?**
```bash
cmake --build . -- --verbose
# Check for JUCE module paths
```

**Presets not loading?**
```
Verify: -DORIENTAL_DEMO_IOS setting matches intended build
Demo should show 3 presets per instrument
```

## Next: App Store

1. Create app in App Store Connect
2. Configure IAP products
3. Build release & upload
4. Submit for review

See `iOS-BUILD.md` for full guide.

---

**Quick refs:**
- Bundle ID: `com.djbilbox.orientalinstrument.ios`
- Deployment: iOS 14.0+
- Audio: AVFoundation + Metal
- IAP: StoreKit2 (iOS 15.0+)
