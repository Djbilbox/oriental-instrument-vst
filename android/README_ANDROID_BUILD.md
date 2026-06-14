# Oriental Instrument - Android Standalone Build

This directory contains the Android app configuration and build scripts for Oriental Instrument as a standalone mobile application.

## Overview

- **Package:** com.djbilbox.orientalinstrument
- **Target API:** 21 (Android 5.0) → 34 (Android 14)
- **NDK Version:** 26.0.10792818 (LTS)
- **Audio Engine:** OpenSLES (built via CMake + C++)
- **UI Framework:** JUCE (responsive for mobile screens)
- **Freemium Model:** DEMO mode with in-app purchase unlock

### Freemium Configuration (ORIENTAL_DEMO_ANDROID)

The free DEMO version includes:
- **3 instruments:** Violin, Oud, Ney (out of 7)
- **3 presets per instrument:** 9 total (out of 70)
- **2 effects:** Reverb, Delay (out of 8)

Unlock the full version via in-app purchase (Google Play Billing):
- 7 instruments, 70 presets, 8 effects
- No ads or time limits

## Prerequisites

Before building, ensure you have installed:

1. **Java Development Kit (JDK) 11+**
   ```
   java -version
   ```
   Download: https://www.oracle.com/java/technologies/downloads/

2. **Android SDK (API 34)**
   - Install via Android Studio or command-line tools
   - Minimum components:
     - SDK Platform API 34
     - SDK Platform API 21 (min API level)
     - Android SDK Build-Tools 34.x
     - CMake 3.22.1

3. **Android NDK 26.0.10792818 (LTS)**
   - Install from https://developer.android.com/ndk/
   - Or install via Android Studio: Tools → SDK Manager → SDK Tools → NDK

4. **Gradle 8.1+**
   - Bundled in Android Studio
   - Or install standalone from https://gradle.org/releases/

5. **CMake 3.22.1+**
   - For native C++ build
   - Install via package manager or https://cmake.org/

## Setup

### Step 1: Configure Environment

Copy and edit `local.properties.template` → `local.properties`:

```bash
cd android
cp local.properties.template local.properties
```

Edit `local.properties` with your paths:

```properties
sdk.dir=/path/to/Android/sdk
ndk.dir=/path/to/Android/ndk/26.0.10792818
```

**On Windows:**
```properties
sdk.dir=C:\\Users\\<Username>\\AppData\\Local\\Android\\sdk
ndk.dir=C:\\Users\\<Username>\\AppData\\Local\\Android\\sdk\\ndk\\26.0.10792818
```

**On macOS:**
```properties
sdk.dir=$HOME/Library/Android/sdk
ndk.dir=$HOME/Library/Android/sdk/ndk/26.0.10792818
```

**On Linux:**
```properties
sdk.dir=$HOME/Android/sdk
ndk.dir=$HOME/Android/sdk/ndk/26.0.10792818
```

### Step 2: Verify CMake Configuration

The Android build uses CMake (configured in `app/build.gradle`):

```gradle
externalNativeBuild {
    cmake {
        version '3.22.1'
        path '../../CMakeLists.txt'
    }
}
```

The root `CMakeLists.txt` (in project root, not `android/`) includes the Android target:

```cmake
if(ANDROID)
    add_executable(OrientalInstrumentAndroid ...)
    target_compile_definitions(... ORIENTAL_DEMO_ANDROID=1 ...)
    target_link_libraries(... OpenSLES android log ...)
endif()
```

## Building

### Build Debug APK

```bash
cd android
./gradlew assembleDebug
```

Output: `app/build/outputs/apk/debug/app-debug.apk`

### Build Release APK

```bash
cd android
./gradlew assembleRelease
```

Requires signing configuration in `local.properties` or `app/build.gradle`.

Output: `app/build/outputs/apk/release/app-release.apk`

### Build and Install to Emulator/Device

```bash
cd android
./gradlew installDebug
```

Requires:
- Android emulator running (e.g., Pixel 4a with API 30)
- Or physical device with USB debugging enabled

### View Build Logs

```bash
./gradlew assembleDebug --info
```

## Key Files

### C++ Source (in parent `Source/` directory)

- `Mobile/AndroidStandalone.cpp` — JUCE app entry point
- `Mobile/AndroidActivity.h/cpp` — Touch + sensor input handling
- `Mobile/UI/AndroidUIAdapter.h/cpp` — Responsive UI wrapper
- `Mobile/IAP/AndroidIAPManager.h/cpp` — Google Play Billing integration

### Android Configuration

- `AndroidManifest.xml` — App permissions, activities, services
- `app/build.gradle` — Gradle configuration, NDK/CMake setup, dependencies
- `build.gradle` — Root-level Gradle settings
- `settings.gradle` — Gradle project structure
- `gradle.properties` — Gradle build options
- `local.properties` — Developer environment (git-ignored)

## Responsive Design

The Android UI adapts to screen sizes via `AndroidUIAdapter`:

- **Small phones (<480dp width):** 75% scale, hide FX panel
- **Normal phones (<600dp):** 85% scale, all UI visible
- **Large phones (<800dp):** 95% scale
- **Tablets:** 100% scale

Portrait orientation is enforced in `AndroidManifest.xml`:

```xml
<activity
    android:name=".MainActivity"
    android:screenOrientation="portrait"
    .../>
```

## Audio Configuration

- **OpenSLES:** Fast, low-latency audio output (native Android)
- **Sampling Rate:** 48 kHz (Android default)
- **Buffer Size:** Adaptive (typically 512–1024 samples)
- **Channels:** Stereo out, mono in (if recording in future)

## Sensors (Optional)

If touch + sensor input is enabled in `AndroidActivity`:

- **Accelerometer (3-axis):** Maps device tilt → pitch bend
- **Gyroscope (3-axis):** Maps device rotation → modulation wheel

Permission: `android.permission.ACCESS_FINE_LOCATION` (optional, for sensor calibration)

## In-App Purchase (Freemium)

`AndroidIAPManager` wraps Google Play Billing Library:

- **Product ID:** `com.djbilbox.orientalinstrument.full`
- **Unlocks:** All 7 instruments, 70 presets, 8 effects
- **Pricing:** Set in Google Play Console

The banner at the bottom of the UI prompts users to unlock (in DEMO mode only).

### Testing IAP Locally

1. Create a test billing account in Google Play Console
2. Add test accounts under **Settings → License Testing**
3. Install APK on test device
4. Test account will see "Test Billing" in purchase dialog

## Troubleshooting

### "No Java executable found"
Ensure JDK is installed and `JAVA_HOME` is set:
```bash
echo $JAVA_HOME  # or %JAVA_HOME% on Windows
java -version
```

### "Android SDK not found"
Verify `sdk.dir` in `local.properties` points to valid SDK:
```bash
ls $sdk.dir/platforms/android-34  # macOS/Linux
dir %sdk.dir%\platforms\android-34  # Windows
```

### "NDK not found"
Verify `ndk.dir` in `local.properties`:
```bash
ls $ndk.dir/toolchains/llvm  # macOS/Linux
dir %ndk.dir%\toolchains\llvm  # Windows
```

### "CMake not found"
Install CMake 3.22.1+:
```bash
# macOS
brew install cmake

# Linux
sudo apt-get install cmake

# Windows: Download from https://cmake.org/
```

### "Gradle sync failed"
1. Run `./gradlew clean`
2. File → Invalidate Caches → Restart (in Android Studio)
3. Ensure Gradle version in `build.gradle` matches installed version

### C++ Compilation Errors

Check NDK version compatibility. The build uses:
- **C++17 standard** (`android.cmake.cppFlags=-std=c++17`)
- **OpenSLES** (native Android audio)
- **JUCE audio modules**

If compilation fails, check:
1. NDK version matches `ndkVersion` in `app/build.gradle`
2. API level >= 21 in `defaultConfig`
3. CMake flags in `app/build.gradle` match parent `CMakeLists.txt`

## CI/CD Integration

For automated builds (e.g., GitHub Actions):

1. Set secrets in CI environment:
   - `ANDROID_SDK_ROOT` → path to Android SDK
   - `ANDROID_NDK_HOME` → path to NDK
   - `JAVA_HOME` → path to JDK

2. Example GitHub Actions workflow:
   ```yaml
   - name: Build Oriental Instrument Android
     run: |
       cd android
       ./gradlew assembleRelease
   ```

3. Upload APK to Google Play Console for testing/release.

## Next Steps

1. **Implement Java Activity:** Create `app/src/main/java/com/djbilbox/orientalinstrument/MainActivity.kt`
   - Initialize JUCE app
   - Handle lifecycle callbacks
   - Set up sensor listeners

2. **Implement Google Play Billing:** Create `BillingManager.kt`
   - Connect to Google Play Billing Library
   - Handle purchase updates
   - Sync with C++ `AndroidIAPManager` via JNI

3. **Test on Device:** Deploy APK to Android device (API 21–34)
   - Verify audio output
   - Test touch input
   - Confirm IAP flow

4. **Publish to Play Store:** Submit signed APK to Google Play Console

## References

- [JUCE Android Documentation](https://docs.juce.com/master/tutorial_android_apps.html)
- [Google Play Billing Library](https://developer.android.com/google-play/billing)
- [Android NDK Documentation](https://developer.android.com/ndk)
- [CMake Android Toolchain](https://developer.android.com/ndk/guides/cmake)

## Support

For issues:
1. Check Android Studio Logcat for runtime errors
2. Review CMake/NDK build output: `./gradlew assembleDebug --info`
3. Verify device API level: `adb shell getprop ro.build.version.sdk`

---

**Oriental Instrument v1.0.0** — Maqam Edition for Android
