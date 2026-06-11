#pragma once
#include <JuceHeader.h>
#include "../Utils/Constants.h"

/**
    Disk-based multisample player.

    Scans  <Documents>/Oriental Instrument/Samples/<InstrumentName>/  for .wav
    (and .aiff/.flac) files, maps each across the keyboard from its detected root
    note, and plays them back with juce::SamplerVoice. Lets the user reach the
    realism of a sampled library (Taqsim-style) without recompiling — drop new
    wavs in the folder and reload the plugin.

    Root-note detection from the filename, in order:
      1. a note name token   — "Oud_A2.wav", "ney-F#4.wav", "qanun C3.wav"
      2. a midi-number token — "Oud_57.wav", "darbuka_60.wav"
      3. fallback            — single file stretched across the whole keyboard.

    When a folder has no samples, hasSamples() is false and the processor falls
    back to the synthesis engine, so the plugin always makes sound.
*/
class SampleEngine
{
public:
    SampleEngine();

    void prepare(double sampleRate, int samplesPerBlock);

    // Load the wavs for an instrument (no-op if already loaded).
    void setInstrument(OrientalConstants::Instrument inst);

    bool hasSamples() const { return currentHasSamples; }

    void render(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi,
                int startSample, int numSamples);

    void allNotesOff();

    // <Documents>/Oriental Instrument/Samples  (created on first use).
    static juce::File samplesRoot();

private:
    void loadFolder(const juce::File& dir);
    static int parseRootNote(const juce::String& fileNameNoExt);

    juce::Synthesiser synth;
    juce::AudioFormatManager formatManager;

    double sampleRate = 44100.0;
    int    blockSize  = 512;
    bool   currentHasSamples = false;
    int    loadedInstrument  = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleEngine)
};
