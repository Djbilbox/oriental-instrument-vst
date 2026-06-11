#include "PluginEditor.h"
#include "GUI/Typography.h"

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

    // ── Header: live preset name ──
    presetNameLabel.setJustificationType(juce::Justification::centredRight);
    presetNameLabel.setColour(juce::Label::textColourId, juce::Colour(Colors::GOLD_LIGHT));
    presetNameLabel.setFont(Typography::display(11.0f, true));
    addAndMakeVisible(presetNameLabel);

    // ── Header: transport buttons ──
    auto setupBtn = [this](juce::TextButton& b, const juce::String& text, bool toggle)
    {
        b.setButtonText(text);
        b.setName(text);                 // LookAndFeel colour-codes REC/PANIC by name
        b.setClickingTogglesState(toggle);
        addAndMakeVisible(b);
    };
    setupBtn(btnMidi,  "MIDI",  true);
    setupBtn(btnPoly,  "POLY",  true);
    setupBtn(btnLeg,   "LEG",   true);
    setupBtn(btnAB,    "A/B",   true);
    setupBtn(btnRec,   "REC",   true);
    setupBtn(btnPanic, "PANIC", false);

    btnPoly.setToggleState(true, juce::dontSendNotification); // polyphonic by default

    // Functional wiring (MIDI / A-B / REC are cosmetic toggles for now)
    btnPoly.onClick  = [this] { processorRef.setMonoMode(! btnPoly.getToggleState()); };
    btnLeg.onClick   = [this] { processorRef.setLegato(btnLeg.getToggleState()); };
    btnPanic.onClick = [this] { processorRef.panicAllNotes(); };

    updatePresetLabel();
    startTimerHz(6);
}

void OrientalInstrumentEditor::timerCallback()
{
    updatePresetLabel();
}

void OrientalInstrumentEditor::updatePresetLabel()
{
    const auto& p = processorRef.getPresetManager().getCurrentPreset();
    juce::String txt = p.name + "  \xC2\xB7  " + juce::String(instrumentName(p.instrument));
    if (p.key.isNotEmpty())
        txt += " \xC2\xB7 " + p.key;
    if (presetNameLabel.getText() != txt)
        presetNameLabel.setText(txt, juce::dontSendNotification);
}

OrientalInstrumentEditor::~OrientalInstrumentEditor()
{
    stopTimer();
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

    // Header area (top 50px) — chrome painted by BackgroundComponent; lay out
    // the interactive children (preset name + transport buttons) on top.
    auto headerArea = bounds.removeFromTop(HEADER_HEIGHT);
    {
        const int h = 18, y = headerArea.getCentreY() - h / 2, gap = 4;
        const int totalBtnW = 34 + 34 + 28 + 30 + 30 + 42 + 5 * gap;
        int x = headerArea.getRight() - 10 - totalBtnW;
        auto place = [&](juce::TextButton& b, int w) { b.setBounds(x, y, w, h); x += w + gap; };
        place(btnMidi, 34); place(btnPoly, 34); place(btnLeg, 28);
        place(btnAB, 30);   place(btnRec, 30);  place(btnPanic, 42);

        const int labelW = 150;
        presetNameLabel.setBounds(headerArea.getRight() - 10 - totalBtnW - 10 - labelW,
                                  y - 2, labelW, h + 4);
    }

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
