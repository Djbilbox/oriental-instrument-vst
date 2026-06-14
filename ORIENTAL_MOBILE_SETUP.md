# Oriental Instrument Mobile — Setup & Build Guide

**Project**: Oriental Instrument iOS + Android  
**Status**: Ready to build  
**Date**: June 14, 2026

---

## 🚀 Quick Start

### Prerequisites Installation

#### Windows / Mac / Linux

**1. Git** (already have)
```bash
git --version  # Verify
```

**2. CMake 3.22+**
- Download: https://cmake.org/download/
- Windows: Add to PATH

**3. Clang/LLVM**
- Windows: https://releases.llvm.org/ (v14+)
- Mac: `xcode-select --install`
- Linux: `sudo apt install clang`

---

## 📱 iOS Setup (macOS Only)

### Step 1: Install Xcode 14+
```bash
# App Store → Xcode
# Or: https://developer.apple.com/download/

xcode-select --install
xcodebuild -version  # Verify
```

### Step 2: Configure iOS Build
```bash
cd "C:\Users\djbil\Desktop\ORIENTAL VST\DJBILBOX VST"

# Create build directory
mkdir build-ios
cd build-ios

# Generate Xcode project
cmake -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
  ..

# Build (Release)
cmake --build . --config Release

# Build (Demo/Freemium)
cmake --build . --config Release --target OrientalInstrumentiOS_Demo
```

### Step 3: Run on Simulator
```bash
# Launch iOS simulator
open -a Simulator

# Install app
xcrun simctl install booted build-ios/Release/Oriental\ Instrument.app

# Launch
xcrun simctl launch booted com.djbilbox.orientalinstrument.ios
```

### Step 4: Submit to App Store
1. Apple Developer Account (required)
2. App Store Connect → Create app
3. Bundle ID: `com.djbilbox.orientalinstrument.ios`
4. Sign in Xcode → Archive → Distribute
5. Submit for review

**Docs**: See `iOS-BUILD.md` in project root

---

## 🤖 Android Setup (Windows/Mac/Linux)

### Step 1: Install Java 11+
```bash
# Windows:
# https://www.oracle.com/java/technologies/javase/jdk11-archive-downloads.html

java -version  # Verify (should be 11+)
```

### Step 2: Install Android SDK
```bash
# Windows: https://developer.android.com/studio
# Or direct SDK: https://developer.android.com/tools/releases/sdk-tools

# Add to environment:
# ANDROID_SDK_ROOT = C:\Android\sdk  (Windows)
# ANDROID_SDK_ROOT = ~/Library/Android/sdk  (Mac)
```

### Step 3: Install Android NDK 26
```bash
# Via Android Studio:
# Settings → SDK Manager → SDK Tools → NDK (r26 LTS)

# Or manual:
# https://developer.android.com/ndk/downloads

# Add to environment:
# ANDROID_NDK = /path/to/ndk/26.0.10792818

# Verify:
echo $ANDROID_NDK  # or echo %ANDROID_NDK% (Windows)
```

### Step 4: Configure Local Properties
```bash
cd "C:\Users\djbil\Desktop\ORIENTAL VST\DJBILBOX VST"

# Copy template
cp android/local.properties.template android/local.properties

# Edit android/local.properties:
# sdk.dir=C:\\Android\\sdk
# ndk.dir=C:\\Android\\ndk\\26.0.10792818
# cmake.dir=C:\\Android\\cmake\\3.22.1
```

### Step 5: Build Android App
```bash
cd android

# Debug build
./gradlew assembleDebug

# Release build
./gradlew assembleRelease

# Demo version
./gradlew assembleDebug -PORIENTAL_DEMO=true

# Output: app/build/outputs/apk/debug/app-debug.apk
```

### Step 6: Install on Device/Emulator
```bash
# Start emulator
emulator -avd Pixel_6_API_30

# Install APK
adb install app/build/outputs/apk/debug/app-debug.apk

# Launch
adb shell am start -n com.djbilbox.orientalinstrument.android/.MainActivity

# Logs
adb logcat | grep Oriental
```

### Step 7: Submit to Google Play
1. Google Play Developer account (required, $25 one-time)
2. Google Play Console → Create app
3. Bundle ID: `com.djbilbox.orientalinstrument.android`
4. Sign release APK (keystore)
5. Upload to internal testing track
6. Submit for review

**Docs**: See `README_ANDROID_BUILD.md` in `android/` folder

---

## 🏗️ Project Structure

```
ORIENTAL VST\DJBILBOX VST\
├── Source/
│   ├── DSP/                    ← Shared synth (unchanged)
│   ├── GUI/                    ← Desktop UI (VST)
│   └── Mobile/                 ← NEW: Mobile code
│       ├── iOS/                ← iOS-specific
│       ├── Android/            ← Android-specific
│       ├── IAP/                ← In-app purchases
│       └── UI/                 ← Touch components
│
├── CMakeLists.txt              ← Updated for iOS/Android
├── android/                    ← NEW: Gradle build system
│   ├── build.gradle
│   ├── app/build.gradle
│   ├── local.properties.template
│   └── README_ANDROID_BUILD.md
│
├── build-ios/                  ← Create for iOS build
├── build-android-arm64-v8a/    ← Create for Android build
│
├── MOBILE_BUILD.md             ← Prerequisites + setup
├── MOBILE_CHECKLIST.md         ← 66-item pre-launch
├── PROJECT_SUMMARY.md          ← This session overview
├── iOS-BUILD.md                ← iOS detailed guide
└── ANDROID_BUILD_INDEX.md      ← Android quick ref
```

---

## ✅ Verification Checklist

### Before Building

- [ ] CMake 3.22+ installed
- [ ] JUCE directory exists: `JUCE/` (or set `-DJUCE_DIR=...`)
- [ ] Git repo clean: `git status`

### iOS Only

- [ ] Xcode 14+ installed
- [ ] `xcode-select --install` done
- [ ] Deployment target: iOS 14.0+

### Android Only

- [ ] Java 11+ installed: `java -version`
- [ ] Android SDK installed (API 21, 30, 34)
- [ ] NDK 26 installed: `echo $ANDROID_NDK`
- [ ] `android/local.properties` configured
- [ ] Gradle 8.1+ (comes with Android Studio)

---

## 🐛 Troubleshooting

### CMake: JUCE not found
```bash
# Solution: Clone JUCE in project root
git clone --depth 1 https://github.com/juce-framework/JUCE.git
```

### iOS: Code signing error
```bash
# In Xcode:
1. Select target → Signing & Capabilities
2. Team: Your Apple ID / Developer account
3. Bundle ID: com.djbilbox.orientalinstrument.ios
```

### Android: NDK not found
```bash
# Set environment variable:
export ANDROID_NDK=/path/to/ndk/26.0.10792818  # Mac/Linux
set ANDROID_NDK=C:\Android\ndk\26.0.10792818   # Windows
```

### Android: Gradle sync fails
```bash
# Clear Gradle cache
cd android
./gradlew clean

# Re-sync
./gradlew assembleDebug
```

### Audio not working in simulator
```bash
# iOS simulator: Audio routing is limited; test on device
# Android emulator: Enable audio output in AVD settings
```

---

## 📚 Documentation Files

**Quick Reference**:
- `iOS-QUICK-START.md` — 30 seconds
- `ANDROID_BUILD_INDEX.md` — 30 seconds

**Complete Guides**:
- `iOS-BUILD.md` — Full iOS walkthrough
- `README_ANDROID_BUILD.md` — Full Android walkthrough

**Architecture**:
- `INTEGRATION_GUIDE.md` — Component assembly
- `PROJECT_SUMMARY.md` — Session overview

**Pre-Launch**:
- `MOBILE_CHECKLIST.md` — 66-item checklist
- `MOBILE_BUILD.md` — Prerequisites

---

## 🎯 Next Steps

1. **Install toolchains** (Xcode, NDK, SDK)
2. **Configure local.properties** (Android)
3. **Build & test**:
   ```bash
   # iOS
   ./build-ios.sh Release arm64
   
   # Android
   ./build-android.sh Release arm64-v8a
   ```
4. **Run on device/simulator**
5. **Test IAP** (TestFlight, Play Store internal)
6. **Submit to stores**

---

## 💬 Support

**Issues**: Check docs in project root  
**Build errors**: See TROUBLESHOOTING section above  
**Code questions**: See `INTEGRATION_GUIDE.md`  

---

**Ready to build!** Follow steps above for your platform.
