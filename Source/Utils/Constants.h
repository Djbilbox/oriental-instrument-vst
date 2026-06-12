#pragma once

namespace OrientalConstants
{
    // Plugin info
    constexpr const char* PLUGIN_NAME = "Oriental Instrument";
    constexpr const char* PLUGIN_MANUFACTURER = "DJBILBOX BEATS";
    constexpr const char* PLUGIN_VERSION = "1.0.0";

    // Audio
    constexpr int MAX_VOICES = 16;
    constexpr int NUM_INSTRUMENTS = 7;
    constexpr int PRESETS_PER_INSTRUMENT = 10;
    constexpr int TOTAL_PRESETS = 70;
    constexpr int WAVETABLE_SIZE = 2048;
    constexpr float DEFAULT_TUNING = 440.0f;

    // GUI dimensions — compact (smaller, BUJAA-style footprint).
    constexpr int WINDOW_WIDTH = 860;
    constexpr int WINDOW_HEIGHT = 540;
    constexpr int HEADER_HEIGHT = 60;
    constexpr int PIANO_HEIGHT = 120;
    constexpr int LEFT_COL_WIDTH = 150;
    constexpr int RIGHT_COL_WIDTH = 150;

    // ══ COLORS — exact from ma_musique.html CSS vars ══
    namespace Colors
    {
        // Core palette
        constexpr unsigned int BLACK        = 0xFF0A0A0A;  // --black: #0a0a0a
        constexpr unsigned int GOLD         = 0xFFC8922A;  // --gold: #c8922a
        constexpr unsigned int GOLD_LIGHT   = 0xFFE8B84B;  // --gold-light: #e8b84b
        constexpr unsigned int GOLD_DIM     = 0xFF8A6018;  // --gold-dim: #8a6018
        constexpr unsigned int RED          = 0xFFDC1E1E;  // --red: #dc1e1e
        constexpr unsigned int RED_GLOW     = 0x99DC1E1E;  // --red-glow: rgba(220,30,30,0.6)

        // Glassmorphism (exact opacities from CSS)
        constexpr unsigned int GLASS_BG     = 0xC70A0805;  // rgba(10,8,5,0.78) — 78% opacity
        constexpr unsigned int GLASS_BORDER = 0x38C8922A;  // rgba(200,146,42,0.22) — 22% opacity

        // Header: rgba(6,4,2,0.93)
        constexpr unsigned int HEADER_BG    = 0xED060402;  // 93% opacity

        // Knob internals (from SVG radialGradient)
        constexpr unsigned int KNOB_HIGHLIGHT = 0xFF3C3530;  // cx=38% cy=32%
        constexpr unsigned int KNOB_SHADOW    = 0xFF100D08;  // center-to-edge
        constexpr unsigned int KNOB_RING      = 0xFF1A1A1A;  // outer ring
        constexpr unsigned int KNOB_OUTLINE   = 0xFF2A2520;  // inner stroke

        // FX items
        constexpr unsigned int FX_OFF_BG       = 0x4D000000;  // rgba(0,0,0,0.3)
        constexpr unsigned int FX_OFF_BORDER   = 0xFF222222;  // border: 1px solid #222
        constexpr unsigned int FX_ON_BORDER    = 0x4DDC1E1E;  // rgba(220,30,30,0.3)
        constexpr unsigned int FX_ON_BG        = 0x0FDC1E1E;  // rgba(220,30,30,0.06)
        constexpr unsigned int FX_LED_OFF      = 0xFF333333;
        constexpr unsigned int FX_NAME_OFF     = 0xFF888888;
        constexpr unsigned int FX_NAME_ON      = 0xFFCCCCCC;

        // Piano keys (from CSS)
        constexpr unsigned int KEY_WHITE_TOP    = 0xFFF5F0E8;  // linear-gradient top
        constexpr unsigned int KEY_WHITE_BOT    = 0xFFE8E0D0;  // linear-gradient bottom
        constexpr unsigned int KEY_BLACK_TOP    = 0xFF222222;
        constexpr unsigned int KEY_BLACK_BOT    = 0xFF111111;
        constexpr unsigned int KEY_PRESSED      = 0xFFDC1E1E;  // .press background

        // Preset list
        constexpr unsigned int PRESET_HOVER_BG  = 0x14C8922A;  // rgba(200,146,42,0.08)
        constexpr unsigned int PRESET_ON_BG     = 0x24C8922A;  // rgba(200,146,42,0.14)
        constexpr unsigned int PRESET_ON_BORDER = 0xFF8A6018;  // var(--gold-dim)

        // Search box
        constexpr unsigned int SEARCH_BG        = 0x80000000;  // rgba(0,0,0,0.5)
        constexpr unsigned int SEARCH_BORDER    = 0xFF8A6018;  // var(--gold-dim)

        // Tab (instrument selector)
        constexpr unsigned int TAB_BG           = 0x66000000;  // rgba(0,0,0,0.4)
        constexpr unsigned int TAB_BORDER       = 0xFF333333;
        constexpr unsigned int TAB_ON_BG        = 0x26C8922A;  // rgba(200,146,42,0.15)
        constexpr unsigned int TAB_ON_BORDER    = 0xFFC8922A;  // var(--gold)
        constexpr unsigned int TAB_NAME_OFF     = 0xFFAAAAAA;
        constexpr unsigned int TAB_NAME_ON      = 0xFFC8922A;

        // Wheel
        constexpr unsigned int WHEEL_TRACK      = 0xFF111111;
        constexpr unsigned int WHEEL_TRACK_BORDER = 0xFF2A2A2A;
        constexpr unsigned int WHEEL_THUMB_TOP  = 0xFF3A3A3A;
        constexpr unsigned int WHEEL_THUMB_BOT  = 0xFF1A1A1A;
        constexpr unsigned int WHEEL_THUMB_BORDER = 0xFF555555;
    }

    // ══ DESIGN TOKENS — from CSS ══
    namespace Design
    {
        constexpr float BORDER_RADIUS_SM   = 3.0f;   // border-radius: 3px
        constexpr float BORDER_RADIUS_MD   = 4.0f;   // border-radius: 4px (fxitem, pitem, tab)
        constexpr float BORDER_RADIUS_LG   = 5.0f;   // border-radius: 5px (srch, tab)
        constexpr float GLASS_BLUR         = 14.0f;  // backdrop-filter: blur(14px)

        constexpr float KNOB_SIZE          = 42.0f;  // smaller round knobs
        constexpr float KNOB_ARC_WIDTH     = 2.8f;   // stroke-width in SVG
        constexpr float KNOB_POINTER_WIDTH = 2.3f;   // indicator line
        constexpr float KNOB_GLOW_ALPHA    = 0.5f;   // drop-shadow alpha

        constexpr float FX_LED_SIZE        = 8.0f;   // width:8px height:8px
        constexpr float TRANSITION_SPEED   = 0.18f;  // --tr: 0.18s ease

        constexpr float PIANO_KEY_RADIUS   = 5.0f;   // border-radius: 0 0 5px 5px
        constexpr float WAVEFORM_HEIGHT    = 26.0f;  // height: 26px
    }

    // Instrument names
    enum class Instrument
    {
        Violin = 0,
        Oud,
        Ney,
        Qanun,
        Darbuka,
        Rababah,
        Mizmar
    };

    inline const char* instrumentName(Instrument inst)
    {
        switch (inst)
        {
            case Instrument::Violin:  return "Violin";
            case Instrument::Oud:     return "Oud";
            case Instrument::Ney:     return "Ney";
            case Instrument::Qanun:   return "Qanun";
            case Instrument::Darbuka: return "Darbuka";
            case Instrument::Rababah: return "Rababah";
            case Instrument::Mizmar:  return "Mizmar";
            default: return "Unknown";
        }
    }
}
