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

void OrientalInstrumentEditor::paint(juce::Graphics&)
{
    // All backdrop, glass relief and chrome are rendered by BackgroundComponent,
    // which sits behind every control. The editor itself paints nothing — its
    // graphics would otherwise be hidden under that child component.
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
