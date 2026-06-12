#!/bin/bash
# Build styled Oriental Instrument .dmg (macOS only).
#   build_dmg.sh <vst3> <out.dmg> [samples.zip] [password]
# - samples.zip : if given, unzipped into the DMG as the full sample library
# - password    : if given, the final DMG is AES-256 encrypted (bundle/Full)
set -euo pipefail

VST3_SRC="${1:?need path to .vst3 bundle}"
OUT_DMG="${2:-Oriental-Instrument-VST3-macOS.dmg}"
SAMPLES_ZIP="${3:-}"
PASSWORD="${4:-}"
PKG_DIR="$(cd "$(dirname "$0")" && pwd)"
VOLNAME="Oriental Instrument"

STAGE="$(mktemp -d)/dmg"
mkdir -p "$STAGE/.background"

echo "Staging plugin..."
cp -R "$VST3_SRC" "$STAGE/$VOLNAME.vst3"
ln -s "/Library/Audio/Plug-Ins/VST3" "$STAGE/Drag here → VST3"

# Samples (all-in-one)
if [ -n "$SAMPLES_ZIP" ] && [ -f "$SAMPLES_ZIP" ]; then
  echo "Unpacking samples into DMG..."
  TMPS="$(mktemp -d)"
  unzip -q "$SAMPLES_ZIP" -d "$TMPS"
  SRCDIR="$(find "$TMPS" -maxdepth 2 -type d -name 'SAMPLE ORIENTAL INSTRUMENT VST3' | head -1)"
  [ -z "$SRCDIR" ] && SRCDIR="$TMPS"
  cp -R "$SRCDIR" "$STAGE/SAMPLE ORIENTAL INSTRUMENT VST3"
fi

cp "$PKG_DIR/dmg_background.png" "$STAGE/.background/bg.png"

# Volume icon
ICONSET="$(mktemp -d)/icon.iconset"; mkdir -p "$ICONSET"
for s in 16 32 64 128 256 512; do
  sips -z $s $s "$PKG_DIR/volume_icon.png" --out "$ICONSET/icon_${s}x${s}.png" >/dev/null
  d=$((s*2)); sips -z $d $d "$PKG_DIR/volume_icon.png" --out "$ICONSET/icon_${s}x${s}@2x.png" >/dev/null
done
iconutil -c icns "$ICONSET" -o "$STAGE/.VolumeIcon.icns"
SetFile -a C "$STAGE" 2>/dev/null || true

cat > "$STAGE/INSTALLATION.txt" << 'EOF'
ORIENTAL INSTRUMENT VST3 — macOS

1. Drag "Oriental Instrument.vst3" onto "Drag here → VST3".
2. Drag the "SAMPLE ORIENTAL INSTRUMENT VST3" folder into:
       ~/Documents/
3. Gatekeeper block? Run in Terminal:
   xattr -dr com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/Oriental Instrument.vst3"
4. Restart your DAW, rescan VST3.

www.djbilboxbeats.com — Dedicated to Mourad Attalah & Yamina Chikhr
EOF

# Writable DMG + Finder layout
RW_DMG="$(mktemp -d)/rw.dmg"
SIZE_MB=$(( $(du -sm "$STAGE" | cut -f1) + 60 ))
hdiutil create -srcfolder "$STAGE" -volname "$VOLNAME" -fs HFS+ -format UDRW -size ${SIZE_MB}m "$RW_DMG"
hdiutil attach "$RW_DMG" -nobrowse
sleep 2
osascript << EOF || true
tell application "Finder"
  tell disk "$VOLNAME"
    open
    set current view of container window to icon view
    set toolbar visible of container window to false
    set statusbar visible of container window to false
    set the bounds of container window to {180, 100, 820, 540}
    set vopts to the icon view options of container window
    set arrangement of vopts to not arranged
    set icon size of vopts to 92
    set background picture of vopts to file ".background:bg.png"
    set position of item "$VOLNAME.vst3" of container window to {150, 200}
    set position of item "Drag here → VST3" of container window to {470, 200}
    set position of item "SAMPLE ORIENTAL INSTRUMENT VST3" of container window to {150, 340}
    set position of item "INSTALLATION.txt" of container window to {470, 340}
    update without registering applications
    delay 1
    close
  end tell
end tell
EOF
sync
hdiutil detach "/Volumes/$VOLNAME"

# Compress; encrypt if password supplied
rm -f "$OUT_DMG"
if [ -n "$PASSWORD" ]; then
  echo "Encrypting (AES-256)..."
  printf '%s' "$PASSWORD" | hdiutil convert "$RW_DMG" -format UDZO -imagekey zlib-level=9 \
    -encryption AES-256 -stdinpass -o "$OUT_DMG"
else
  hdiutil convert "$RW_DMG" -format UDZO -imagekey zlib-level=9 -o "$OUT_DMG"
fi
echo "✅ DMG: $OUT_DMG"
