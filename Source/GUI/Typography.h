#pragma once
#include <JuceHeader.h>

// Centralised typographic hierarchy. A single source of truth for every font
// in the UI so weights/sizes stay consistent and "premium".
//
// NOTE on BinaryData: once the real .ttf files (Cinzel, Inter) are dropped into
// Resources/Fonts and registered via juce_add_binary_data() in CMakeLists.txt,
// swap the named-typeface lookups below for:
//     juce::Typeface::createSystemTypefaceFor(BinaryData::Cinzel_ttf, BinaryData::Cinzel_ttfSize);
// cached in a static. Until then we fall back to the system-resolved name, which
// degrades gracefully to the default sans serif if the family is absent.
namespace Typography
{
    inline const juce::String& displayFamily() { static juce::String f = "Cinzel"; return f; } // titles
    inline const juce::String& bodyFamily()    { static juce::String f = "Inter";  return f; } // labels/values

    // ── Hierarchy ──
    inline juce::Font title()       { return juce::Font(displayFamily(), 22.0f, juce::Font::bold); }
    inline juce::Font subtitle()    { return juce::Font(displayFamily(),  9.0f, juce::Font::bold); }
    inline juce::Font sectionLabel(){ return juce::Font(bodyFamily(),     9.0f, juce::Font::bold); }
    inline juce::Font knobLabel()   { return juce::Font(bodyFamily(),    10.0f, juce::Font::bold); }
    inline juce::Font value()       { return juce::Font(bodyFamily(),    12.0f, juce::Font::plain); }
    inline juce::Font tab()         { return juce::Font(displayFamily(),  9.0f, juce::Font::bold); }

    // Uppercase + tracked spacing helper for labels (JUCE has no letter-spacing,
    // so we inject thin spaces between glyphs for the "tracked caps" look).
    inline juce::String tracked(const juce::String& text)
    {
        juce::String out;
        auto up = text.toUpperCase();
        for (int i = 0; i < up.length(); ++i)
        {
            out += up[i];
            if (i < up.length() - 1)
                out += juce::String::charToString(0x2009); // thin space
        }
        return out;
    }
}
