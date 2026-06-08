#pragma once
#include <JuceHeader.h>
#include "../Utils/Constants.h"

struct InstrumentProfile
{
    OrientalConstants::Instrument instrument;
    enum class WaveType { Sine, Sawtooth, Triangle, Square } waveType;
    float vibratoRate;
    float vibratoDepth;
    float attack;
    float decay;
    float sustain;
    float release;
    float filterFreq;
    float filterQ;
    std::vector<float> harmonics;
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
