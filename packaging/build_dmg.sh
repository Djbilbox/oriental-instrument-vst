#!/bin/bash
# Build styled Oriental Instrument .dmg (macOS only). Run from repo root or CI.
#   ./packaging/build_dmg.sh "<path-to-Oriental Instrument.vst3>" "<output.dmg>"
# Samples are NOT bundled (190 MB, shipped separately). DMG carries plugin +
# branded background + drag-to-install layout + instructions.
set -euo pipefail

VST3_SRC="${1:?need path to .vst3 bundle}"
OUT_DMG="${2:-Oriental-Instrument-VST3-macOS.dmg}"
PKG_DIR="$(cd "$(dirname "$0")" && pwd)"
VOLNAME="Oriental Instrument"

STAGE="$(mktemp -d)/dmg"
mkdir -p "$STAGE/.background"

echo "Staging plugin..."
cp -R "$VST3_SRC" "$STAGE/$VOLNAME.vst3"

# Drag target: symlink to the system VST3 folder
ln -s "/Library/Audio/Plug-Ins/VST3" "$STAGE/Drag here → VST3"

# Background image
cp "$PKG_DIR/dmg_background.png" "$STAGE/.background/bg.png"

# Volume icon (.icns from png)
ICONSET="$(mktemp -d)/icon.iconset"
mkdir -p "$ICONSET"
for s in 16 32 64 128 256 512; do
  sips -z $s $s "$PKG_DIR/volume_icon.png" --out "$ICONSET/icon_${s}x${s}.png" >/dev/null
  d=$((s*2))
  sips -z $d $d "$PKG_DIR/volume_icon.png" --out "$ICONSET/icon_${s}x${s}@2x.png" >/dev/null
done
iconutil -c icns "$ICONSET" -o "$STAGE/.VolumeIcon.icns"
SetFile -a C "$STAGE" 2>/dev/null || true

# Instructions
cat > "$STAGE/INSTALLATION.txt" << 'EOF'
ORIENTAL INSTRUMENT VST3 — macOS

1. Drag "Oriental Instrument.vst3" onto the "Drag here → VST3" shortcut
   (installs to /Library/Audio/Plug-Ins/VST3/).

2. SAMPLES (separate download — "SAMPLE ORIENTAL INSTRUMENT VST3.zip"):
   Unzip into  ~/Documents/SAMPLE ORIENTAL INSTRUMENT VST3/

3. If Gatekeeper blocks it:
   xattr -dr com.apple.quarantine "/Library/Audio/Plug-Ins/VST3/Oriental Instrument.vst3"

4. Restart your DAW, rescan VST3.

www.djbilboxbeats.com — Dedicated to Mourad Attalah & Yamina Chikhr
EOF

# Build writable DMG, lay out window, compress
RW_DMG="$(mktemp -d)/rw.dmg"
SIZE_MB=$(( $(du -sm "$STAGE" | cut -f1) + 40 ))
hdiutil create -srcfolder "$STAGE" -volname "$VOLNAME" -fs HFS+ \
  -format UDRW -size ${SIZE_MB}m "$RW_DMG"

MOUNT_DIR="/Volumes/$VOLNAME"
hdiutil attach "$RW_DMG" -nobrowse
sleep 2

osascript << EOF || true
tell application "Finder"
  tell disk "$VOLNAME"
    open
    set current view of container window to icon view
    set toolbar visible of container window to false
    set statusbar visible of container window to false
    set the bounds of container window to {200, 120, 800, 520}
    set vopts to the icon view options of container window
    set arrangement of vopts to not arranged
    set icon size of vopts to 96
    set background picture of vopts to file ".background:bg.png"
    set position of item "$VOLNAME.vst3" of container window to {150, 250}
    set position of item "Drag here → VST3" of container window to {450, 250}
    set position of item "INSTALLATION.txt" of container window to {300, 360}
    update without registering applications
    delay 1
    close
  end tell
end tell
EOF

sync
hdiutil detach "$MOUNT_DIR"
rm -f "$OUT_DMG"
hdiutil convert "$RW_DMG" -format UDZO -imagekey zlib-level=9 -o "$OUT_DMG"
echo "✅ DMG: $OUT_DMG"
