# Oriental Instrument Android Build - Index & Quick Reference

**Last Updated:** 2026-06-14  
**Status:** SCAFFOLDING COMPLETE ✓

---

## Quick Navigation

### 📋 Start Here
1. **[BUILD_SUMMARY.md](BUILD_SUMMARY.md)** — Executive summary of all deliverables
2. **[ANDROID_BUILD_STATUS.md](ANDROID_BUILD_STATUS.md)** — Detailed status, blockers, and next phases

### 🛠 Setup & Build
- **[android/README_ANDROID_BUILD.md](android/README_ANDROID_BUILD.md)** — Complete build guide (setup → deployment)
- **[android/local.properties.template](android/local.properties.template)** — Configure SDK/NDK paths (copy to local.properties)

### 📁 C++ Source Files (NEW)

#### Mobile Integration
- **[Source/Mobile/AndroidStandalone.cpp](Source/Mobile/AndroidStandalone.cpp)** — JUCE app entry point
  - Initializes processor, creates main window, starts IAP manager

- **[Source/Mobile/AndroidActivity.h](Source/Mobile/AndroidActivity.h)** — Header
- **[Source/Mobile/AndroidActivity.cpp](Source/Mobile/AndroidActivity.cpp)** — Implementation
  - Touch input handling (MIDI notes)
  - Accelerometer (pitch bend), gyroscope (mod wheel)
  - Lifecycle management, audio focus

#### UI & Responsiveness
- **[Source/Mobile/UI/AndroidUIAdapter.h](Source/Mobile/UI/AndroidUIAdapter.h)** — Header
- **[Source/Mobile/UI/AndroidUIAdapter.cpp](Source/Mobile/UI/AndroidUIAdapter.cpp)** — Implementation
  - Dynamic scaling (0.75x–1.0x based on screen size)
  - Safe area insets, freemium banner
  - Responsive layout adaptation

#### In-App Purchase
- **[Source/Mobile/IAP/AndroidIAPManager.h](Source/Mobile/IAP/AndroidIAPManager.h)** — Header
- **[Source/Mobile/IAP/AndroidIAPManager.cpp](Source/Mobile/IAP/AndroidIAPManager.cpp)** — Implementation
  - Google Play Billing wrapper
  - Product query, purchase flow
  - JNI stubs for Java integration

### 🏗 Gradle Build Configuration

#### Root Level
- **[android/build.gradle](android/build.gradle)** — Gradle plugins, repositories
- **[android/settings.gradle](android/settings.gradle)** — Project structure (`:app` module)
- **[android/gradle.properties](android/gradle.properties)** — JVM, parallelization, CMake flags
- **[android/gradlew.bat](android/gradlew.bat)** — Windows Gradle wrapper

#### App Level
- **[android/app/build.gradle](android/app/build.gradle)** — Main configuration
  - NDK 26.0.10792818, API 21–34
  - CMake 3.22.1 integration
  - Google Play Billing v6.0.1
  - Build types (debug/release)

### 📝 Android Configuration
- **[android/AndroidManifest.xml](android/AndroidManifest.xml)** — App manifest
  - Package: com.djbilbox.orientalinstrument
  - Permissions: RECORD_AUDIO, INTERNET, BILLING, LOCATION
  - Activities, services, sensors

### 📖 CMake Configuration
- **[CMakeLists.txt](CMakeLists.txt)** — Root CMake (added Android section)
  - Lines 93–176: Android standalone target
  - Conditional: `if(ANDROID) ... endif()`
  - Links OpenSLES + Android framework

---

## File Structure

```
OrientalInstrument/
├── CMakeLists.txt                          (MODIFIED: +84 lines Android section)
├── Source/
│   ├── Mobile/                             (NEW DIRECTORY)
│   │   ├── AndroidStandalone.cpp           (NEW)
│   │   ├── AndroidActivity.h               (NEW)
│   │   ├── AndroidActivity.cpp             (NEW)
│   │   ├── UI/                             (NEW DIRECTORY)
│   │   │   ├── AndroidUIAdapter.h          (NEW)
│   │   │   └── AndroidUIAdapter.cpp        (NEW)
│   │   └── IAP/                            (NEW DIRECTORY)
│   │       ├── AndroidIAPManager.h         (NEW)
│   │       └── AndroidIAPManager.cpp       (NEW)
│   ├── PluginProcessor.h/cpp               (EXISTING, used by Android)
│   ├── PluginEditor.h/cpp                  (EXISTING, used by Android)
│   ├── DSP/                                (EXISTING, portable to Android)
│   │   ├── OrientalSynthesiser.*
│   │   ├── SampleEngine.*
│   │   ├── WavetableOscillator.*
│   │   ├── FXChain/
│   │   │   ├── ReverbFX.*
│   │   │   ├── DelayFX.*
│   │   │   ├── ChorusFX.*
│   │   │   ├── DistortionFX.*
│   │   │   ├── CompressorFX.*
│   │   │   ├── EQ3BandFX.*
│   │   │   ├── PhaserFX.*
│   │   │   └── BitcrusherFX.*
│   │   └── ...
│   ├── Data/
│   │   ├── PresetManager.*                 (Freemium-aware)
│   │   └── InstrumentProfiles.*
│   └── GUI/
│       ├── BackgroundComponent.*           (Responsive for mobile)
│       └── ...
├── android/                                (NEW DIRECTORY)
│   ├── build.gradle                        (NEW)
│   ├── app/
│   │   └── build.gradle                    (NEW)
│   ├── settings.gradle                     (NEW)
│   ├── gradle.properties                   (NEW)
│   ├── local.properties.template           (NEW)
│   ├── gradlew.bat                         (NEW)
│   ├── AndroidManifest.xml                 (NEW)
│   └── README_ANDROID_BUILD.md             (NEW)
├── BUILD_SUMMARY.md                        (NEW)
├── ANDROID_BUILD_STATUS.md                 (NEW)
└── ANDROID_BUILD_INDEX.md                  (NEW: This file)
```

---

## Key Concepts

### Freemium Mode (ORIENTAL_DEMO_ANDROID)

**DEMO (Free):**
- 3 instruments: Violin, Oud, Ney
- 3 presets each (9 total)
- 2 effects: Reverb, Delay
- UI banner prompting unlock

**FULL (Unlocked via IAP):**
- 7 instruments: + Qanun, Darbuka, Rababah, Mizmar
- 10 presets each (70 total)
- 8 effects: + Chorus, Distortion, Compressor, EQ, Phaser, Bitcrusher

### Audio & Sensors

| Input | Source | Mapping |
|-------|--------|---------|
| Touch | Screen (piano-like) | Normalized (X, Y) → MIDI 36–96 |
| Accelerometer | Device tilt | Accel Y → Pitch bend ±12 semitones |
| Gyroscope | Device rotation | Gyro Y → Mod wheel 0–1 |

### Build System

```
Gradle (CMake integration)
  ↓
CMakeLists.txt (if ANDROID)
  ↓
Android NDK Clang (C++17)
  ↓
Compiles OrientalInstrumentAndroid executable
  ↓
Links: JUCE + OpenSLES + Android framework
  ↓
APK packaged with .so libs (arm64-v8a, armeabi-v7a)
```

---

## Workflow

### 1. Initial Setup (One-time)

```bash
# Install toolchain (JDK, Android SDK/NDK, CMake, Gradle)
# See: android/README_ANDROID_BUILD.md → Prerequisites

# Configure local paths
cd android
cp local.properties.template local.properties
# Edit: sdk.dir, ndk.dir (your paths)

# Verify environment
java -version
echo $ANDROID_SDK_ROOT
echo $ANDROID_NDK_HOME
cmake --version
```

### 2. Build Debug APK

```bash
cd android
./gradlew clean
./gradlew assembleDebug
# Output: app/build/outputs/apk/debug/app-debug.apk (~60 MB)
```

### 3. Deploy & Test

```bash
# Install on emulator/device
./gradlew installDebug

# View logs
adb logcat | grep Oriental

# Test interaction
# - Touch piano keys → play notes
# - Tilt device → pitch bend
# - Rotate device → modulation
# - Tap "UNLOCK" banner → trigger IAP
```

### 4. Build Release APK

```bash
./gradlew assembleRelease
# Output: app/build/outputs/apk/release/app-release.apk
# (Requires signing config in local.properties or gradle)
```

---

## Compilation Flags

Set in `CMakeLists.txt` (Android section) and `app/build.gradle`:

```cmake
-DANDROID=ON                      # Enable Android build
-DORIENTAL_DEMO_ANDROID=1         # Freemium mode (3 inst, 9 presets, 2 fx)
-DCMAKE_BUILD_TYPE=Release        # Optimization
-DANDROID_PLATFORM=android-21     # Min API level
```

Gradle CMake arguments:
```gradle
arguments '-DANDROID=ON',
          '-DORIENTAL_DEMO_ANDROID=ON',
          '-DCMAKE_BUILD_TYPE=Release'
```

---

## Troubleshooting Quick Links

See **[android/README_ANDROID_BUILD.md](android/README_ANDROID_BUILD.md)** → Troubleshooting for:

- "No Java executable found" → Install JDK, set JAVA_HOME
- "Android SDK not found" → Verify sdk.dir in local.properties
- "NDK not found" → Verify ndk.dir, version 26.0+
- "CMake not found" → Install CMake 3.22.1+
- "Gradle sync failed" → Clean .gradle/, invalidate cache
- "C++ compilation error" → Check NDK version, CMake paths
- "Permission denied" → Run as admin (Windows), check PATH (Unix)

---

## Implementation Roadmap

### ✅ Phase 1: Scaffolding (COMPLETE)
- CMakeLists.txt: Android target
- C++ headers & implementations: 8 files
- Android config: manifest + Gradle
- Documentation: comprehensive guides

### ⏳ Phase 2: Java/Kotlin (NEXT)
- **MainActivity.kt**: JUCE activity, lifecycle
- **BillingManager.kt**: Google Play Billing client
- **JNI bridge**: C++ callbacks from Java

### ⏳ Phase 3: Resources
- strings.xml, colors.xml, dimens.xml
- App icons (mdpi–xxxhdpi)
- Adaptive icon, notification icons

### ⏳ Phase 4: Build Verification
- `./gradlew assembleDebug` succeeds
- APK produced, ~60 MB
- Deploy to emulator/device

### ⏳ Phase 5: Testing & Release
- Touch input works (piano notes)
- Sensors work (pitch, mod wheel)
- IAP flow works (unlock full version)
- Sign & publish to Play Store

---

## Key Contacts & Resources

### Documentation
- **JUCE:** https://docs.juce.com/master/tutorial_android_apps.html
- **Google Play Billing:** https://developer.android.com/google-play/billing
- **Android NDK:** https://developer.android.com/ndk
- **CMake Toolchain:** https://developer.android.com/ndk/guides/cmake

### Debugging
- **Logs:** `adb logcat | grep Oriental`
- **Verbose build:** `./gradlew assembleDebug --info`
- **Device SDK:** `adb shell getprop ro.build.version.sdk`

---

## Checklist for Next Session

- [ ] Install JDK 11+ (java -version)
- [ ] Install Android SDK (API 21, 34)
- [ ] Install Android NDK 26.0.10792818
- [ ] Install CMake 3.22.1+
- [ ] Copy local.properties.template → local.properties
- [ ] Edit local.properties (SDK/NDK paths)
- [ ] Run: ./gradlew clean
- [ ] Run: ./gradlew :app:externalNativeBuildDebug --info
  - Watch for CMake compilation
  - Check for C++ errors
- [ ] Run: ./gradlew assembleDebug
  - Should produce APK in app/build/outputs/apk/debug/
- [ ] Deploy: ./gradlew installDebug
- [ ] Test on device/emulator

---

## Summary

**Status:** All scaffolding complete, ready for toolchain setup

**Deliverables:**
- 8 C++ source files (Mobile layer)
- 1 modified CMakeLists.txt (+84 lines)
- 7 Gradle configuration files
- 1 Android manifest
- 3 comprehensive documentation files

**Total:** 19 files (18 new, 1 modified)

**Blockers:** Java/Android toolchain not installed (JDK, SDK, NDK, CMake, Gradle)

**Next Step:** Install toolchain, then run `./gradlew assembleDebug` to verify build

---

**Oriental Instrument v1.0.0** — Maqam Edition for Android
