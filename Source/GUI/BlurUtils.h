#pragma once
#include <JuceHeader.h>

// Lightweight separable box blur (3 passes ≈ Gaussian) used to "frost" the
// background behind glass panels. Intended for one-shot use on size changes,
// not per-frame. Operates on an opaque ARGB image.
namespace BlurUtils
{
    inline void horizontalPass(juce::Image& src, juce::Image& dst, int radius)
    {
        const int w = src.getWidth();
        const int h = src.getHeight();
        const float norm = 1.0f / static_cast<float>(radius * 2 + 1);

        juce::Image::BitmapData s(src, juce::Image::BitmapData::readOnly);
        juce::Image::BitmapData d(dst, juce::Image::BitmapData::writeOnly);

        for (int y = 0; y < h; ++y)
        {
            float r = 0, g = 0, b = 0;
            // Prime the window
            for (int k = -radius; k <= radius; ++k)
            {
                int x = juce::jlimit(0, w - 1, k);
                auto c = s.getPixelColour(x, y);
                r += c.getFloatRed(); g += c.getFloatGreen(); b += c.getFloatBlue();
            }
            for (int x = 0; x < w; ++x)
            {
                d.setPixelColour(x, y, juce::Colour::fromFloatRGBA(r * norm, g * norm, b * norm, 1.0f));
                int addX = juce::jlimit(0, w - 1, x + radius + 1);
                int subX = juce::jlimit(0, w - 1, x - radius);
                auto ca = s.getPixelColour(addX, y);
                auto cs = s.getPixelColour(subX, y);
                r += ca.getFloatRed()   - cs.getFloatRed();
                g += ca.getFloatGreen() - cs.getFloatGreen();
                b += ca.getFloatBlue()  - cs.getFloatBlue();
            }
        }
    }

    inline void verticalPass(juce::Image& src, juce::Image& dst, int radius)
    {
        const int w = src.getWidth();
        const int h = src.getHeight();
        const float norm = 1.0f / static_cast<float>(radius * 2 + 1);

        juce::Image::BitmapData s(src, juce::Image::BitmapData::readOnly);
        juce::Image::BitmapData d(dst, juce::Image::BitmapData::writeOnly);

        for (int x = 0; x < w; ++x)
        {
            float r = 0, g = 0, b = 0;
            for (int k = -radius; k <= radius; ++k)
            {
                int y = juce::jlimit(0, h - 1, k);
                auto c = s.getPixelColour(x, y);
                r += c.getFloatRed(); g += c.getFloatGreen(); b += c.getFloatBlue();
            }
            for (int y = 0; y < h; ++y)
            {
                d.setPixelColour(x, y, juce::Colour::fromFloatRGBA(r * norm, g * norm, b * norm, 1.0f));
                int addY = juce::jlimit(0, h - 1, y + radius + 1);
                int subY = juce::jlimit(0, h - 1, y - radius);
                auto ca = s.getPixelColour(x, addY);
                auto cs = s.getPixelColour(x, subY);
                r += ca.getFloatRed()   - cs.getFloatRed();
                g += ca.getFloatGreen() - cs.getFloatGreen();
                b += ca.getFloatBlue()  - cs.getFloatBlue();
            }
        }
    }

    // Returns a blurred copy of the source image.
    inline juce::Image frosted(const juce::Image& source, int radius = 16, int passes = 3)
    {
        if (source.isNull() || radius < 1)
            return source.createCopy();

        juce::Image a = source.createCopy();
        juce::Image b = juce::Image(juce::Image::ARGB, source.getWidth(), source.getHeight(), true);

        for (int p = 0; p < passes; ++p)
        {
            horizontalPass(a, b, radius);
            verticalPass(b, a, radius);
        }
        return a;
    }
}
