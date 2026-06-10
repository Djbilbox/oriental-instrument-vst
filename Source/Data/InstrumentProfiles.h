#pragma once
#include <JuceHeader.h>
#include "../Utils/Constants.h"

// A formant / body-resonance peak (parallel bandpass).
struct FormantPeak
{
    float freq;  // Hz
    float q;     // resonance
    float gain;  // mix amount (0 = off)
};

struct InstrumentProfile
{
    OrientalConstants::Instrument instrument;
    enum class WaveType { Sine, Sawtooth, Triangle, Square } waveType;

    // Vibrato
    float vibratoRate;
    float vibratoDepth;

    // Amp ADSR (fallback when no preset ADSR loaded)
    float attack;
    float decay;
    float sustain;
    float release;

    // Base filter
    float filterFreq;
    float filterQ;

    // Oscillator spectrum
    std::vector<float> harmonics;

    // ── Character / grain (new engine) ──
    float drive          = 1.0f;  // base waveshaper drive (1 = clean, >1 = saturated)
    float driveAsym      = 0.12f; // asymmetry → even harmonics (tube-like warmth)
    float subLevel       = 0.0f;  // sub-oscillator (one octave down) level 0..1
    float detuneCents    = 6.0f;  // unison spread in cents
    float unisonMix      = 0.5f;  // how loud the detuned voices sit vs centre

    // Noise / breath / bow layer
    float noiseAmount    = 0.0f;  // 0..1 level mixed before drive
    float noiseColor     = 0.5f;  // 0 = dark, 1 = bright (one-pole shaping)
    float noiseDecay     = 0.15f; // seconds — its own little decay (attack transient)
    bool  noiseSustains  = false; // true = breath stays (ney), false = pluck chiff

    // Filter envelope (modulates SVF cutoff)
    float filterEnvAmt   = 1.5f;  // octaves the env opens the cutoff
    float filterEnvDecay = 0.25f; // seconds
    float filterEnvSust  = 0.25f; // 0..1 sustained opening
    float keyTrack       = 0.4f;  // 0..1 cutoff follows pitch
    float velToFilter    = 0.6f;  // velocity → cutoff (octaves at full velocity)
    float velToDrive     = 0.5f;  // velocity → extra drive

    // Body resonance
    FormantPeak formant1 { 0.0f, 1.0f, 0.0f };
    FormantPeak formant2 { 0.0f, 1.0f, 0.0f };
};

class InstrumentProfiles
{
public:
    static const InstrumentProfile& getProfile(OrientalConstants::Instrument inst);
    static const std::vector<InstrumentProfile>& getAllProfiles();

private:
    static std::vector<InstrumentProfile> createProfiles();
    static const std::vector<InstrumentProfile> profiles;
};
