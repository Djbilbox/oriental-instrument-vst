#include "InstrumentProfiles.h"

using Inst = OrientalConstants::Instrument;
using Wave = InstrumentProfile::WaveType;

const std::vector<InstrumentProfile> InstrumentProfiles::profiles = InstrumentProfiles::createProfiles();

std::vector<InstrumentProfile> InstrumentProfiles::createProfiles()
{
    std::vector<InstrumentProfile> p;
    p.reserve(OrientalConstants::NUM_INSTRUMENTS);

    // Violin: sawtooth, vibrato, warm legato
    p.push_back({ Inst::Violin, Wave::Sawtooth,
                  5.5f, 0.012f,
                  0.12f, 0.1f, 0.75f, 0.45f,
                  3800.0f, 1.2f,
                  { 1.0f, 0.7f, 0.4f, 0.25f, 0.15f } });

    // Oud: triangle, plucked, no vibrato
    p.push_back({ Inst::Oud, Wave::Triangle,
                  0.0f, 0.0f,
                  0.006f, 0.4f, 0.35f, 0.8f,
                  2800.0f, 1.8f,
                  { 1.0f, 0.5f, 0.3f, 0.2f, 0.1f, 0.06f } });

    // Ney: sine, breathy, slow vibrato
    p.push_back({ Inst::Ney, Wave::Sine,
                  6.0f, 0.018f,
                  0.08f, 0.05f, 0.8f, 0.6f,
                  2200.0f, 0.8f,
                  { 1.0f, 0.3f, 0.15f, 0.08f } });

    // Qanun: triangle, bright pluck
    p.push_back({ Inst::Qanun, Wave::Triangle,
                  0.0f, 0.0f,
                  0.004f, 0.5f, 0.25f, 1.0f,
                  5000.0f, 2.2f,
                  { 1.0f, 0.6f, 0.4f, 0.3f, 0.2f, 0.1f } });

    // Darbuka: sine, percussive
    p.push_back({ Inst::Darbuka, Wave::Sine,
                  0.0f, 0.0f,
                  0.002f, 0.18f, 0.05f, 0.25f,
                  800.0f, 3.0f,
                  { 1.0f, 0.4f, 0.2f } });

    // Rababah: sawtooth, bowed, vibrato
    p.push_back({ Inst::Rababah, Wave::Sawtooth,
                  4.5f, 0.015f,
                  0.15f, 0.1f, 0.7f, 0.55f,
                  2600.0f, 1.4f,
                  { 1.0f, 0.6f, 0.35f, 0.2f, 0.1f } });

    // Mizmar: square, reed, bright
    p.push_back({ Inst::Mizmar, Wave::Square,
                  5.0f, 0.02f,
                  0.05f, 0.08f, 0.72f, 0.35f,
                  3200.0f, 1.6f,
                  { 1.0f, 0.5f, 0.25f, 0.12f } });

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
