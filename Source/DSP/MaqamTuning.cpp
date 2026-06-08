#include "MaqamTuning.h"

// Maqam scales with quarter-tone deviations from 12-TET (in cents)
// Each entry = deviation for scale degree 1-8 relative to root
const MaqamTuning::MaqamScale MaqamTuning::scales[] =
{
    { "Rast",          { 0, 0, -50, 0, 0, 0, -50, 0 } },       // 3/4 tone on 3rd and 7th
    { "Bayati",        { 0, -50, 0, 0, 0, -50, 0, 0 } },       // 3/4 tone on 2nd and 6th
    { "Saba",          { 0, -50, 0, -50, 0, 0, 0, 0 } },       // 3/4 tone on 2nd and 4th
    { "Hijaz",         { 0, -50, +50, 0, 0, -50, 0, 0 } },     // augmented 2nd
    { "Hijaz Kar",     { 0, -50, +50, 0, 0, -50, +50, 0 } },
    { "Kurd",          { 0, 0, 0, 0, 0, 0, 0, 0 } },           // close to Phrygian
    { "Nahawand",      { 0, 0, 0, 0, 0, 0, 0, 0 } },           // close to minor
    { "Ajam",          { 0, 0, 0, 0, 0, 0, 0, 0 } },           // close to major
    { "Segah",         { 0, -50, 0, 0, -50, 0, 0, 0 } },
    { "Siga",          { -50, 0, 0, -50, 0, 0, 0, 0 } },
    { "Nawa Athar",    { 0, 0, -50, +50, 0, -50, +50, 0 } },
    { "Saba Zamzam",   { 0, -50, 0, -50, -50, 0, 0, 0 } },
    { "Rahat al-Arwah",{ 0, -50, +50, 0, -50, +50, 0, 0 } },
    { "Jiharkah",      { 0, 0, -50, 0, 0, -50, 0, 0 } },
    { "Zanjaran",      { 0, -50, +50, 0, -50, +50, 0, 0 } },
};

MaqamTuning::MaqamTuning() = default;

void MaqamTuning::setMaqam(Maqam maqam)
{
    currentMaqam = maqam;
}

void MaqamTuning::setRootNote(int midiNote)
{
    rootNote = midiNote;
}

void MaqamTuning::setBaseTuning(float tuningHz)
{
    baseTuning = tuningHz;
}

float MaqamTuning::getFrequencyForNote(int midiNote) const
{
    float centsDeviation = getCentsDeviation(midiNote);
    float standardFreq = baseTuning * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
    return standardFreq * std::pow(2.0f, centsDeviation / 1200.0f);
}

float MaqamTuning::getCentsDeviation(int midiNote) const
{
    int scaleIdx = static_cast<int>(currentMaqam);
    if (scaleIdx < 0 || scaleIdx >= static_cast<int>(Maqam::Count))
        return 0.0f;

    int degree = ((midiNote - rootNote) % 12 + 12) % 12;

    // Map chromatic degree to scale degree (simplified)
    // For a more accurate implementation, each maqam would have its own chromatic mapping
    int scaleDegree = -1;
    static const int majorDegrees[] = { 0, -1, 1, -1, 2, 3, -1, 4, -1, 5, -1, 6 };

    if (degree >= 0 && degree < 12)
        scaleDegree = majorDegrees[degree];

    if (scaleDegree >= 0 && scaleDegree < 8)
        return scales[scaleIdx].intervals[scaleDegree];

    return 0.0f;
}

const char* MaqamTuning::getMaqamName(Maqam maqam)
{
    int idx = static_cast<int>(maqam);
    if (idx >= 0 && idx < static_cast<int>(Maqam::Count))
        return scales[idx].name;
    return "Unknown";
}

MaqamTuning::Maqam MaqamTuning::maqamFromString(const juce::String& name)
{
    for (int i = 0; i < static_cast<int>(Maqam::Count); ++i)
        if (name.equalsIgnoreCase(scales[i].name))
            return static_cast<Maqam>(i);
    return Maqam::Rast;
}
