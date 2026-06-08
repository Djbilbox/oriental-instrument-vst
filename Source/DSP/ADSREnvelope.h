#pragma once
#include <JuceHeader.h>

class ADSREnvelope
{
public:
    struct Parameters
    {
        float attack  = 0.1f;
        float decay   = 0.1f;
        float sustain = 0.7f;
        float release = 0.3f;
    };

    ADSREnvelope() = default;
    ~ADSREnvelope() = default;

    void prepare(double sampleRate);
    void setParameters(const Parameters& params);
    void noteOn();
    void noteOff();
    float getNextSample();
    bool isActive() const;
    void reset();

private:
    enum class State { Idle, Attack, Decay, Sustain, Release };

    State currentState = State::Idle;
    Parameters params;
    double sampleRate = 44100.0;
    float currentLevel = 0.0f;
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseRate = 0.0f;

    void calculateRates();
};
