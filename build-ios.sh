#!/bin/bash
# Build Oriental Instrument for iOS

set -e

BUILD_TYPE="${1:-Release}"
ARCH="${2:-arm64}"  # arm64 or x86_64 (simulator)
DEPLOYMENT_TARGET="13.0"

echo "🍎 Building Oriental Instrument for iOS..."
echo "  Build Type: $BUILD_TYPE"
echo "  Architecture: $ARCH"
echo "  Deployment Target: $DEPLOYMENT_TARGET"

# Create build directory
BUILD_DIR="build-ios-${ARCH}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for iOS
cmake -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=$ARCH \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=$DEPLOYMENT_TARGET \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DJUCE_BUILD_EXTRAS=OFF \
  -DJUCE_BUILD_EXAMPLES=OFF \
  ..

# Build
cmake --build . --config $BUILD_TYPE --target OrientalInstrumentiOS

echo "✅ iOS build complete!"
echo "   Product: build-ios-${ARCH}/Oriental\ Instrument.app"
