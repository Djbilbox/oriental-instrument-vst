#include "BackgroundComponent.h"
#include "../Utils/Constants.h"

void BackgroundComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    drawDesertGradient(g, bounds);
    drawStars(g, bounds);
    drawSun(g, bounds.getWidth() * 0.51f, bounds.getHeight() * 0.56f);
    drawPyramids(g, bounds);
    drawDunes(g, bounds);
    drawVignette(g, bounds);
}

void BackgroundComponent::drawDesertGradient(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Exact sky gradient from HTML SVG: #sky linearGradient
    juce::ColourGradient sky(juce::Colour(0xFF030008), 0, 0,
                             juce::Colour(0xFFC86010), 0, bounds.getHeight(), false);
    sky.addColour(0.08, juce::Colour(0xFF0A0115));
    sky.addColour(0.22, juce::Colour(0xFF1E0418));
    sky.addColour(0.36, juce::Colour(0xFF380610));
    sky.addColour(0.52, juce::Colour(0xFF6E1A06));
    sky.addColour(0.68, juce::Colour(0xFFAE3C0A));
    sky.addColour(0.82, juce::Colour(0xFFD85E14));
    sky.addColour(0.92, juce::Colour(0xFFEE7C18));
    g.setGradientFill(sky);
    g.fillRect(bounds);

    // Desert ground floor: #2a1406 opacity 0.7
    g.setColour(juce::Colour(0xB32A1406));
    g.fillRect(0.0f, bounds.getHeight() * 0.556f, bounds.getWidth(), bounds.getHeight() * 0.45f);
}

void BackgroundComponent::drawStars(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();

    // Croissant de lune (from HTML: translate(88,44), r=11)
    float moonX = w * 0.09f;
    float moonY = h * 0.069f;
    g.setColour(juce::Colour(0xE0FFF8E0));
    g.fillEllipse(moonX - 11.0f, moonY - 11.0f, 22.0f, 22.0f);
    g.setColour(juce::Colour(0xF20E0520)); // dark mask for crescent
    g.fillEllipse(moonX - 6.0f, moonY - 14.0f, 18.0f, 18.0f);

    // Large stars (exact positions from HTML scaled to percentage)
    struct Star { float x; float y; float r; float alpha; };
    Star stars[] = {
        {0.059f, 0.034f, 1.4f, 0.85f}, {0.143f, 0.016f, 1.0f, 0.65f},
        {0.230f, 0.055f, 1.2f, 0.75f}, {0.316f, 0.023f, 0.9f, 0.70f},
        {0.398f, 0.070f, 0.8f, 0.60f}, {0.471f, 0.013f, 1.5f, 0.90f},
        {0.549f, 0.044f, 0.9f, 0.65f}, {0.631f, 0.009f, 1.1f, 0.75f},
        {0.709f, 0.066f, 1.0f, 0.68f}, {0.784f, 0.028f, 1.3f, 0.80f},
        {0.862f, 0.053f, 0.8f, 0.55f}, {0.937f, 0.019f, 1.1f, 0.72f},
        {0.971f, 0.081f, 0.9f, 0.60f},
        // Smaller stars
        {0.080f, 0.102f, 0.7f, 0.50f}, {0.189f, 0.125f, 0.8f, 0.55f},
        {0.355f, 0.109f, 0.7f, 0.50f}, {0.490f, 0.086f, 0.6f, 0.60f},
        {0.665f, 0.113f, 0.9f, 0.58f}, {0.827f, 0.094f, 0.8f, 0.52f},
        {0.029f, 0.075f, 0.6f, 0.45f}, {0.745f, 0.133f, 0.7f, 0.40f},
    };

    for (auto& s : stars)
    {
        g.setColour(juce::Colours::white.withAlpha(s.alpha));
        g.fillEllipse(s.x * w - s.r, s.y * h - s.r, s.r * 2.0f, s.r * 2.0f);
    }

    // Shooting star (line from HTML: x1=160 y1=38 x2=220 y2=52)
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawLine(w * 0.163f, h * 0.059f, w * 0.224f, h * 0.081f, 0.6f);
}

void BackgroundComponent::drawSun(juce::Graphics& g, float cx, float cy)
{
    // Sun halo: radialGradient #sunhalo cx=51% cy=58% r=40%
    // #ffb840 alpha 0.65 → #f07010 alpha 0.35 → transparent
    juce::ColourGradient halo(juce::Colour(0xA6FFB840), cx, cy,
                              juce::Colours::transparentBlack, cx, cy - 210.0f, true);
    halo.addColour(0.25, juce::Colour(0x59F07010));
    halo.addColour(0.60, juce::Colour(0x1F882010));
    g.setGradientFill(halo);
    g.fillEllipse(cx - 320.0f, cy - 210.0f, 640.0f, 420.0f);

    // Sun rays (very subtle, opacity 0.055)
    g.setColour(juce::Colour(0x0EFFAA30));
    float rayAngles[] = { -1.2f, -0.6f, -0.1f, 0.3f, 0.8f, 1.1f };
    for (float angle : rayAngles)
    {
        float endX = cx + std::cos(angle) * 500.0f;
        float endY = cy + std::sin(angle) * -400.0f;
        g.drawLine(cx, cy, endX, endY, 25.0f);
    }

    // Sun core: radialGradient #suncore
    juce::ColourGradient core(juce::Colour(0xFAFFFBE0), cx, cy,
                              juce::Colours::transparentBlack, cx + 42.0f, cy, true);
    core.addColour(0.30, juce::Colour(0xD9FFD060));
    core.addColour(0.65, juce::Colour(0x66FF9020));
    g.setGradientFill(core);
    g.fillEllipse(cx - 26.0f, cy - 26.0f, 52.0f, 52.0f);

    // Corona rings
    g.setColour(juce::Colour(0x4DFFCC40));
    g.drawEllipse(cx - 32.0f, cy - 32.0f, 64.0f, 64.0f, 1.5f);
    g.setColour(juce::Colour(0x26FFA830));
    g.drawEllipse(cx - 42.0f, cy - 42.0f, 84.0f, 84.0f, 0.8f);
}

void BackgroundComponent::drawPyramids(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float baseY = h * 0.566f; // y=362 / 640

    // Khufu — Grande Pyramide (points from HTML: 268,362 → 400,185 → 540,362)
    {
        // Dark face (west)
        juce::Path darkFace;
        darkFace.startNewSubPath(w * 0.2735f, baseY);
        darkFace.lineTo(w * 0.4082f, h * 0.289f);
        darkFace.lineTo(w * 0.4388f, baseY);
        darkFace.closeSubPath();

        juce::ColourGradient darkGrad(juce::Colour(0xFA100602), w * 0.4082f, h * 0.289f,
                                       juce::Colour(0xFA1A0A04), w * 0.2735f, baseY, false);
        g.setGradientFill(darkGrad);
        g.fillPath(darkFace);

        // Lit face (east) — pyra-lit gradient
        juce::Path litFace;
        litFace.startNewSubPath(w * 0.4082f, h * 0.289f);
        litFace.lineTo(w * 0.5510f, baseY);
        litFace.lineTo(w * 0.4388f, baseY);
        litFace.closeSubPath();

        juce::ColourGradient litGrad(juce::Colour(0xF52A1406), w * 0.4082f, h * 0.289f,
                                      juce::Colour(0xF53E2010), w * 0.5510f, baseY, false);
        g.setGradientFill(litGrad);
        g.fillPath(litFace);

        // Ridge line
        g.setColour(juce::Colour(0x66503020));
        g.drawLine(w * 0.4082f, h * 0.289f, w * 0.4082f, baseY, 0.6f);

        // Summit point
        g.setColour(juce::Colour(0xB3C8A060));
        g.fillEllipse(w * 0.4082f - 2.5f, h * 0.289f - 2.5f, 5.0f, 5.0f);

        // Entrance
        juce::Path entrance;
        entrance.startNewSubPath(w * 0.365f, h * 0.50f);
        entrance.lineTo(w * 0.383f, h * 0.445f);
        entrance.lineTo(w * 0.400f, h * 0.50f);
        entrance.closeSubPath();
        g.setColour(juce::Colour(0xE6080300));
        g.fillPath(entrance);

        // Stone lines
        g.setColour(juce::Colour(0x4D1A0A02));
        for (float row = 0.1f; row < 0.9f; row += 0.1f)
        {
            float ly = h * 0.289f + (baseY - h * 0.289f) * row;
            float lx1 = w * 0.2735f + (w * 0.4082f - w * 0.2735f) * (1.0f - row) * 0.15f + (w * 0.4082f - w * 0.2735f) * row * 0.1f;
            float lx2 = w * 0.4082f + (w * 0.5510f - w * 0.4082f) * row * 0.9f;
            g.drawLine(lx1, ly, lx2, ly, 0.4f);
        }
    }

    // Khafre — 2nd pyramid (528,362 → 636,207 → 748,362)
    {
        juce::Path dark;
        dark.startNewSubPath(w * 0.539f, baseY);
        dark.lineTo(w * 0.649f, h * 0.323f);
        dark.lineTo(w * 0.674f, baseY);
        dark.closeSubPath();
        g.setColour(juce::Colour(0xF0160804));
        g.fillPath(dark);

        juce::Path lit;
        lit.startNewSubPath(w * 0.649f, h * 0.323f);
        lit.lineTo(w * 0.764f, baseY);
        lit.lineTo(w * 0.674f, baseY);
        lit.closeSubPath();
        g.setColour(juce::Colour(0xEB241408));
        g.fillPath(lit);

        g.setColour(juce::Colour(0x59503020));
        g.drawLine(w * 0.649f, h * 0.323f, w * 0.649f, baseY, 0.5f);
        g.setColour(juce::Colour(0x99C8A060));
        g.fillEllipse(w * 0.649f - 2.0f, h * 0.323f - 2.0f, 4.0f, 4.0f);

        // Limestone cap
        juce::Path cap;
        cap.startNewSubPath(w * 0.649f, h * 0.323f);
        cap.lineTo(w * 0.674f, h * 0.375f);
        cap.lineTo(w * 0.624f, h * 0.375f);
        cap.closeSubPath();
        g.setColour(juce::Colour(0x598A7050));
        g.fillPath(cap);
    }

    // Menkaure — 3rd pyramid (718,362 → 778,265 → 838,362)
    {
        juce::Path dark;
        dark.startNewSubPath(w * 0.733f, baseY);
        dark.lineTo(w * 0.794f, h * 0.414f);
        dark.lineTo(w * 0.815f, baseY);
        dark.closeSubPath();
        g.setColour(juce::Colour(0xE6140602));
        g.fillPath(dark);

        juce::Path lit;
        lit.startNewSubPath(w * 0.794f, h * 0.414f);
        lit.lineTo(w * 0.855f, baseY);
        lit.lineTo(w * 0.815f, baseY);
        lit.closeSubPath();
        g.setColour(juce::Colour(0xE0201008));
        g.fillPath(lit);

        g.setColour(juce::Colour(0x80C8A060));
        g.fillEllipse(w * 0.794f - 1.5f, h * 0.414f - 1.5f, 3.0f, 3.0f);
    }

    // Giza plateau base
    g.setColour(juce::Colour(0xBF1A0C04));
    g.fillRect(w * 0.245f, baseY - 2.0f, w * 0.633f, 10.0f);
}

void BackgroundComponent::drawDunes(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();

    // Dune layer A: gradient #da (c06818 → 4a1e04), opacity 0.78
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.606f);
        dune.quadraticTo(w * 0.092f, h * 0.531f, w * 0.209f, h * 0.588f);
        dune.quadraticTo(w * 0.327f, h * 0.644f, w * 0.457f, h * 0.544f);
        dune.quadraticTo(w * 0.584f, h * 0.444f, w * 0.702f, h * 0.563f);
        dune.quadraticTo(w * 0.820f, h * 0.681f, w, h * 0.575f);
        dune.lineTo(w, h * 0.722f);
        dune.lineTo(0, h * 0.722f);
        dune.closeSubPath();

        juce::ColourGradient grad(juce::Colour(0xC7C06818), 0, h * 0.531f,
                                   juce::Colour(0xC74A1E04), 0, h * 0.722f, false);
        g.setGradientFill(grad);
        g.fillPath(dune);
    }

    // Dune layer B: gradient #db (d87820 → 5e2608), opacity 0.85
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.669f);
        dune.quadraticTo(w * 0.087f, h * 0.602f, w * 0.202f, h * 0.656f);
        dune.quadraticTo(w * 0.318f, h * 0.711f, w * 0.461f, h * 0.622f);
        dune.quadraticTo(w * 0.598f, h * 0.533f, w * 0.735f, h * 0.644f);
        dune.quadraticTo(w * 0.869f, h * 0.750f, w, h * 0.653f);
        dune.lineTo(w, h * 0.828f);
        dune.lineTo(0, h * 0.828f);
        dune.closeSubPath();

        juce::ColourGradient grad(juce::Colour(0xD9D87820), 0, h * 0.602f,
                                   juce::Colour(0xD95E2608), 0, h * 0.828f, false);
        g.setGradientFill(grad);
        g.fillPath(dune);
    }

    // Dune layer C: gradient #dc (e89030 → 6e3010), opacity 0.90
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.742f);
        dune.quadraticTo(w * 0.138f, h * 0.675f, w * 0.314f, h * 0.741f);
        dune.quadraticTo(w * 0.488f, h * 0.809f, w * 0.631f, h * 0.722f);
        dune.quadraticTo(w * 0.774f, h * 0.634f, w, h * 0.747f);
        dune.lineTo(w, h * 0.891f);
        dune.lineTo(0, h * 0.891f);
        dune.closeSubPath();

        juce::ColourGradient grad(juce::Colour(0xE6E89030), 0, h * 0.675f,
                                   juce::Colour(0xE66E3010), 0, h * 0.891f, false);
        g.setGradientFill(grad);
        g.fillPath(dune);
    }

    // Dune layer D: gradient #dd (cf7828 → 361404), opacity 0.96
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.825f);
        dune.quadraticTo(w * 0.179f, h * 0.773f, w * 0.386f, h * 0.825f);
        dune.quadraticTo(w * 0.574f, h * 0.875f, w * 0.755f, h * 0.806f);
        dune.quadraticTo(w * 0.886f, h * 0.756f, w, h * 0.820f);
        dune.lineTo(w, h);
        dune.lineTo(0, h);
        dune.closeSubPath();

        juce::ColourGradient grad(juce::Colour(0xF5CF7828), 0, h * 0.773f,
                                   juce::Colour(0xF5361404), 0, h, false);
        g.setGradientFill(grad);
        g.fillPath(dune);
    }

    // Bottom sand floor: #3a1c08 opacity 0.99
    g.setColour(juce::Colour(0xFC3A1C08));
    g.fillRect(0.0f, h * 0.930f, w, h * 0.07f);

    // Caravan traces (dashed path)
    g.setColour(juce::Colour(0x664A2008));
    juce::Path caravan;
    caravan.startNewSubPath(w * 0.051f, h * 0.875f);
    caravan.quadraticTo(w * 0.204f, h * 0.847f, w * 0.388f, h * 0.863f);
    caravan.quadraticTo(w * 0.531f, h * 0.875f, w * 0.694f, h * 0.836f);
    caravan.quadraticTo(w * 0.816f, h * 0.805f, w * 0.969f, h * 0.844f);
    juce::PathStrokeType caravanStroke(1.5f);
    float dashLengths[] = { 6.0f, 8.0f };
    juce::Path dashedCaravan;
    caravanStroke.createDashedStroke(dashedCaravan, caravan, dashLengths, 2);
    g.fillPath(dashedCaravan);

    // Dune crest highlights
    g.setColour(juce::Colour(0x2EE8A040));
    juce::Path crest1;
    crest1.startNewSubPath(0, h * 0.606f);
    crest1.quadraticTo(w * 0.092f, h * 0.531f, w * 0.209f, h * 0.588f);
    crest1.quadraticTo(w * 0.327f, h * 0.644f, w * 0.457f, h * 0.544f);
    crest1.quadraticTo(w * 0.584f, h * 0.444f, w * 0.702f, h * 0.563f);
    crest1.quadraticTo(w * 0.820f, h * 0.681f, w, h * 0.575f);
    g.strokePath(crest1, juce::PathStrokeType(0.8f));
}

void BackgroundComponent::drawVignette(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Radial vignette: transparent center → 65% black edges
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float radius = bounds.getWidth() * 0.7f;

    juce::ColourGradient vig(juce::Colours::transparentBlack, cx, cy,
                              juce::Colour(0xA6000000), cx + radius, cy, true);
    vig.addColour(0.7, juce::Colours::transparentBlack);
    g.setGradientFill(vig);
    g.fillRect(bounds);
}
