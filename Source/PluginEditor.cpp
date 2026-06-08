#include "PluginEditor.h"

using namespace OrientalConstants;

OrientalInstrumentEditor::OrientalInstrumentEditor(OrientalInstrumentProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      presetBrowser(p.getPresetManager(), p.getAPVTS()),
      fxPanel(p.getFXChain(), p.getAPVTS()),
      piano(p.getSynthesiser()),
      waveform(),
      pitchWheel(WheelComponent::Type::PitchBend, "PITCH"),
      modWheel(WheelComponent::Type::ModWheel, "MOD"),
      knobGlide("GLIDE"),
      knobSpace("SPACE"),
      knobFilter("FILTER"),
      knobOrient("ORIENT"),
      knobReso("RESO"),
      knobDepth("DEPTH"),
      knobFxMix("FX MIX")
{
    setLookAndFeel(&orientalLnf);
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    addAndMakeVisible(background);
    addAndMakeVisible(presetBrowser);
    addAndMakeVisible(fxPanel);
    addAndMakeVisible(piano);
    addAndMakeVisible(waveform);
    addAndMakeVisible(pitchWheel);
    addAndMakeVisible(modWheel);

    addAndMakeVisible(knobGlide);
    addAndMakeVisible(knobSpace);
    addAndMakeVisible(knobFilter);
    addAndMakeVisible(knobOrient);
    addAndMakeVisible(knobReso);
    addAndMakeVisible(knobDepth);
    addAndMakeVisible(knobFxMix);

    // Connect wheels directly to processor DSP
    pitchWheel.onValueChange = [this](float value) { processorRef.setPitchBend(value); };
    modWheel.onValueChange = [this](float value) { processorRef.setModWheel(value); };

    // Attach APVTS parameters to knob sliders
    auto& apvts = p.getAPVTS();
    glideAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "glide", knobGlide.getSlider());
    spaceAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "space", knobSpace.getSlider());
    filterAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filter", knobFilter.getSlider());
    orientAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "orient", knobOrient.getSlider());
    resoAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reso", knobReso.getSlider());
    depthAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "depth", knobDepth.getSlider());
    fxMixAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "fxmix", knobFxMix.getSlider());
}

OrientalInstrumentEditor::~OrientalInstrumentEditor()
{
    setLookAndFeel(nullptr);
}

void OrientalInstrumentEditor::paint(juce::Graphics& g)
{
    // Background is drawn by BackgroundComponent (z-order behind everything)

    auto bounds = getLocalBounds();

    // ── HEADER glass panel: rgba(6,4,2,0.93), border-bottom: 1px solid var(--gold-dim) ──
    auto headerBounds = bounds.removeFromTop(HEADER_HEIGHT).toFloat();
    g.setColour(juce::Colour(Colors::HEADER_BG));
    g.fillRect(headerBounds);
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.drawLine(headerBounds.getX(), headerBounds.getBottom(),
               headerBounds.getRight(), headerBounds.getBottom(), 1.0f);

    // Header title: "ORIENTAL INSTRUMENT — MAQAM EDITION"
    g.setColour(juce::Colour(Colors::GOLD));
    g.setFont(juce::Font("Cinzel", 17.0f, juce::Font::bold));
    g.drawText("ORIENTAL INSTRUMENT", headerBounds, juce::Justification::centred);

    // Logo left
    g.setFont(juce::Font("Cinzel", 11.0f, juce::Font::bold));
    g.drawText("DJBILBOX", headerBounds.withWidth(140.0f).translated(14.0f, -6.0f),
               juce::Justification::centredLeft);
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.setFont(juce::Font("Cinzel", 8.0f, juce::Font::plain));
    g.drawText("MAQAM ENGINE PRO", headerBounds.withWidth(140.0f).translated(14.0f, 8.0f),
               juce::Justification::centredLeft);

    // Remove piano area for glass panels calculation
    auto pianoAreaRef = bounds.removeFromBottom(PIANO_HEIGHT);
    (void)pianoAreaRef;

    // ── LEFT COLUMN glass: var(--glass-bg) 78%, border-right ──
    auto leftBounds = bounds.removeFromLeft(LEFT_COL_WIDTH).toFloat();
    g.setColour(juce::Colour(Colors::GLASS_BG));
    g.fillRect(leftBounds);
    g.setColour(juce::Colour(Colors::GLASS_BORDER));
    g.drawLine(leftBounds.getRight(), leftBounds.getY(),
               leftBounds.getRight(), leftBounds.getBottom(), 1.0f);

    // Left column label: "MACRO"
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.setFont(juce::Font("Cinzel", 9.0f, juce::Font::bold));
    g.drawText("MACRO", leftBounds.removeFromTop(18.0f), juce::Justification::centred);

    // ── RIGHT COLUMN glass ──
    auto rightBounds = bounds.removeFromRight(RIGHT_COL_WIDTH).toFloat();
    g.setColour(juce::Colour(Colors::GLASS_BG));
    g.fillRect(rightBounds);
    g.setColour(juce::Colour(Colors::GLASS_BORDER));
    g.drawLine(rightBounds.getX(), rightBounds.getY(),
               rightBounds.getX(), rightBounds.getBottom(), 1.0f);

    // ── CENTER glass ──
    auto centerBounds = bounds.toFloat();
    g.setColour(juce::Colour(Colors::GLASS_BG));
    g.fillRect(centerBounds);

    // ── PIANO border top: 1px solid var(--gold-dim) ──
    float pianoTop = static_cast<float>(getHeight() - PIANO_HEIGHT);
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.drawLine(0.0f, pianoTop, static_cast<float>(getWidth()), pianoTop, 1.0f);

    // Plugin border: 1px solid var(--gold-dim) + outer shadow
    g.setColour(juce::Colour(Colors::GOLD_DIM));
    g.drawRect(getLocalBounds(), 1);
}

void OrientalInstrumentEditor::resized()
{
    auto bounds = getLocalBounds();

    background.setBounds(bounds);

    // Header area (top 50px) — painted in paint(), no child component
    bounds.removeFromTop(HEADER_HEIGHT);

    // Piano area (bottom 148px) with wheels on left/right
    auto pianoArea = bounds.removeFromBottom(PIANO_HEIGHT);
    auto pitchWheelArea = pianoArea.removeFromLeft(54);
    auto modWheelArea = pianoArea.removeFromRight(54);
    pitchWheel.setBounds(pitchWheelArea);
    modWheel.setBounds(modWheelArea);
    piano.setBounds(pianoArea);

    // Main area: left knobs | center browser | right FX
    auto mainArea = bounds;

    auto leftCol = mainArea.removeFromLeft(LEFT_COL_WIDTH);
    auto rightCol = mainArea.removeFromRight(RIGHT_COL_WIDTH);
    auto centerCol = mainArea;

    // Layout 6 knobs in left column (skip top 20px for "MACRO" label)
    leftCol.removeFromTop(20);
    int knobHeight = leftCol.getHeight() / 6;
    knobGlide.setBounds(leftCol.removeFromTop(knobHeight));
    knobSpace.setBounds(leftCol.removeFromTop(knobHeight));
    knobFilter.setBounds(leftCol.removeFromTop(knobHeight));
    knobOrient.setBounds(leftCol.removeFromTop(knobHeight));
    knobReso.setBounds(leftCol.removeFromTop(knobHeight));
    knobDepth.setBounds(leftCol.removeFromTop(knobHeight));

    // Center: waveform on top (26px), preset browser below
    auto waveArea = centerCol.removeFromTop(static_cast<int>(Design::WAVEFORM_HEIGHT) + 4);
    waveform.setBounds(waveArea.reduced(4, 2));
    presetBrowser.setBounds(centerCol.reduced(2));

    // Right column: FX panel + FX mix knob at bottom
    auto fxMixArea = rightCol.removeFromBottom(70);
    knobFxMix.setBounds(fxMixArea);
    fxPanel.setBounds(rightCol);
}
