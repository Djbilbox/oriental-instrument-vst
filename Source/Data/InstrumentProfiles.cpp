#include "InstrumentProfiles.h"

using Inst = OrientalConstants::Instrument;
using Wave = InstrumentProfile::WaveType;

const std::vector<InstrumentProfile> InstrumentProfiles::profiles = InstrumentProfiles::createProfiles();

std::vector<InstrumentProfile> InstrumentProfiles::createProfiles()
{
    std::vector<InstrumentProfile> p;
    p.reserve(OrientalConstants::NUM_INSTRUMENTS);

    // ── VIOLIN — bowed, rich saw, warm legato, strong vibrato ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Violin;  v.waveType = Wave::Sawtooth;
        v.vibratoRate = 5.5f; v.vibratoDepth = 0.012f;
        v.attack = 0.12f; v.decay = 0.10f; v.sustain = 0.78f; v.release = 0.45f;
        v.filterFreq = 4200.0f; v.filterQ = 1.2f;
        v.harmonics = { 1.0f, 0.78f, 0.55f, 0.42f, 0.32f, 0.24f, 0.18f, 0.13f, 0.09f, 0.06f };
        v.drive = 1.45f; v.driveAsym = 0.14f; v.subLevel = 0.10f;
        v.detuneCents = 7.0f; v.unisonMix = 0.55f;
        v.noiseAmount = 0.05f; v.noiseColor = 0.5f; v.noiseDecay = 0.30f; v.noiseSustains = true;
        v.filterEnvAmt = 1.2f; v.filterEnvDecay = 0.35f; v.filterEnvSust = 0.45f;
        v.keyTrack = 0.5f; v.velToFilter = 0.7f; v.velToDrive = 0.6f;
        v.formant1 = { 550.0f, 3.5f, 0.30f };   // bridge resonance
        v.formant2 = { 1100.0f, 4.0f, 0.20f };
        p.push_back(v);
    }

    // ── OUD — plucked, woody body, bright transient then warm decay ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Oud;  v.waveType = Wave::Triangle;
        v.vibratoRate = 0.0f; v.vibratoDepth = 0.0f;
        v.attack = 0.004f; v.decay = 0.45f; v.sustain = 0.30f; v.release = 0.80f;
        v.filterFreq = 3000.0f; v.filterQ = 1.6f;
        v.harmonics = { 1.0f, 0.62f, 0.45f, 0.34f, 0.26f, 0.18f, 0.12f, 0.08f };
        v.drive = 1.4f; v.driveAsym = 0.18f; v.subLevel = 0.18f;
        v.detuneCents = 4.0f; v.unisonMix = 0.35f;       // courses slightly detuned
        v.noiseAmount = 0.15f; v.noiseColor = 0.75f; v.noiseDecay = 0.05f; v.noiseSustains = false; // pluck chiff
        v.filterEnvAmt = 2.6f; v.filterEnvDecay = 0.40f; v.filterEnvSust = 0.10f; // bright attack → dark
        v.keyTrack = 0.45f; v.velToFilter = 1.1f; v.velToDrive = 0.7f;
        v.formant1 = { 320.0f, 2.5f, 0.45f };   // soundbox
        v.formant2 = { 900.0f, 3.0f, 0.28f };
        p.push_back(v);
    }

    // ── NEY — breathy end-blown flute, hollow, slow vibrato ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Ney;  v.waveType = Wave::Sine;
        v.vibratoRate = 6.0f; v.vibratoDepth = 0.018f;
        v.attack = 0.09f; v.decay = 0.06f; v.sustain = 0.82f; v.release = 0.55f;
        v.filterFreq = 2400.0f; v.filterQ = 0.9f;
        v.harmonics = { 1.0f, 0.10f, 0.32f, 0.06f, 0.14f, 0.04f, 0.06f }; // odd-dominant, hollow
        v.drive = 1.15f; v.driveAsym = 0.08f; v.subLevel = 0.05f;
        v.detuneCents = 3.0f; v.unisonMix = 0.25f;
        v.noiseAmount = 0.26f; v.noiseColor = 0.45f; v.noiseDecay = 0.20f; v.noiseSustains = true; // sustained breath
        v.filterEnvAmt = 0.8f; v.filterEnvDecay = 0.30f; v.filterEnvSust = 0.60f;
        v.keyTrack = 0.6f; v.velToFilter = 0.5f; v.velToDrive = 0.3f;
        v.formant1 = { 800.0f, 2.0f, 0.25f };   // air column
        v.formant2 = { 2000.0f, 2.5f, 0.15f };
        p.push_back(v);
    }

    // ── QANUN — bright plucked zither, metallic shimmer ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Qanun;  v.waveType = Wave::Triangle;
        v.vibratoRate = 0.0f; v.vibratoDepth = 0.0f;
        v.attack = 0.003f; v.decay = 0.55f; v.sustain = 0.22f; v.release = 0.70f;
        v.filterFreq = 5400.0f; v.filterQ = 2.0f;
        v.harmonics = { 1.0f, 0.7f, 0.55f, 0.45f, 0.38f, 0.30f, 0.24f, 0.18f, 0.13f, 0.09f };
        v.drive = 1.3f; v.driveAsym = 0.12f; v.subLevel = 0.06f;
        v.detuneCents = 9.0f; v.unisonMix = 0.5f;        // triple-string courses
        v.noiseAmount = 0.12f; v.noiseColor = 0.9f; v.noiseDecay = 0.03f; v.noiseSustains = false;
        v.filterEnvAmt = 2.2f; v.filterEnvDecay = 0.50f; v.filterEnvSust = 0.08f;
        v.keyTrack = 0.5f; v.velToFilter = 1.0f; v.velToDrive = 0.6f;
        v.formant1 = { 1200.0f, 3.0f, 0.30f };
        v.formant2 = { 2600.0f, 3.5f, 0.20f };
        p.push_back(v);
    }

    // ── DARBUKA — percussive membrane, pitched thump ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Darbuka;  v.waveType = Wave::Sine;
        v.vibratoRate = 0.0f; v.vibratoDepth = 0.0f;
        v.attack = 0.001f; v.decay = 0.16f; v.sustain = 0.0f; v.release = 0.20f;
        v.filterFreq = 1200.0f; v.filterQ = 1.4f;
        v.harmonics = { 1.0f, 0.5f, 0.28f, 0.16f };
        v.drive = 2.1f; v.driveAsym = 0.20f; v.subLevel = 0.45f;
        v.detuneCents = 0.0f; v.unisonMix = 0.0f;
        v.noiseAmount = 0.5f; v.noiseColor = 0.8f; v.noiseDecay = 0.02f; v.noiseSustains = false; // slap/tek
        v.filterEnvAmt = 3.0f; v.filterEnvDecay = 0.06f; v.filterEnvSust = 0.0f;  // sharp click
        v.keyTrack = 0.3f; v.velToFilter = 1.4f; v.velToDrive = 1.0f;
        v.formant1 = { 180.0f, 1.5f, 0.40f };   // body boom
        v.formant2 = { 0.0f, 1.0f, 0.0f };
        p.push_back(v);
    }

    // ── RABABAH — bowed spike fiddle, raspy and reedy ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Rababah;  v.waveType = Wave::Sawtooth;
        v.vibratoRate = 4.5f; v.vibratoDepth = 0.016f;
        v.attack = 0.14f; v.decay = 0.10f; v.sustain = 0.72f; v.release = 0.55f;
        v.filterFreq = 2800.0f; v.filterQ = 1.4f;
        v.harmonics = { 1.0f, 0.82f, 0.6f, 0.48f, 0.38f, 0.30f, 0.22f, 0.16f, 0.10f };
        v.drive = 1.7f; v.driveAsym = 0.18f; v.subLevel = 0.12f;   // reedy, not fuzzy
        v.detuneCents = 8.0f; v.unisonMix = 0.5f;
        v.noiseAmount = 0.20f; v.noiseColor = 0.55f; v.noiseDecay = 0.30f; v.noiseSustains = true; // bow scrape
        v.filterEnvAmt = 1.0f; v.filterEnvDecay = 0.35f; v.filterEnvSust = 0.5f;
        v.keyTrack = 0.5f; v.velToFilter = 0.8f; v.velToDrive = 0.8f;
        v.formant1 = { 480.0f, 3.0f, 0.35f };
        v.formant2 = { 1500.0f, 3.5f, 0.25f };
        p.push_back(v);
    }

    // ── MIZMAR — double-reed shawm, piercing and nasal ──
    {
        InstrumentProfile v;
        v.instrument = Inst::Mizmar;  v.waveType = Wave::Square;
        v.vibratoRate = 5.0f; v.vibratoDepth = 0.02f;
        v.attack = 0.05f; v.decay = 0.08f; v.sustain = 0.74f; v.release = 0.35f;
        v.filterFreq = 3400.0f; v.filterQ = 1.8f;
        v.harmonics = { 1.0f, 0.4f, 0.7f, 0.3f, 0.5f, 0.22f, 0.35f, 0.16f, 0.22f }; // strong odd → nasal
        v.drive = 1.9f; v.driveAsym = 0.22f; v.subLevel = 0.0f;     // bright, reedy
        v.detuneCents = 5.0f; v.unisonMix = 0.4f;
        v.noiseAmount = 0.16f; v.noiseColor = 0.7f; v.noiseDecay = 0.18f; v.noiseSustains = true;
        v.filterEnvAmt = 1.4f; v.filterEnvDecay = 0.25f; v.filterEnvSust = 0.55f;
        v.keyTrack = 0.55f; v.velToFilter = 0.9f; v.velToDrive = 0.9f;
        v.formant1 = { 1400.0f, 4.0f, 0.40f };  // nasal honk
        v.formant2 = { 2800.0f, 4.5f, 0.28f };
        p.push_back(v);
    }

    return p;
}

const InstrumentProfile& InstrumentProfiles::getProfile(OrientalConstants::Instrument inst)
{
    return profiles[static_cast<int>(inst)];
}

const std::vector<InstrumentProfile>& InstrumentProfiles::getAllProfiles()
{
    return profiles;
}
