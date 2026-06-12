#include "BackgroundComponent.h"
#include "BlurUtils.h"
#include "Typography.h"
#include "../Utils/Constants.h"
#include <initializer_list>
#include <cmath>

using namespace OrientalConstants;

// RGB (0xRRGGBB) + alpha helper to mirror SVG fill+opacity.
static inline juce::Colour rgbA(unsigned int rgb, float a)
{
    return juce::Colour(0xFF000000u | rgb).withAlpha(a);
}

BackgroundComponent::BackgroundComponent()
{
    startTimerHz(30); // animated halo
}

BackgroundComponent::~BackgroundComponent()
{
    stopTimer();
}

void BackgroundComponent::resized()
{
    rebuildCaches();
}

void BackgroundComponent::timerCallback()
{
    haloPhase += 0.045f;
    if (haloPhase > juce::MathConstants<float>::twoPi)
        haloPhase -= juce::MathConstants<float>::twoPi;
    repaint();
}

void BackgroundComponent::rebuildCaches()
{
    auto b = getLocalBounds();
    if (b.isEmpty())
        return;

    const float sx = b.getWidth()  / 980.0f;
    const float sy = b.getHeight() / 640.0f;

    cachedSize = b;
    cachedDesert = juce::Image(juce::Image::ARGB, b.getWidth(), b.getHeight(), true);
    {
        juce::Graphics g(cachedDesert);
        auto bounds = b.toFloat();
        // Premium dark arabesque backdrop, Algiers monument + flag kept.
        drawArabesqueBackdrop(g, bounds);
        drawMonument(g, sx, sy);     // Maqam Echahid (Monument des Martyrs, Alger)
        drawAlgerianFlag(g, sx, sy);
        drawVignette(g, bounds);
    }

    cachedFrosted = BlurUtils::frosted(cachedDesert, 17, 3);
}

void BackgroundComponent::paint(juce::Graphics& g)
{
    if (cachedDesert.isNull() || cachedSize != getLocalBounds())
        rebuildCaches();

    g.drawImageAt(cachedDesert, 0, 0);

    // Subtle breathing halo on top for life
    float pulse = 0.05f + 0.05f * std::sin(haloPhase);
    drawHalo(g, getWidth() * 0.5f, getHeight() * 0.02f, pulse);

    drawGlassPanels(g);
    drawChrome(g);
}

// ════════════════════════════ GLASS PANELS ════════════════════════════

void BackgroundComponent::drawGlassPanel(juce::Graphics& g, juce::Rectangle<int> r,
                                          bool topEdge, bool leftEdge)
{
    auto rf = r.toFloat();

    {
        juce::Graphics::ScopedSaveState save(g);
        g.reduceClipRegion(r);
        if (cachedFrosted.isValid())
            g.drawImageAt(cachedFrosted, 0, 0);
    }

    g.setColour(juce::Colour(0xA60C0906)); // ~65% warm-dark tint
    g.fillRect(rf);

    juce::ColourGradient sheen(juce::Colours::white.withAlpha(0.07f), rf.getX(), rf.getY(),
                               juce::Colours::transparentWhite, rf.getX(), rf.getY() + 70.0f, false);
    g.setGradientFill(sheen);
    g.fillRect(rf.withHeight(70.0f));

    juce::ColourGradient border(juce::Colour(Colors::GOLD_LIGHT).withAlpha(0.45f), rf.getX(), rf.getY(),
                                juce::Colour(Colors::GOLD_DIM).withAlpha(0.14f), rf.getX(), rf.getBottom(), false);
    g.setGradientFill(border);
    if (topEdge)  g.fillRect(rf.getX(), rf.getY(), rf.getWidth(), 1.0f);
    if (leftEdge) g.fillRect(rf.getX(), rf.getY(), 1.0f, rf.getHeight());
    g.fillRect(rf.getRight() - 1.0f, rf.getY(), 1.0f, rf.getHeight());
}

void BackgroundComponent::drawGlassPanels(juce::Graphics& g)
{
    auto b = getLocalBounds();
    b.removeFromTop(HEADER_HEIGHT);
    b.removeFromBottom(PIANO_HEIGHT);

    auto left   = b.removeFromLeft(LEFT_COL_WIDTH);
    auto right  = b.removeFromRight(RIGHT_COL_WIDTH);
    auto center = b;

    drawGlassPanel(g, left,   true, false);
    drawGlassPanel(g, center, true, true);
    drawGlassPanel(g, right,  true, true);
}

// ════════════════════════════ CHROME ════════════════════════════

void BackgroundComponent::drawChrome(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(HEADER_HEIGHT).toFloat();

    // Header glass bar
    g.setColour(juce::Colour(Colors::HEADER_BG));
    g.fillRect(header);
    juce::ColourGradient hdr(juce::Colour(Colors::GOLD).withAlpha(0.35f), header.getX(), header.getBottom(),
                             juce::Colour(Colors::GOLD_DIM).withAlpha(0.10f), header.getRight(), header.getBottom(), false);
    g.setGradientFill(hdr);
    g.fillRect(header.getX(), header.getBottom() - 1.0f, header.getWidth(), 1.0f);

    // ── Logo (left): geo diamonds + name + sub ──
    {
        float cx = 18.0f, cy = header.getCentreY();
        for (int i = 0; i < 3; ++i)
        {
            juce::Path d;
            float s = 5.0f;
            d.addRectangle(cx + i * 8.0f - s * 0.5f, cy - 9.0f - s * 0.5f, s, s);
            g.setColour(i == 1 ? juce::Colour(Colors::GOLD) : juce::Colour(Colors::GOLD_DIM));
            g.fillPath(d, juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 0.25f,
                                                          cx + i * 8.0f, cy - 9.0f));
        }
        g.setColour(juce::Colour(Colors::GOLD));
        g.setFont(Typography::display(11.0f, true));
        g.drawText("DJBILBOX", juce::Rectangle<float>(12.0f, cy - 2.0f, 150.0f, 14.0f),
                   juce::Justification::centredLeft);
        g.setColour(juce::Colour(Colors::GOLD_DIM));
        g.setFont(Typography::body(7.5f, false));
        g.drawText(Typography::tracked("Maqam Engine Pro"),
                   juce::Rectangle<float>(12.0f, cy + 9.0f, 200.0f, 10.0f), juce::Justification::centredLeft);
    }

    // ── Title (center) + by-line — centred in the open middle area so it clears
    //    the logo (left) and the preset label + transport buttons (right) ──
    auto titleZone = header.withTrimmedLeft(170.0f).withTrimmedRight(270.0f);

    // Golden light rays fanning down behind the title.
    drawLightRays(g, titleZone.getCentreX(), 0.0f, 96.0f);

    g.setColour(juce::Colour(Colors::GOLD_LIGHT));
    g.setFont(Typography::headerTitle());
    g.drawText("ORIENTAL INSTRUMENT",
               titleZone.withTrimmedTop(8.0f), juce::Justification::centredTop);

    // ── By-line: sober, luxurious, legible — Cinzel tracked caps in gold,
    //    framed by two thin gold rules (engraved-plate look). ──
    {
        auto blz = titleZone.withTrimmedTop(28.0f).withTrimmedBottom(2.0f);
        g.setColour(juce::Colour(Colors::GOLD_DIM));
        g.setFont(Typography::display(8.5f, true));
        const juce::String byline = Typography::tracked("DJBILBOX BEATS");
        g.drawText(byline, blz, juce::Justification::centredTop);

        // Thin gold rules flanking the name.
        const float ty = blz.getY() + 6.0f;
        const float tw = g.getCurrentFont().getStringWidthFloat(byline);
        const float cx = blz.getCentreX();
        g.setColour(juce::Colour(Colors::GOLD_DIM).withAlpha(0.55f));
        g.fillRect(cx - tw * 0.5f - 22.0f, ty, 16.0f, 1.0f);
        g.fillRect(cx + tw * 0.5f + 6.0f,  ty, 16.0f, 1.0f);
    }

    // (Transport buttons are real interactive components in the editor.)

    // ── Section label (left "MACRO"; the right column's FX title is drawn by FXPanel) ──
    auto main = bounds;
    main.removeFromBottom(PIANO_HEIGHT);
    auto left = main.removeFromLeft(LEFT_COL_WIDTH);

    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.setFont(Typography::sectionLabel());
    g.drawText(Typography::tracked("Macro"), left.removeFromTop(18), juce::Justification::centred);

    // Piano divider + outer frame
    float pianoTop = static_cast<float>(getHeight() - PIANO_HEIGHT);
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.drawLine(0.0f, pianoTop, static_cast<float>(getWidth()), pianoTop, 1.0f);
    g.drawRect(getLocalBounds(), 1);
}

// ════════════════════════════ SCENE — base ════════════════════════════

void BackgroundComponent::drawDesertGradient(juce::Graphics& g, juce::Rectangle<float> bounds)
{
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

    g.setColour(juce::Colour(0xB32A1406));
    g.fillRect(0.0f, bounds.getHeight() * 0.556f, bounds.getWidth(), bounds.getHeight() * 0.45f);
}

// ── Premium dark backdrop: deep gradient, warm top-centre light, faint gold
//    arabesque mandalas. Replaces the desert scene. ──
void BackgroundComponent::drawArabesqueBackdrop(juce::Graphics& g, juce::Rectangle<float> b)
{
    const float w = b.getWidth(), h = b.getHeight();

    // Base vertical gradient — near-black, a touch warmer toward the top centre.
    juce::ColourGradient base(juce::Colour(0xFF14100B), 0, 0,
                              juce::Colour(0xFF050403), 0, h, false);
    base.addColour(0.45, juce::Colour(0xFF0C0A07));
    g.setGradientFill(base);
    g.fillRect(b);

    // Warm light source at top centre (origin of the title rays).
    juce::ColourGradient glow(juce::Colour(Colors::GOLD).withAlpha(0.16f), w * 0.5f, h * 0.02f,
                              juce::Colours::transparentBlack, w * 0.5f, h * 0.42f, true);
    g.setGradientFill(glow);
    g.fillRect(b);

    // Faint gold arabesque mandalas.
    drawMandala(g, w * 0.50f, h * 0.46f, juce::jmin(w, h) * 0.40f, 16, 0.06f);
    drawMandala(g, w * 0.50f, h * 0.46f, juce::jmin(w, h) * 0.26f, 12, 0.05f);
    drawMandala(g, w * 0.12f, h * 0.80f, juce::jmin(w, h) * 0.20f, 12, 0.04f);
    drawMandala(g, w * 0.88f, h * 0.80f, juce::jmin(w, h) * 0.20f, 12, 0.04f);
    drawMandala(g, w * 0.12f, h * 0.16f, juce::jmin(w, h) * 0.16f, 10, 0.035f);
    drawMandala(g, w * 0.88f, h * 0.16f, juce::jmin(w, h) * 0.16f, 10, 0.035f);
}

// One arabesque mandala: concentric rings, radial spokes, node circles, petals.
void BackgroundComponent::drawMandala(juce::Graphics& g, float cx, float cy, float R,
                                      int petals, float alpha)
{
    const juce::Colour gold = juce::Colour(Colors::GOLD).withAlpha(alpha);
    g.setColour(gold);

    g.drawEllipse(cx - R, cy - R, R * 2.0f, R * 2.0f, 1.0f);
    g.drawEllipse(cx - R * 0.62f, cy - R * 0.62f, R * 1.24f, R * 1.24f, 1.0f);
    g.drawEllipse(cx - R * 0.30f, cy - R * 0.30f, R * 0.60f, R * 0.60f, 1.0f);

    for (int k = 0; k < petals; ++k)
    {
        const float a = juce::MathConstants<float>::twoPi * (static_cast<float>(k) / petals);
        const float ca = std::cos(a), sa = std::sin(a);

        // spoke
        g.drawLine(cx + ca * R * 0.30f, cy + sa * R * 0.30f,
                   cx + ca * R, cy + sa * R, 0.8f);

        // node circle on the mid ring
        const float nx = cx + ca * R * 0.62f, ny = cy + sa * R * 0.62f;
        g.drawEllipse(nx - R * 0.05f, ny - R * 0.05f, R * 0.10f, R * 0.10f, 0.8f);

        // pointed petal between the mid ring and the rim
        const float a2 = a + juce::MathConstants<float>::twoPi / (petals * 2.0f);
        juce::Path petal;
        petal.startNewSubPath(cx + ca * R * 0.62f, cy + sa * R * 0.62f);
        petal.quadraticTo(cx + std::cos(a2) * R * 0.92f, cy + std::sin(a2) * R * 0.92f,
                          cx + std::cos(a + juce::MathConstants<float>::twoPi / petals) * R * 0.62f,
                          cy + std::sin(a + juce::MathConstants<float>::twoPi / petals) * R * 0.62f);
        g.strokePath(petal, juce::PathStrokeType(0.8f));
    }
}

// Volumetric light rays fanning down from a point (drawn sharp behind the title).
void BackgroundComponent::drawLightRays(juce::Graphics& g, float cx, float topY, float len)
{
    juce::Graphics::ScopedSaveState save(g);
    static const float spread[] = { -0.42f, -0.26f, -0.12f, 0.04f, 0.18f, 0.34f, 0.50f };
    for (float s : spread)
    {
        juce::Path ray;
        ray.startNewSubPath(cx, topY);
        ray.lineTo(cx + s * len - 10.0f, topY + len);
        ray.lineTo(cx + s * len + 10.0f, topY + len);
        ray.closeSubPath();
        juce::ColourGradient rg(juce::Colour(Colors::GOLD_LIGHT).withAlpha(0.10f), cx, topY,
                                juce::Colours::transparentBlack, cx, topY + len, false);
        g.setGradientFill(rg);
        g.fillPath(ray);
    }
}

// ── Maqam Echahid (Monument des Martyrs, Alger) — three concrete palm fronds
//    curving up to a shared apex, sheltering an eternal flame. Drawn as a
//    backlit silhouette with a warm gold rim against the sun. ──
void BackgroundComponent::drawMonument(juce::Graphics& g, float sx, float sy)
{
    auto P = [&](float x, float y) { return juce::Point<float>(x * sx, y * sy); };

    const float cx = 360.0f;     // scene-space centre (980x640 reference)
    const float baseY = 366.0f;  // foot on the horizon
    const float apexY = 226.0f;  // fronds meet here

    const juce::Colour body(0xFF3A2820u);          // lighter silhouette for visibility
    const juce::Colour rim(Colors::GOLD);          // brighter gold rim

    // Base platform
    g.setColour(body);
    g.fillRect(P(cx - 34.0f, baseY).x, P(cx - 34.0f, baseY).y, 68.0f * sx, 12.0f * sy);

    // Three fronds: left, centre, right, all leaning into the apex.
    for (int i = -1; i <= 1; ++i)
    {
        const float foot = cx + i * 24.0f;
        const float fw   = 11.0f;
        const float midY = (baseY + apexY) * 0.5f;

        juce::Path fr;
        fr.startNewSubPath(P(foot - fw, baseY));
        fr.quadraticTo(P(foot - i * 10.0f - 12.0f, midY), P(cx - i * 3.0f, apexY));
        fr.quadraticTo(P(foot - i * 10.0f + 8.0f,  midY), P(foot + fw, baseY));
        fr.closeSubPath();

        g.setColour(body);
        g.fillPath(fr);
        g.setColour(rim.withAlpha(0.75f));
        g.strokePath(fr, juce::PathStrokeType(1.5f));
    }

    // Eternal flame at the apex
    auto flame = P(cx, apexY - 6.0f);
    juce::ColourGradient fg(rim.withAlpha(1.0f), flame.x, flame.y,
                            rim.withAlpha(0.3f), flame.x, flame.y - 14.0f * sy, false);
    g.setGradientFill(fg);
    g.fillEllipse(flame.x - 5.0f * sx, flame.y - 14.0f * sy, 10.0f * sx, 20.0f * sy);
    // Glow around flame
    g.setColour(rim.withAlpha(0.25f));
    g.fillEllipse(flame.x - 8.0f * sx, flame.y - 18.0f * sy, 16.0f * sx, 26.0f * sy);
}

// ── Algerian flag on a pole, planted on the dune (green/white split, red
//    crescent + five-point star). ──
void BackgroundComponent::drawAlgerianFlag(juce::Graphics& g, float sx, float sy)
{
    auto P = [&](float x, float y) { return juce::Point<float>(x * sx, y * sy); };

    const float px = 656.0f;     // pole x
    const float topY = 300.0f;   // flag top
    const float baseY = 372.0f;  // pole foot on the dune
    const float fw = 65.0f, fh = 42.0f;

    // Pole (thicker for visibility)
    g.setColour(juce::Colour(0xFF4A3A2A));
    g.fillRect(P(px, topY - 6.0f).x, P(px, topY - 6.0f).y, 3.0f * sx, (baseY - topY + 6.0f) * sy);

    auto fl = juce::Rectangle<float>(P(px + 2.0f, topY).x, P(px + 2.0f, topY).y, fw * sx, fh * sy);

    // Green (left) + white (right) halves
    g.setColour(juce::Colour(0xFF006233));
    g.fillRect(fl.withWidth(fl.getWidth() * 0.5f));
    g.setColour(juce::Colour(0xFFF2F2EE));
    g.fillRect(fl.withTrimmedLeft(fl.getWidth() * 0.5f));

    // Red crescent (centred on the seam) — outer disc minus an offset disc.
    const juce::Colour red(Colors::RED);
    const float ccx = fl.getCentreX(), ccy = fl.getCentreY();
    const float r = fh * 0.30f * sy;
    juce::Path crescent;
    crescent.setUsingNonZeroWinding(false);
    crescent.addEllipse(ccx - r, ccy - r, r * 2.0f, r * 2.0f);
    crescent.addEllipse(ccx - r + r * 0.55f, ccy - r * 0.82f, r * 1.64f, r * 1.64f);
    g.setColour(red);
    g.fillPath(crescent);

    // Five-point star in the crescent opening.
    juce::Path star;
    const float sr = r * 0.62f, srIn = sr * 0.42f;
    const float starCx = ccx + r * 0.55f, starCy = ccy;
    for (int k = 0; k < 10; ++k)
    {
        const float ang = -juce::MathConstants<float>::halfPi + k * juce::MathConstants<float>::pi / 5.0f;
        const float rad = (k % 2 == 0) ? sr : srIn;
        const auto pt = juce::Point<float>(starCx + std::cos(ang) * rad, starCy + std::sin(ang) * rad);
        if (k == 0) star.startNewSubPath(pt); else star.lineTo(pt);
    }
    star.closeSubPath();
    g.setColour(red);
    g.fillPath(star);
}

void BackgroundComponent::drawStars(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();

    float moonX = w * 0.09f;
    float moonY = h * 0.069f;
    g.setColour(juce::Colour(0xE0FFF8E0));
    g.fillEllipse(moonX - 11.0f, moonY - 11.0f, 22.0f, 22.0f);
    g.setColour(juce::Colour(0xF20E0520));
    g.fillEllipse(moonX - 6.0f, moonY - 14.0f, 18.0f, 18.0f);

    struct Star { float x; float y; float r; float alpha; };
    Star stars[] = {
        {0.059f, 0.034f, 1.4f, 0.85f}, {0.143f, 0.016f, 1.0f, 0.65f},
        {0.230f, 0.055f, 1.2f, 0.75f}, {0.316f, 0.023f, 0.9f, 0.70f},
        {0.398f, 0.070f, 0.8f, 0.60f}, {0.471f, 0.013f, 1.5f, 0.90f},
        {0.549f, 0.044f, 0.9f, 0.65f}, {0.631f, 0.009f, 1.1f, 0.75f},
        {0.709f, 0.066f, 1.0f, 0.68f}, {0.784f, 0.028f, 1.3f, 0.80f},
        {0.862f, 0.053f, 0.8f, 0.55f}, {0.937f, 0.019f, 1.1f, 0.72f},
        {0.971f, 0.081f, 0.9f, 0.60f},
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
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawLine(w * 0.163f, h * 0.059f, w * 0.224f, h * 0.081f, 0.6f);
}

void BackgroundComponent::drawSun(juce::Graphics& g, float cx, float cy, float intensity)
{
    juce::ColourGradient halo(juce::Colour(0xFFFFB840).withAlpha(0.65f * intensity), cx, cy,
                              juce::Colours::transparentBlack, cx, cy - 210.0f, true);
    halo.addColour(0.25, juce::Colour(0xFFF07010).withAlpha(0.35f * intensity));
    halo.addColour(0.60, juce::Colour(0xFF882010).withAlpha(0.12f * intensity));
    g.setGradientFill(halo);
    g.fillEllipse(cx - 320.0f, cy - 210.0f, 640.0f, 420.0f);

    g.setColour(juce::Colour(0xFFFFAA30).withAlpha(0.055f * intensity));
    float rayAngles[] = { -1.2f, -0.6f, -0.1f, 0.3f, 0.8f, 1.1f };
    for (float angle : rayAngles)
        g.drawLine(cx, cy, cx + std::cos(angle) * 500.0f, cy + std::sin(angle) * -400.0f, 25.0f);

    juce::ColourGradient core(juce::Colour(0xFAFFFBE0), cx, cy,
                              juce::Colours::transparentBlack, cx + 42.0f, cy, true);
    core.addColour(0.30, juce::Colour(0xD9FFD060));
    core.addColour(0.65, juce::Colour(0x66FF9020));
    g.setGradientFill(core);
    g.fillEllipse(cx - 26.0f, cy - 26.0f, 52.0f, 52.0f);

    g.setColour(juce::Colour(0xFFFFCC40).withAlpha(0.30f * intensity));
    g.drawEllipse(cx - 32.0f, cy - 32.0f, 64.0f, 64.0f, 1.5f);
    g.setColour(juce::Colour(0xFFFFA830).withAlpha(0.15f * intensity));
    g.drawEllipse(cx - 42.0f, cy - 42.0f, 84.0f, 84.0f, 0.8f);
}

void BackgroundComponent::drawHalo(juce::Graphics& g, float cx, float cy, float alpha)
{
    juce::ColourGradient halo(juce::Colour(0xFFFFB840).withAlpha(alpha), cx, cy,
                              juce::Colours::transparentBlack, cx, cy - 210.0f, true);
    halo.addColour(0.4, juce::Colour(0xFFF07010).withAlpha(alpha * 0.5f));
    g.setGradientFill(halo);
    g.fillEllipse(cx - 320.0f, cy - 210.0f, 640.0f, 420.0f);
}

void BackgroundComponent::drawPyramids(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float baseY = h * 0.566f;

    {
        juce::Path darkFace;
        darkFace.startNewSubPath(w * 0.2735f, baseY);
        darkFace.lineTo(w * 0.4082f, h * 0.289f);
        darkFace.lineTo(w * 0.4388f, baseY);
        darkFace.closeSubPath();
        juce::ColourGradient darkGrad(juce::Colour(0xFA100602), w * 0.4082f, h * 0.289f,
                                       juce::Colour(0xFA1A0A04), w * 0.2735f, baseY, false);
        g.setGradientFill(darkGrad);
        g.fillPath(darkFace);

        juce::Path litFace;
        litFace.startNewSubPath(w * 0.4082f, h * 0.289f);
        litFace.lineTo(w * 0.5510f, baseY);
        litFace.lineTo(w * 0.4388f, baseY);
        litFace.closeSubPath();
        juce::ColourGradient litGrad(juce::Colour(0xF52A1406), w * 0.4082f, h * 0.289f,
                                      juce::Colour(0xF53E2010), w * 0.5510f, baseY, false);
        g.setGradientFill(litGrad);
        g.fillPath(litFace);

        g.setColour(juce::Colour(0x66503020));
        g.drawLine(w * 0.4082f, h * 0.289f, w * 0.4082f, baseY, 0.6f);
        g.setColour(juce::Colour(0xB3C8A060));
        g.fillEllipse(w * 0.4082f - 2.5f, h * 0.289f - 2.5f, 5.0f, 5.0f);

        juce::Path entrance;
        entrance.startNewSubPath(w * 0.365f, h * 0.50f);
        entrance.lineTo(w * 0.383f, h * 0.445f);
        entrance.lineTo(w * 0.400f, h * 0.50f);
        entrance.closeSubPath();
        g.setColour(juce::Colour(0xE6080300));
        g.fillPath(entrance);

        g.setColour(juce::Colour(0x4D1A0A02));
        for (float row = 0.1f; row < 0.9f; row += 0.1f)
        {
            float ly = h * 0.289f + (baseY - h * 0.289f) * row;
            float lx1 = w * 0.2735f + (w * 0.4082f - w * 0.2735f) * (1.0f - row) * 0.15f + (w * 0.4082f - w * 0.2735f) * row * 0.1f;
            float lx2 = w * 0.4082f + (w * 0.5510f - w * 0.4082f) * row * 0.9f;
            g.drawLine(lx1, ly, lx2, ly, 0.4f);
        }
    }
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

        juce::Path cap;
        cap.startNewSubPath(w * 0.649f, h * 0.323f);
        cap.lineTo(w * 0.674f, h * 0.375f);
        cap.lineTo(w * 0.624f, h * 0.375f);
        cap.closeSubPath();
        g.setColour(juce::Colour(0x598A7050));
        g.fillPath(cap);
    }
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

    g.setColour(juce::Colour(0xBF1A0C04));
    g.fillRect(w * 0.245f, baseY - 2.0f, w * 0.633f, 10.0f);
}

void BackgroundComponent::drawDunes(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float w = bounds.getWidth();
    float h = bounds.getHeight();

    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.606f);
        dune.quadraticTo(w * 0.092f, h * 0.531f, w * 0.209f, h * 0.588f);
        dune.quadraticTo(w * 0.327f, h * 0.644f, w * 0.457f, h * 0.544f);
        dune.quadraticTo(w * 0.584f, h * 0.444f, w * 0.702f, h * 0.563f);
        dune.quadraticTo(w * 0.820f, h * 0.681f, w, h * 0.575f);
        dune.lineTo(w, h * 0.722f); dune.lineTo(0, h * 0.722f); dune.closeSubPath();
        juce::ColourGradient grad(juce::Colour(0xC7C06818), 0, h * 0.531f,
                                   juce::Colour(0xC74A1E04), 0, h * 0.722f, false);
        g.setGradientFill(grad); g.fillPath(dune);
    }
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.669f);
        dune.quadraticTo(w * 0.087f, h * 0.602f, w * 0.202f, h * 0.656f);
        dune.quadraticTo(w * 0.318f, h * 0.711f, w * 0.461f, h * 0.622f);
        dune.quadraticTo(w * 0.598f, h * 0.533f, w * 0.735f, h * 0.644f);
        dune.quadraticTo(w * 0.869f, h * 0.750f, w, h * 0.653f);
        dune.lineTo(w, h * 0.828f); dune.lineTo(0, h * 0.828f); dune.closeSubPath();
        juce::ColourGradient grad(juce::Colour(0xD9D87820), 0, h * 0.602f,
                                   juce::Colour(0xD95E2608), 0, h * 0.828f, false);
        g.setGradientFill(grad); g.fillPath(dune);
    }
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.742f);
        dune.quadraticTo(w * 0.138f, h * 0.675f, w * 0.314f, h * 0.741f);
        dune.quadraticTo(w * 0.488f, h * 0.809f, w * 0.631f, h * 0.722f);
        dune.quadraticTo(w * 0.774f, h * 0.634f, w, h * 0.747f);
        dune.lineTo(w, h * 0.891f); dune.lineTo(0, h * 0.891f); dune.closeSubPath();
        juce::ColourGradient grad(juce::Colour(0xE6E89030), 0, h * 0.675f,
                                   juce::Colour(0xE66E3010), 0, h * 0.891f, false);
        g.setGradientFill(grad); g.fillPath(dune);
    }
    {
        juce::Path dune;
        dune.startNewSubPath(0, h * 0.825f);
        dune.quadraticTo(w * 0.179f, h * 0.773f, w * 0.386f, h * 0.825f);
        dune.quadraticTo(w * 0.574f, h * 0.875f, w * 0.755f, h * 0.806f);
        dune.quadraticTo(w * 0.886f, h * 0.756f, w, h * 0.820f);
        dune.lineTo(w, h); dune.lineTo(0, h); dune.closeSubPath();
        juce::ColourGradient grad(juce::Colour(0xF5CF7828), 0, h * 0.773f,
                                   juce::Colour(0xF5361404), 0, h, false);
        g.setGradientFill(grad); g.fillPath(dune);
    }

    g.setColour(juce::Colour(0xFC3A1C08));
    g.fillRect(0.0f, h * 0.930f, w, h * 0.07f);

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

    g.setColour(juce::Colour(0x2EE8A040));
    juce::Path crest1;
    crest1.startNewSubPath(0, h * 0.606f);
    crest1.quadraticTo(w * 0.092f, h * 0.531f, w * 0.209f, h * 0.588f);
    crest1.quadraticTo(w * 0.327f, h * 0.644f, w * 0.457f, h * 0.544f);
    crest1.quadraticTo(w * 0.584f, h * 0.444f, w * 0.702f, h * 0.563f);
    crest1.quadraticTo(w * 0.820f, h * 0.681f, w, h * 0.575f);
    g.strokePath(crest1, juce::PathStrokeType(0.8f));
}

// ════════════════════════════ SCENE — figures ════════════════════════════

void BackgroundComponent::drawSphinx(juce::Graphics& g, float sx, float sy)
{
    const float tx = 490.0f, ty = 350.0f;
    auto X = [&](float v) { return (tx + v) * sx; };
    auto Y = [&](float v) { return (ty + v) * sy; };
    auto E = [&](float cx, float cy, float rx, float ry, juce::Colour c)
    { g.setColour(c); g.fillEllipse(X(cx) - rx * sx, Y(cy) - ry * sy, 2 * rx * sx, 2 * ry * sy); };
    auto R = [&](float x, float y, float w, float h, juce::Colour c)
    { g.setColour(c); g.fillRoundedRectangle(X(x), Y(y), w * sx, h * sy, 3.0f); };

    E(0, 8, 55, 18, rgbA(0x1e1006, 0.92f));               // body
    R(-40, 14, 14, 20, rgbA(0x180c04, 0.9f));             // front legs
    R(-22, 14, 14, 20, rgbA(0x180c04, 0.9f));
    R(-10, -16, 22, 26, rgbA(0x1a0e06, 0.92f));           // neck
    E(2, -22, 18, 14, rgbA(0x1e1206, 0.94f));             // nemes head
    {   // nemes sides
        juce::Path l; l.startNewSubPath(X(-16), Y(-18)); l.quadraticTo(X(-22), Y(-10), X(-18), Y(4));
        l.lineTo(X(-12), Y(0)); l.quadraticTo(X(-14), Y(-8), X(-12), Y(-16)); l.closeSubPath();
        g.setColour(rgbA(0x160c04, 0.9f)); g.fillPath(l);
        juce::Path r; r.startNewSubPath(X(18), Y(-18)); r.quadraticTo(X(24), Y(-10), X(20), Y(4));
        r.lineTo(X(14), Y(0)); r.quadraticTo(X(16), Y(-8), X(14), Y(-16)); r.closeSubPath();
        g.setColour(rgbA(0x160c04, 0.9f)); g.fillPath(r);
    }
    E(2, -22, 12, 10, rgbA(0x221408, 0.9f));              // face
    E(-4, -24, 3, 1.5f, rgbA(0x060200, 0.95f));           // eyes
    E(8, -24, 3, 1.5f, rgbA(0x060200, 0.95f));
    R(-2, -12, 6, 10, rgbA(0x140a02, 0.8f));              // beard
    {   // uraeus (cobra)
        juce::Path u; u.startNewSubPath(X(2), Y(-33)); u.quadraticTo(X(6), Y(-30), X(4), Y(-26));
        u.quadraticTo(X(2), Y(-24), X(2), Y(-26)); u.quadraticTo(X(0), Y(-28), X(2), Y(-33)); u.closeSubPath();
        g.setColour(rgbA(0xc09020, 0.7f)); g.fillPath(u);
    }
}

void BackgroundComponent::drawCamel(juce::Graphics& g, float sx, float sy)
{
    const float tx = 700.0f, ty = 425.0f;
    auto X = [&](float v) { return (tx + v) * sx; };
    auto Y = [&](float v) { return (ty + v) * sy; };
    auto E = [&](float cx, float cy, float rx, float ry, juce::Colour c)
    { g.setColour(c); g.fillEllipse(X(cx) - rx * sx, Y(cy) - ry * sy, 2 * rx * sx, 2 * ry * sy); };
    auto R = [&](float x, float y, float w, float h, juce::Colour c)
    { g.setColour(c); g.fillRoundedRectangle(X(x), Y(y), w * sx, h * sy, 3.0f); };
    auto P = [&](std::initializer_list<float> pts, juce::Colour c, bool quad)
    {
        juce::Path p; auto it = pts.begin();
        float x0 = *it++, y0 = *it++; p.startNewSubPath(X(x0), Y(y0));
        while (it != pts.end())
        {
            if (quad) { float cx = *it++, cy = *it++, ex = *it++, ey = *it++; p.quadraticTo(X(cx), Y(cy), X(ex), Y(ey)); }
            else      { float ex = *it++, ey = *it++; p.lineTo(X(ex), Y(ey)); }
        }
        p.closeSubPath(); g.setColour(c); g.fillPath(p);
    };

    E(0, 0, 52, 28, rgbA(0x130900, 0.96f));   // body
    E(-10, -30, 20, 18, rgbA(0x130900, 0.96f)); // hump
    P({ 32,-12, 48,-8,56,-24, 60,-42,52,-48, 46,-40,48,-26, 42,-12,36,-10 }, rgbA(0x110800, 0.96f), true); // neck
    E(56, -50, 16, 11, rgbA(0x150a02, 0.96f)); // head
    P({ 62,-42, 70,-40,68,-36, 62,-35,58,-38 }, rgbA(0x1e1208, 0.96f), true); // lip
    E(52, -54, 2.2f, 2.2f, rgbA(0x060400, 0.96f));     // eye
    E(52.6f, -54.6f, 1, 1, rgbA(0xb08820, 0.9f));       // eye glint
    P({ 44,-58, 42,-68,48,-64, 50,-58,46,-54 }, rgbA(0x190a04, 0.96f), true); // ear
    P({ -50,-4, -62,-10,-60,-18, -54,-12,-52,-4 }, rgbA(0x130900, 0.96f), true); // tail
    // legs
    R(24, 20, 9, 16, rgbA(0x130900, 0.96f)); R(26, 36, 7, 18, rgbA(0x130900, 0.96f));
    R(36, 20, 9, 14, rgbA(0x130900, 0.96f)); R(38, 34, 7, 16, rgbA(0x130900, 0.96f));
    R(-22, 22, 9, 14, rgbA(0x130900, 0.96f)); R(-20, 36, 7, 16, rgbA(0x130900, 0.96f));
    R(-34, 22, 9, 12, rgbA(0x130900, 0.96f)); R(-32, 34, 7, 14, rgbA(0x130900, 0.96f));
    // hooves
    E(28, 55, 6, 3, rgbA(0x0a0500, 1.0f)); E(41, 51, 6, 3, rgbA(0x0a0500, 1.0f));
    E(-18, 53, 6, 3, rgbA(0x0a0500, 1.0f)); E(-29, 49, 6, 3, rgbA(0x0a0500, 1.0f));
    // saddle + rug + load
    P({ -14,-10, 0,-38,14,-10, 6,-8,-6,-8 }, rgbA(0x3a2010, 0.85f), true);
    E(0, -6, 16, 5, rgbA(0x502a14, 0.7f));
    R(-46, -8, 14, 10, rgbA(0x2e1808, 0.8f)); R(34, -8, 14, 10, rgbA(0x2e1808, 0.8f));
}

void BackgroundComponent::drawEagles(juce::Graphics& g, float sx, float sy)
{
    auto eagle = [&](float tx, float ty, float scale, float op)
    {
        auto X = [&](float v) { return (tx + v * scale) * sx; };
        auto Y = [&](float v) { return (ty + v * scale) * sy; };
        auto E = [&](float cx, float cy, float rx, float ry, juce::Colour c)
        { g.setColour(c); g.fillEllipse(X(cx) - rx * scale * sx, Y(cy) - ry * scale * sy,
                                        2 * rx * scale * sx, 2 * ry * scale * sy); };
        auto P = [&](std::initializer_list<float> pts, juce::Colour c)
        {
            juce::Path p; auto it = pts.begin();
            float x0 = *it++, y0 = *it++; p.startNewSubPath(X(x0), Y(y0));
            while (it != pts.end())
            { float cx = *it++, cy = *it++, ex = *it++, ey = *it++; p.quadraticTo(X(cx), Y(cy), X(ex), Y(ey)); }
            p.closeSubPath(); g.setColour(c); g.fillPath(p);
        };

        E(0, 0, 22, 9, rgbA(0x1c1208, op));            // body
        E(20, -5, 8, 8, rgbA(0x180e06, op));           // head
        P({ 26,-3, 33,0,31,5, 26,4,24,2 }, rgbA(0x2a2008, op)); // beak
        // wings (main)
        P({ -10,-5, -38,-24,-62,-20, -80,-16,-88,-9, -78,-7,-60,-14, -40,-18,-12,-3 }, rgbA(0x1a1206, op));
        P({ 12,-5, 40,-24,64,-20, 82,-16,90,-9, 80,-7,62,-14, 42,-18,14,-3 }, rgbA(0x1a1206, op));
        if (scale > 0.6f)
        {
            E(22, -6, 2.2f, 2.2f, rgbA(0x060402, op));  // eye
            E(22.6f, -6.6f, 1, 1, rgbA(0xc89820, op * 0.9f));
            // a couple of primaries + tail
            P({ -60,-20, -68,-30,-64,-36, -58,-26,-60,-20 }, rgbA(0x120c04, op));
            P({ 62,-20, 70,-30,66,-36, 60,-26,62,-20 }, rgbA(0x120c04, op));
            P({ -20,2, -32,9,-34,5, -28,0,-20,2 }, rgbA(0x1a1006, op));
        }
    };

    eagle(780.0f, 110.0f, 1.0f, 0.96f);   // main eagle
    eagle(192.0f, 88.0f, 0.42f, 0.5f);    // distant eagle
}

void BackgroundComponent::drawPalms(juce::Graphics& g, float sx, float sy)
{
    auto palm = [&](float tx, float ty, float trunkW, float trunkH, float frondLen, float op)
    {
        auto X = [&](float v) { return (tx + v) * sx; };
        auto Y = [&](float v) { return (ty + v) * sy; };
        juce::Colour c = rgbA(0x0a0500, op);
        g.setColour(c);
        g.fillRoundedRectangle(X(-trunkW * 0.5f), Y(0), trunkW * sx, trunkH * sy, trunkW * 0.5f * sx);
        float ang[] = { -2.4f, -1.9f, -1.57f, -1.2f, -0.7f };
        for (float a : ang)
        {
            juce::Path f;
            f.startNewSubPath(X(0), Y(0));
            float mx = std::cos(a) * frondLen * 0.7f, my = std::sin(a) * frondLen * 0.7f;
            float ex = std::cos(a) * frondLen, ey = std::sin(a) * frondLen - 6.0f;
            f.quadraticTo(X(mx), Y(my), X(ex), Y(ey));
            g.strokePath(f, juce::PathStrokeType(2.6f * sx, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));
        }
    };

    palm(820.0f, 340.0f, 6.0f, 28.0f, 24.0f, 0.85f);
    palm(840.0f, 344.0f, 5.0f, 22.0f, 19.0f, 0.75f);
    palm(808.0f, 348.0f, 4.0f, 18.0f, 15.0f, 0.65f);
}

void BackgroundComponent::drawMusicians(juce::Graphics& g, float sx, float sy)
{
    auto robe = [&](float tx, float ty, std::initializer_list<float> pts, juce::Colour c)
    {
        juce::Path p; auto it = pts.begin();
        float x0 = *it++, y0 = *it++; p.startNewSubPath((tx + x0) * sx, (ty + y0) * sy);
        while (it != pts.end())
        {
            float cx = *it++, cy = *it++, ex = *it++, ey = *it++;
            p.quadraticTo((tx + cx) * sx, (ty + cy) * sy, (tx + ex) * sx, (ty + ey) * sy);
        }
        p.closeSubPath(); g.setColour(c); g.fillPath(p);
    };
    auto circle = [&](float tx, float ty, float cx, float cy, float r, juce::Colour c)
    { g.setColour(c); g.fillEllipse((tx + cx) * sx - r * sx, (ty + cy) * sy - r * sy, 2 * r * sx, 2 * r * sy); };
    // Djellaba body: M0,0 Q(-a,b)(-c,d) L(c,d) Q(a,b) 0,0 Z
    auto djellaba = [&](float tx, float ty, float a, float b, float c, float d, juce::Colour col)
    {
        juce::Path p;
        p.startNewSubPath(tx * sx, ty * sy);
        p.quadraticTo((tx - a) * sx, (ty + b) * sy, (tx - c) * sx, (ty + d) * sy);
        p.lineTo((tx + c) * sx, (ty + d) * sy);
        p.quadraticTo((tx + a) * sx, (ty + b) * sy, tx * sx, ty * sy);
        p.closeSubPath(); g.setColour(col); g.fillPath(p);
    };
    auto stroke = [&](float tx, float ty, std::initializer_list<float> pts, float wd, juce::Colour c)
    {
        juce::Path p; auto it = pts.begin();
        float x0 = *it++, y0 = *it++; p.startNewSubPath((tx + x0) * sx, (ty + y0) * sy);
        while (it != pts.end())
        {
            float cx = *it++, cy = *it++, ex = *it++, ey = *it++;
            p.quadraticTo((tx + cx) * sx, (ty + cy) * sy, (tx + ex) * sx, (ty + ey) * sy);
        }
        g.setColour(c); g.strokePath(p, juce::PathStrokeType(wd * sx, juce::PathStrokeType::curved,
                                                             juce::PathStrokeType::rounded));
    };

    // ── M1 — Mâalem with guembri (100,462) ──
    djellaba(100, 462, 16, 48, 20, 96, rgbA(0x1a0c06, 0.95f));
    circle(100, 462, 0, -16, 11, rgbA(0x0c0502, 0.95f)); // head outer
    circle(100, 462, 0, -16, 9,  rgbA(0x1a0e08, 0.95f)); // face
    g.setColour(rgbA(0x2a1808, 0.95f));
    g.fillEllipse((100 - 10) * sx, (462 - 30) * sy, 20 * sx, 8 * sy); // turban
    circle(100, 462, -3, -17, 1.6f, rgbA(0x040200, 0.95f));
    circle(100, 462, 3, -17, 1.6f, rgbA(0x040200, 0.95f));
    {   // guembri at (62,500)
        float gx = 62.0f, gy = 500.0f;
        g.setColour(rgbA(0x2a1a0a, 0.95f)); g.fillEllipse((gx - 9) * sx, (gy - 12) * sy, 18 * sx, 24 * sy);
        g.setColour(rgbA(0x382210, 0.9f));  g.fillEllipse((gx - 7) * sx, (gy - 10) * sy, 14 * sx, 20 * sy);
        g.setColour(rgbA(0x1a1006, 0.95f)); g.fillEllipse((gx - 3) * sx, (gy - 1) * sy, 6 * sx, 6 * sy);
        g.setColour(rgbA(0x1e1206, 0.95f)); g.fillRoundedRectangle((gx - 2) * sx, (gy - 22) * sy, 4 * sx, 24 * sy, 2.0f);
        g.setColour(rgbA(0x7a6040, 0.8f));
        for (float dxs : { -2.0f, 0.0f, 2.0f })
            g.drawLine((gx + dxs) * sx, (gy - 22) * sy, (gx + dxs) * sx, (gy + 8) * sy, 0.6f);
    }
    stroke(100, 462, { -12,22, -28,28,-32,40 }, 4.0f, rgbA(0x1a0c06, 0.95f)); // arm to instrument

    // ── M2 — Qraqeb player (148,468) ──
    djellaba(148, 468, 14, 42, 18, 88, rgbA(0x140e08, 0.93f));
    circle(148, 468, 0, -15, 10, rgbA(0x1a0e08, 0.93f));
    g.setColour(rgbA(0x8a1808, 0.93f));
    g.fillEllipse((148 - 9) * sx, (468 - 31) * sy, 18 * sx, 8 * sy); // chéchia
    stroke(148, 468, { -10,16, -24,6,-26,-4 }, 4.0f, rgbA(0x140e08, 0.93f));
    stroke(148, 468, { 10,16, 24,6,26,-4 }, 4.0f, rgbA(0x140e08, 0.93f));
    for (float qx : { -26.0f, 26.0f }) // qraqeb pairs
    {
        circle(148, 468, qx - 4, -6, 5.5f, rgbA(0x2a2418, 0.95f));
        circle(148, 468, qx + 4, -6, 5.5f, rgbA(0x2a2418, 0.95f));
    }

    // ── M3 — Dancer in trance (192,472) ──
    djellaba(192, 472, 12, 36, 16, 80, rgbA(0x180a04, 0.91f));
    circle(192, 472, 0, -14, 9.5f, rgbA(0x1a0e08, 0.91f));
    robe(192, 472, { -8,-18, 0,-38,8,-18, 0,-20,-8,-18 }, rgbA(0x601808, 0.91f)); // pointy hat
    stroke(192, 472, { -10,14, -30,8,-34,2 }, 6.0f, rgbA(0x180a04, 0.91f));
    stroke(192, 472, { 10,14, 26,20,30,16 }, 6.0f, rgbA(0x180a04, 0.91f));
    circle(192, 472, -6, 20, 2, rgbA(0x801808, 0.8f));
    circle(192, 472, 0, 15, 2, rgbA(0x801808, 0.8f));
    circle(192, 472, 6, 20, 2, rgbA(0x801808, 0.8f));

    // ── M4 — 2nd guembri player (234,476) ──
    djellaba(234, 476, 11, 32, 14, 72, rgbA(0x160c06, 0.87f));
    circle(234, 476, 0, -13, 9, rgbA(0x1a0e08, 0.87f));
    robe(234, 476, { -9,-18, 0,-32,9,-18, 0,-18,-9,-18 }, rgbA(0x1e1006, 0.87f));
    stroke(234, 476, { -10,14, -22,20,-24,28 }, 5.0f, rgbA(0x160c06, 0.87f));
    stroke(234, 476, { 10,14, 20,18,22,26 }, 5.0f, rgbA(0x160c06, 0.87f));
}

void BackgroundComponent::drawHeatHaze(juce::Graphics& g, float sx, float sy)
{
    g.setColour(rgbA(0xc06010, 0.06f));
    g.fillRect(0.0f, 340.0f * sy, 980.0f * sx, 28.0f * sy);
    g.setColour(rgbA(0xd07818, 0.08f));
    g.fillRect(0.0f, 600.0f * sy, 980.0f * sx, 40.0f * sy);
}

void BackgroundComponent::drawVignette(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float radius = bounds.getWidth() * 0.7f;

    juce::ColourGradient vig(juce::Colours::transparentBlack, cx, cy,
                              juce::Colour(0xA6000000), cx + radius, cy, true);
    vig.addColour(0.7, juce::Colours::transparentBlack);
    g.setGradientFill(vig);
    g.fillRect(bounds);
}
