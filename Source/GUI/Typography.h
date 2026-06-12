#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

// Centralised typographic hierarchy. Fonts are embedded via BinaryData
// (Cinzel for display/titles, Inter for body/labels) so the look is identical
// on every machine, regardless of installed system fonts.
namespace Typography
{
    inline juce::Typeface::Ptr cinzelRegular()
    {
        static auto tf = juce::Typeface::createSystemTypefaceFor(
            BinaryData::CinzelRegular_ttf, (size_t) BinaryData::CinzelRegular_ttfSize);
        return tf;
    }
    inline juce::Typeface::Ptr cinzelBold()
    {
        static auto tf = juce::Typeface::createSystemTypefaceFor(
            BinaryData::CinzelBold_ttf, (size_t) BinaryData::CinzelBold_ttfSize);
        return tf;
    }
    inline juce::Typeface::Ptr interRegular()
    {
        static auto tf = juce::Typeface::createSystemTypefaceFor(
            BinaryData::InterRegular_ttf, (size_t) BinaryData::InterRegular_ttfSize);
        return tf;
    }
    inline juce::Typeface::Ptr interBold()
    {
        static auto tf = juce::Typeface::createSystemTypefaceFor(
            BinaryData::InterBold_ttf, (size_t) BinaryData::InterBold_ttfSize);
        return tf;
    }
    // Blackletter — West-Coast / Chicano lowrider lettering (UnifrakturCook, OFL).
    inline juce::Typeface::Ptr chicano()
    {
        static auto tf = juce::Typeface::createSystemTypefaceFor(
            BinaryData::UnifrakturCookBold_ttf, (size_t) BinaryData::UnifrakturCookBold_ttfSize);
        return tf;
    }

    inline juce::Font display(float height, bool bold)
    {
        juce::Font f(bold ? cinzelBold() : cinzelRegular());
        f.setHeight(height);
        return f;
    }
    inline juce::Font body(float height, bool bold)
    {
        juce::Font f(bold ? interBold() : interRegular());
        f.setHeight(height);
        return f;
    }
    inline juce::Font chicanoFont(float height)
    {
        juce::Font f(chicano());
        f.setHeight(height);
        return f;
    }

    // Legacy family names kept for any remaining call sites.
    inline const juce::String& displayFamily() { static juce::String f = "Cinzel"; return f; }
    inline const juce::String& bodyFamily()    { static juce::String f = "Inter";  return f; }

    // ── Hierarchy ──
    inline juce::Font title()        { return display(22.0f, true); }
    inline juce::Font headerTitle()  { return display(20.0f, true); }
    inline juce::Font subtitle()     { return display( 9.0f, true); }
    inline juce::Font sectionLabel() { return body(    9.0f, true); }
    inline juce::Font knobLabel()    { return body(   10.0f, true); }
    inline juce::Font value()        { return body(   12.0f, false); }
    inline juce::Font tab()          { return display(12.0f, true); }

    // Uppercase + tracked spacing helper (JUCE has no letter-spacing, so we
    // inject thin spaces between glyphs for the "tracked caps" look).
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
