# Mobile UI Integration Guide

Guide to assembling Oriental Instrument mobile UI from reusable components.

## Architecture

```
iOSMainComponent / AndroidMainComponent (root)
  ├── MobileAudioContext (audio engine)
  ├── MobileUIController (layout manager)
  │   ├── MobileKeyboard (bottom: 2-octave quarter-tone keyboard)
  │   ├── MobileKnob × 6 (macro controls: Glide, Space, Filter, Orient, Reso, Depth)
  │   ├── MobilePresetBrowser (left: instrument tabs + preset list)
  │   ├── FreemiumIndicator (watermark + purchase overlay)
  │   └── BackgroundComponent (background image + animations, from desktop)
  └── iOSIAPManager / AndroidIAPManager (in-app purchases)
```

## Component Responsibilities

### MobileAudioContext
- Wraps PluginProcessor for standalone audio
- Manages audio device + MIDI input
- Routes audio to device speaker

### MobileUIController
- Main layout engine (portrait/landscape)
- Routes touch events to sub-components
- Updates UI on preset/parameter changes

### MobileKeyboard
- Renders 2-octave quarter-tone keyboard
- Detects note on/off from touch
- Vertical drag = pitch-bend (-1 to +1 octave)

### MobileKnob (× 6 macro controls)
- `Glide`: Portamento time (0-100)
- `Space`: Reverb tail + release (0-100)
- `Filter`: SVF cutoff (0-100)
- `Orient`: Vibrato + detune (0-100)
- `Reso`: Resonance peak Q (0-100)
- `Depth`: Saturation intensity (0-100)

### MobilePresetBrowser
- Instrument tabs (7 total, 3 in demo)
- Scrollable preset list (10 per inst, 3 in demo)
- Shows lock icon for unavailable presets

### FreemiumIndicator
- Red "DEMO" watermark (pulsing)
- Tap to unlock: launches IAP flow
- Hidden once pro unlocked

## Integration Steps

### 1. MobileUIController Layout

```cpp
// Portrait layout (default)
void MobileUIController::layoutPortrait(int w, int h)
{
    presetArea = { 0, 0, w*0.25f, h*0.5f };      // Left column: presets
    controlsArea = { w*0.25f, 0, w*0.75f, h*0.5f }; // Knobs grid
    keyboardArea = { 0, h*0.5f, w, h*0.5f };     // Bottom: keyboard
}

// Landscape layout (wide screen)
void MobileUIController::layoutLandscape(int w, int h)
{
    presetArea = { 0, 0, w*0.2f, h };            // Left column
    controlsArea = { w*0.2f, 0, w*0.5f, h };     // Middle: knobs
    keyboardArea = { w*0.7f, 0, w*0.3f, h };     // Right: keyboard
}
```

### 2. Create & Attach Sub-Components

```cpp
// In iOSMainComponent constructor or resized()
keyboard = std::make_unique<MobileKeyboard>(processor);
addAndMakeVisible(*keyboard);
keyboard->setBounds(keyboardArea);

presetBrowser = std::make_unique<MobilePresetBrowser>(processor, pm);
addAndMakeVisible(*presetBrowser);
presetBrowser->setBounds(presetArea);

// Macro knobs (6 total)
knobGlide = std::make_unique<MobileKnob>("Glide");
knobGlide->setValueChangedCallback([this](float v) {
    processor.getAPVTS().getParameter("glide")->setValue(v);
});
addAndMakeVisible(*knobGlide);
```

### 3. Parameter Binding

```cpp
// Link knobs to processor parameters
auto* glideParam = processor.getAPVTS().getParameter("glide");
glideParam->addListener(this);  // Observe changes

// When preset changes, update all knob displays
void MyComponent::parameterChanged(const juce::String& paramID, float value)
{
    if (paramID == "glide")
        knobGlide->setValue(value);
    // ... etc for other knobs
}
```

### 4. IAP Integration

```cpp
// In iOSMainComponent
iapManager = std::make_unique<iOSIAPManager>();
iapManager->initialise();

// Set preset manager to demo mode if not pro
bool isProUnlocked = iapManager->isProUnlocked();
processor.getPresetManager().setProUnlocked(isProUnlocked);

// Wire freemium indicator
freemiumIndicator = std::make_unique<FreemiumIndicator>(iapManager.get());
addAndMakeVisible(*freemiumIndicator);
freemiumIndicator->toFront(false);

// Detect locked preset access
if (!presetManager.isPresetAvailable(selectedIndex) && !isProUnlocked)
{
    freemiumIndicator->showPurchasePrompt();
}
```

### 5. Touch Routing

```cpp
// MobileUIController delegates touch to sub-components
void MobileUIController::mouseDown(const juce::MouseEvent& e)
{
    auto pos = e.getPosition();

    // Check hit areas and route to correct component
    if (keyboardArea.contains(pos))
        keyboard->mouseDown(e);
    else if (presetArea.contains(pos))
        presetBrowser->mouseDown(e);
    else if (controlsArea.contains(pos))
        routeToKnob(e);  // Determine which knob
}
```

## Portrait Mode Layout (1200×800 example)

```
┌─────────────────────────────────────────────┐
│  Preset Selector (300×400)                  │ Knob Controls (900×400)
│  ┌──────────────────┐                       │ ┌────┐ ┌────┐ ┌────┐
│  │ Violin           │ (selected)            │ │Glid│ │Spac│ │Filt│
│  │ Oud              │                       │ │e   │ │e   │ │er  │
│  │ Ney   ✓          │ demo only            │ │    │ │    │ │    │
│  │ Qanun ✗          │ pro only             │ └────┘ └────┘ └────┘
│  │ Darbuka ✗        │                       │ ┌────┐ ┌────┐ ┌────┐
│  │ Rababah ✗        │                       │ │Orie│ │Reso│ │Dept│
│  │ Mizmar ✗         │                       │ │ent │ │    │ │h   │
│  └──────────────────┘ Preset 1              │ │    │ │    │ │    │
│                       Preset 2              │ └────┘ └────┘ └────┘
│  ┌──────────────────┐ Preset 3 ✓ (demo)    │
│  │ 1. Cairo Dance   │ Preset 4 ✗ (pro)     │  DEMO  (watermark)
│  │ 2. Desert Wind   │ Preset 5 ✗ (pro)     │ tap to unlock
│  │ 3. Medina...     │                       │
│  └──────────────────┘                       │
├─────────────────────────────────────────────┤
│ Keyboard: 48 quarter-tone keys (2 octaves) │
│ ┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐      │
│ │C │C#│Db│D │D#│Eb│E │ ... (24 keys)  │  │
│ │  │  │  │  │  │  │  │ ... (24 keys)  │  │
│ └──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘      │
│ Vertical drag on keyboard = pitch-bend     │
└─────────────────────────────────────────────┘
```

## Landscape Mode Layout (1200×600 example)

```
┌────────┬──────────────────┬───────────┐
│Presets │    Knobs (6)     │ Keyboard  │
│┌──────┐│ ┌────┬────┬────┐ │  (1-oct)  │
││Violin││ │Glid│Spac│Filt│ │ ┌──┬──┐  │
││Oud   ││ │    │    │    │ │ │C │D │  │
││Ney ✓ ││ └────┴────┴────┘ │ │  │  │  │
││Qanun ││ ┌────┬────┬────┐ │ │E │F │  │
││...   ││ │Orie│Reso│Dept│ │ │  │  │  │
││      ││ │    │    │    │ │ │G │A │  │
│└──────┘│ └────┴────┴────┘ │ │  │  │  │
│        │                  │ │B │C'│  │
│Preset  │ DEMO (watermark) │ └──┴──┘  │
│list    │ tap to unlock    │ Vertical │
│(scroll)│                  │ drag for │
│        │                  │ pitch    │
└────────┴──────────────────┴───────────┘
```

## Data Flow

```
AudioEngine (PluginProcessor)
    ↓
     ← Parameter changes (APVTS)
    ↑
PresetManager ← UI events (knob, keyboard, preset select)
    ↑
    MobileUIController (orchestrates all UI components)
```

## Testing Checklist

- [ ] Keyboard notes play correctly
- [ ] Pitch-bend drag works (vertical)
- [ ] Knob drag adjusts parameters
- [ ] Preset selection updates audio
- [ ] Freemium watermark visible in demo
- [ ] Locked presets show availability hint
- [ ] Purchase prompt works
- [ ] Portrait ↔ landscape rotation smooth
- [ ] No audio glitches during touch
- [ ] Responsive to slow/fast swipes

## Common Issues & Fixes

**Keyboard notes sticking on orientation change**
→ Call `noteOff()` for all notes in `resized()`

**Knob value jumps when touching**
→ Store last touch position, use delta (not absolute)

**Preset list doesn't scroll**
→ Ensure `ScrollBar` bounds are set correctly

**Audio cuts out in background**
→ Configure audio session: `JUCE_FORCE_USE_CUSTOM_AUDIO_DEVICE=1`

**IAP purchase always succeeds in testing**
→ Expected; real implementation requires App Store / Play Store sandbox
