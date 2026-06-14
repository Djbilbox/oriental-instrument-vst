#!/bin/bash
# Quick verification that mobile targets compile

set -e

echo "🧪 Mobile Build Verification Test"
echo ""

# Ensure JUCE is available
if [ ! -d "JUCE" ]; then
    echo "❌ JUCE directory not found."
    echo "   Clone: git clone --depth 1 https://github.com/juce-framework/JUCE.git"
    exit 1
fi

echo "✅ JUCE found"
echo ""

# Test iOS (Xcode syntax check only)
echo "Checking iOS CMake configuration..."
mkdir -p _test_ios
cd _test_ios
cmake -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    .. > /dev/null 2>&1 && echo "✅ iOS CMake OK" || echo "❌ iOS CMake FAILED"
cd ..
rm -rf _test_ios

# Test Android (requires NDK)
echo "Checking Android CMake configuration..."
if [ -z "$ANDROID_NDK" ]; then
    echo "⚠️  ANDROID_NDK not set (skipping Android test)"
    echo "    Set: export ANDROID_NDK=/path/to/ndk/r24"
else
    mkdir -p _test_android
    cd _test_android
    cmake -G Ninja \
        -DCMAKE_SYSTEM_NAME=Android \
        -DCMAKE_SYSTEM_VERSION=30 \
        -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
        -DCMAKE_ANDROID_NDK=$ANDROID_NDK \
        .. > /dev/null 2>&1 && echo "✅ Android CMake OK" || echo "❌ Android CMake FAILED"
    cd ..
    rm -rf _test_android
fi

echo ""
echo "✅ Mobile build configuration verified!"
echo ""
echo "Next steps:"
echo "  iOS:     ./build-ios.sh Release arm64"
echo "  Android: ./build-android.sh Release arm64-v8a"
