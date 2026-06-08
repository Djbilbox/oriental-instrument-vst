#include "ADSREnvelope.h"

void ADSREnvelope::prepare(double sr)
{
    sampleRate = sr;
    calculateRates();
}

void ADSREnvelope::setParameters(const Parameters& p)
{
    params = p;
    calculateRates();
}

void ADSREnvelope::noteOn()
{
    currentState = State::Attack;
    currentLevel = 0.0f;
}

void ADSREnvelope::noteOff()
{
    if (currentState != State::Idle)
        currentState = State::Release;
}

float ADSREnvelope::getNextSample()
{
    switch (currentState)
    {
        case State::Idle:
            return 0.0f;

        case State::Attack:
            currentLevel += attackRate;
            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                currentState = State::Decay;
            }
            break;

        case State::Decay:
            currentLevel -= decayRate;
            if (currentLevel <= params.sustain)
            {
                currentLevel = params.sustain;
                currentState = State::Sustain;
            }
            break;

        case State::Sustain:
            currentLevel = params.sustain;
            break;

        case State::Release:
            currentLevel -= releaseRate;
            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                currentState = State::Idle;
            }
            break;
    }

    return currentLevel;
}

bool ADSREnvelope::isActive() const
{
    return currentState != State::Idle;
}

void ADSREnvelope::reset()
{
    currentState = State::Idle;
    currentLevel = 0.0f;
}

void ADSREnvelope::calculateRates()
{
    float sr = static_cast<float>(sampleRate);
    attackRate  = params.attack  > 0.0f ? 1.0f / (params.attack * sr)  : 1.0f;
    decayRate   = params.decay   > 0.0f ? (1.0f - params.sustain) / (params.decay * sr) : 1.0f;
    releaseRate = params.release > 0.0f ? params.sustain / (params.release * sr) : 1.0f;
}
