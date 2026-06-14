#!/bin/bash
# Build Oriental Instrument for Android

set -e

BUILD_TYPE="${1:-Release}"
ABI="${2:-arm64-v8a}"  # arm64-v8a, armeabi-v7a, x86, x86_64
ANDROID_API="${3:-30}"
NDK_VERSION="r24"

echo "🤖 Building Oriental Instrument for Android..."
echo "  Build Type: $BUILD_TYPE"
echo "  ABI: $ABI"
echo "  API Level: $ANDROID_API"
echo "  NDK Version: $NDK_VERSION"

# Check for ANDROID_NDK environment
if [ -z "$ANDROID_NDK" ]; then
  echo "❌ ANDROID_NDK not set. Set to your NDK installation path."
  exit 1
fi

# Create build directory
BUILD_DIR="build-android-${ABI}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for Android
cmake -G Ninja \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_SYSTEM_VERSION=$ANDROID_API \
  -DCMAKE_ANDROID_ARCH_ABI=$ABI \
  -DCMAKE_ANDROID_NDK=$ANDROID_NDK \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DJUCE_BUILD_EXTRAS=OFF \
  -DJUCE_BUILD_EXAMPLES=OFF \
  ..

# Build
cmake --build . --config $BUILD_TYPE --target OrientalInstrumentAndroid

echo "✅ Android build complete!"
echo "   APK: build-android-${ABI}/OrientalInstrument-${BUILD_TYPE}.apk"
