#include "OrientalVoice.h"
#include "OrientalSound.h"

OrientalVoice::OrientalVoice()
{
    vibratoLFO.generateSineTable();
}

void OrientalVoice::prepareToPlay(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    oscillator.prepare(sr);
    vibratoLFO.prepare(sr);
    envelope.prepare(sr);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;
    filter.prepare(spec);
}

bool OrientalVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<OrientalSound*>(sound) != nullptr;
}

void OrientalVoice::startNote(int midiNoteNumber, float vel,
                              juce::SynthesiserSound*, int currentPitchWheelPosition)
{
    velocity = vel;
    targetFrequency = maqamTuning.getFrequencyForNote(midiNoteNumber);

    if (glideRate > 0.0f && currentFrequency > 0.0f)
    {
        // Glide from current to target - smooth portamento
    }
    else
    {
        currentFrequency = targetFrequency;
    }

    oscillator.setFrequency(currentFrequency * pitchBendFactor);
    oscillator.reset();

    if (currentProfile)
    {
        vibratoLFO.setFrequency(currentProfile->vibratoRate);

        ADSREnvelope::Parameters envParams;
        envParams.attack  = currentProfile->attack;
        envParams.decay   = currentProfile->decay;
        // Depth affects sustain level
        envParams.sustain = currentProfile->sustain * (0.3f + depthAmount * 0.7f);
        envParams.release = currentProfile->release;
        envelope.setParameters(envParams);

        // Apply filter with current cutoff
        float safeCutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.45), filterCutoff);
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, safeCutoff, filterResonance);
        *filter.coefficients = *coeffs;
    }

    envelope.noteOn();
    pitchWheelMoved(currentPitchWheelPosition);
}

void OrientalVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
    }
    else
    {
        envelope.reset();
        clearCurrentNote();
    }
}

void OrientalVoice::pitchWheelMoved(int newValue)
{
    float normalized = (static_cast<float>(newValue) - 8192.0f) / 8192.0f;
    pitchBendFactor = std::pow(2.0f, normalized * 2.0f / 12.0f); // +/- 2 semitones
}

void OrientalVoice::controllerMoved(int controllerNumber, int newValue)
{
    if (controllerNumber == 1) // Mod wheel
        modWheelValue = static_cast<float>(newValue) / 127.0f;
}

void OrientalVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                     int startSample, int numSamples)
{
    if (!envelope.isActive())
    {
        clearCurrentNote();
        return;
    }

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Glide towards target frequency
        if (std::abs(currentFrequency - targetFrequency) > 0.01f)
        {
            float glideStep = (targetFrequency - currentFrequency) * glideRate;
            if (std::abs(glideStep) < 0.01f)
                currentFrequency = targetFrequency;
            else
                currentFrequency += glideStep;
        }

        // Vibrato modulated by ORIENT knob and mod wheel
        float vibratoMod = 1.0f;
        if (currentProfile && currentProfile->vibratoRate > 0.0f)
        {
            float vibratoSample = vibratoLFO.getNextSample();
            // orientAmount scales base vibrato depth, mod wheel adds extra
            float effectiveDepth = currentProfile->vibratoDepth * orientAmount * 2.0f;
            effectiveDepth += currentProfile->vibratoDepth * modWheelValue * 1.5f;
            vibratoMod = 1.0f + vibratoSample * effectiveDepth;
        }

        // Set oscillator frequency with pitch bend and vibrato
        oscillator.setFrequency(currentFrequency * pitchBendFactor * vibratoMod);

        // Get sample from wavetable
        float oscSample = oscillator.getNextSample();

        // Depth affects amplitude/harmonic richness by scaling output gain
        float depthGain = 0.4f + depthAmount * 0.6f;

        // Envelope
        float envValue = envelope.getNextSample();

        // Filter
        float filteredSample = filter.processSample(oscSample);

        // Final output
        float outputSample = filteredSample * envValue * velocity * depthGain;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample(channel, startSample + sample, outputSample);
    }
}

void OrientalVoice::setInstrumentProfile(const InstrumentProfile& profile)
{
    currentProfile = &profile;

    // Generate wavetable based on instrument harmonics
    if (!profile.harmonics.empty())
    {
        oscillator.generateFromHarmonics(profile.harmonics);
    }
    else
    {
        switch (profile.waveType)
        {
            case InstrumentProfile::WaveType::Sine:
                oscillator.generateSineTable();
                break;
            case InstrumentProfile::WaveType::Sawtooth:
                oscillator.generateSawtoothTable();
                break;
            case InstrumentProfile::WaveType::Triangle:
                oscillator.generateTriangleTable();
                break;
            case InstrumentProfile::WaveType::Square:
                oscillator.generateSquareTable();
                break;
        }
    }

    filterCutoff = profile.filterFreq;
    filterResonance = profile.filterQ;
}

void OrientalVoice::setMaqamTuning(const MaqamTuning& tuning)
{
    maqamTuning = tuning;
}

void OrientalVoice::setGlide(float glideTime)
{
    // glideTime 0..1 mapped to instant..slow
    if (glideTime < 0.01f)
        glideRate = 1.0f;
    else
        glideRate = 1.0f / (glideTime * static_cast<float>(sampleRate) * 0.5f);
}

void OrientalVoice::setFilterParams(float cutoff, float resonance)
{
    filterCutoff = cutoff;
    filterResonance = resonance;
    if (sampleRate > 0.0)
    {
        float safeCutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.45), cutoff);
        float safeQ = juce::jlimit(0.1f, 10.0f, resonance);
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, safeCutoff, safeQ);
        *filter.coefficients = *coeffs;
    }
}

void OrientalVoice::setOrientAmount(float amount)
{
    orientAmount = amount;
}

void OrientalVoice::setDepth(float depth)
{
    depthAmount = depth;
}
