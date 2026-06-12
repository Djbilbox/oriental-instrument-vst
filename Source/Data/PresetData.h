#pragma once
#include <JuceHeader.h>
#include "../Utils/Constants.h"

struct PresetData
{
    juce::String name;
    juce::String city;
    juce::String key;
    juce::String maqam;
    juce::String soundDescription;
    juce::String tags;
    OrientalConstants::Instrument instrument;

    // Relative path under <Documents>/Oriental Instrument/Samples that supplies
    // THIS preset's source samples. Empty = use the instrument's default folder.
    // Lets two presets of the same instrument pull different source material
    // (e.g. "Oud Sample Pack/Oud Sample Pack" vs top-level "Oud") so they have a
    // genuinely distinct sonic identity, not just different knob values.
    juce::String sampleFolder;

    // Synth parameters
    float glide   = 25.0f;
    float space   = 60.0f;
    float filter  = 80.0f;
    float orient  = 50.0f;
    float reso    = 35.0f;
    float depth   = 70.0f;
    float fxMix   = 50.0f;

    // ADSR — indépendants par preset (en secondes / 0-1 pour sustain)
    float attack  = 0.10f;
    float decay   = 0.10f;
    float sustain = 0.70f;
    float release = 0.30f;

    // FX on/off states
    bool reverbOn     = true;
    bool echoOn       = false;
    bool chorusOn     = true;
    bool distortOn    = false;
    bool compressorOn = true;
    bool eqOn         = false;
    bool phaserOn     = false;
    bool bitcrushOn   = false;

    // FX amounts
    float reverbAmount     = 0.72f;
    float echoAmount       = 0.45f;
    float chorusAmount     = 0.38f;
    float distortAmount    = 0.18f;
    float compressorAmount = 0.60f;
    float phaserAmount     = 0.30f;
    float bitcrushAmount   = 0.08f;
};
