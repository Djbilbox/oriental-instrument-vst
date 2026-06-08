#pragma once
#include <JuceHeader.h>

class MaqamTuning
{
public:
    MaqamTuning();
    ~MaqamTuning() = default;

    enum class Maqam
    {
        Rast, Bayati, Saba, Hijaz, HijazKar, Kurd,
        Nahawand, Ajam, Segah, Siga, NawaAthar,
        SabaZamzam, RahatAlArwah, Jiharkah, Zanjaran,
        Count
    };

    void setMaqam(Maqam maqam);
    void setRootNote(int midiNote);
    void setBaseTuning(float tuningHz);

    float getFrequencyForNote(int midiNote) const;
    static const char* getMaqamName(Maqam maqam);
    static Maqam maqamFromString(const juce::String& name);

private:
    struct MaqamScale
    {
        const char* name;
        float intervals[8]; // quarter-tone deviations in cents from 12-TET
    };

    Maqam currentMaqam = Maqam::Rast;
    int rootNote = 62; // D4
    float baseTuning = 440.0f;

    static const MaqamScale scales[];
    float getCentsDeviation(int midiNote) const;
};
