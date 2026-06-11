#include "OrientalVoice.h"
#include "OrientalSound.h"

OrientalVoice::OrientalVoice()
{
    vibratoLFO.generateSineTable();
    subOsc.generateSineTable();
}

void OrientalVoice::prepareToPlay(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;

    for (auto& osc : oscillators)
        osc.prepare(sr);
    subOsc.prepare(sr);
    vibratoLFO.prepare(sr);

    envelope.prepare(sr);
    filterEnv.prepare(sr);
    noiseEnv.prepare(sr);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;

    svfL.prepare(spec); svfR.prepare(spec);
    svfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    svfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    formant1L.prepare(spec); formant1R.prepare(spec);
    formant2L.prepare(spec); formant2R.prepare(spec);
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
    currentFrequency = (glideRate > 0.0f && currentFrequency > 0.0f) ? currentFrequency
                                                                     : targetFrequency;

    for (auto& osc : oscillators)
        osc.reset();
    subOsc.reset();
    noiseLP = 0.0f;
    svfL.reset(); svfR.reset();
    formant1L.reset(); formant1R.reset();
    formant2L.reset(); formant2R.reset();

    if (currentProfile)
    {
        vibratoLFO.setFrequency(currentProfile->vibratoRate);

        // ── Amplitude envelope ──
        ADSREnvelope::Parameters envParams;
        if (hasPresetADSR)
        {
            envParams.attack  = presetADSR.attack;
            envParams.decay   = presetADSR.decay;
            envParams.sustain = presetADSR.sustain * (0.3f + depthAmount * 0.7f);
            envParams.release = presetADSR.release;
        }
        else
        {
            envParams.attack  = currentProfile->attack;
            envParams.decay   = currentProfile->decay;
            envParams.sustain = currentProfile->sustain * (0.3f + depthAmount * 0.7f);
            envParams.release = currentProfile->release;
        }
        envelope.setParameters(envParams);

        // ── Filter envelope ──
        ADSREnvelope::Parameters fp;
        fp.attack  = juce::jmax(0.001f, envParams.attack * 0.5f);
        fp.decay   = currentProfile->filterEnvDecay;
        fp.sustain = currentProfile->filterEnvSust;
        fp.release = envParams.release;
        filterEnv.setParameters(fp);

        // ── Noise / breath envelope ──
        ADSREnvelope::Parameters np;
        if (currentProfile->noiseSustains)
            np = { 0.01f, currentProfile->noiseDecay, 0.7f, 0.2f };
        else
            np = { 0.001f, currentProfile->noiseDecay, 0.0f, 0.05f };
        noiseEnv.setParameters(np);

        // ── Resolve per-note character ──
        noteDrive = currentProfile->drive * (1.0f + currentProfile->velToDrive * velocity);
        noteAsym  = currentProfile->driveAsym;

        // ── Body resonance coefficients (key-independent) ──
        hasFormant1 = currentProfile->formant1.gain > 0.0f && currentProfile->formant1.freq > 20.0f;
        hasFormant2 = currentProfile->formant2.gain > 0.0f && currentProfile->formant2.freq > 20.0f;
        if (hasFormant1)
        {
            auto c1 = juce::dsp::IIR::Coefficients<float>::makeBandPass(
                sampleRate, currentProfile->formant1.freq, currentProfile->formant1.q);
            *formant1L.coefficients = *c1; *formant1R.coefficients = *c1;
        }
        if (hasFormant2)
        {
            auto c2 = juce::dsp::IIR::Coefficients<float>::makeBandPass(
                sampleRate, currentProfile->formant2.freq, currentProfile->formant2.q);
            *formant2L.coefficients = *c2; *formant2R.coefficients = *c2;
        }
    }

    float safeRes = juce::jlimit(0.5f, 8.0f, baseResonance);
    svfL.setResonance(safeRes); svfR.setResonance(safeRes);

    envelope.noteOn();
    filterEnv.noteOn();
    noiseEnv.noteOn();
    pitchWheelMoved(currentPitchWheelPosition);
}

void OrientalVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
        filterEnv.noteOff();
        noiseEnv.noteOff();
    }
    else
    {
        envelope.reset();
        filterEnv.reset();
        noiseEnv.reset();
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

float OrientalVoice::shape(float x) const
{
    // Asymmetric soft saturation → even + odd harmonics ("organic grain").
    // DC component removed by subtracting the bias offset.
    return std::tanh(x + noteAsym) - std::tanh(noteAsym);
}

void OrientalVoice::updateFilterCoefficients(float cutoffHz)
{
    float safe = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.45), cutoffHz);
    svfL.setCutoffFrequency(safe);
    svfR.setCutoffFrequency(safe);
}

void OrientalVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                     int startSample, int numSamples)
{
    if (currentProfile == nullptr) { clearCurrentNote(); return; }
    if (!envelope.isActive())      { clearCurrentNote(); return; }

    const auto& prof = *currentProfile;
    const int numChannels = outputBuffer.getNumChannels();

    // Pan gains for the detuned unison voices (equal power).
    float lGain[kUnison], rGain[kUnison];
    for (int i = 0; i < kUnison; ++i)
    {
        float p = unisonPan[i];
        lGain[i] = std::sqrt(0.5f * (1.0f - p));
        rGain[i] = std::sqrt(0.5f * (1.0f + p));
    }

    const float detuneSpread = prof.detuneCents * (0.6f + orientAmount * 0.8f);
    const float driveAmt = noteDrive * (0.6f + depthAmount * 1.4f);
    const float keyOct = prof.keyTrack * std::log2(juce::jmax(20.0f, currentFrequency) / 440.0f);
    const float velOct = prof.velToFilter * velocity;
    const float ampMakeup = 0.5f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Glide toward target
        if (std::abs(currentFrequency - targetFrequency) > 0.01f)
        {
            float step = (targetFrequency - currentFrequency) * glideRate;
            currentFrequency = (std::abs(step) < 0.01f) ? targetFrequency
                                                        : currentFrequency + step;
        }

        // Vibrato (Orient + mod wheel)
        float vibratoMod = 1.0f;
        if (prof.vibratoRate > 0.0f)
        {
            float v = vibratoLFO.getNextSample();
            float depth = prof.vibratoDepth * orientAmount * 2.0f
                        + prof.vibratoDepth * modWheelValue * 1.5f;
            vibratoMod = 1.0f + v * depth;
        }

        float baseFreq = currentFrequency * pitchBendFactor * vibratoMod;

        // ── Unison oscillator stack ──
        float center = 0.0f, sideL = 0.0f, sideR = 0.0f;
        for (int i = 0; i < kUnison; ++i)
        {
            float cents = unisonDetune[i] * detuneSpread;
            oscillators[i].setFrequency(baseFreq * std::exp2(cents / 1200.0f));
            float s = oscillators[i].getNextSample();
            if (i == 0)
            {
                center += s;
            }
            else
            {
                s *= prof.unisonMix;
                sideL += s * lGain[i];
                sideR += s * rGain[i];
            }
        }

        // Sub oscillator (mono, centre)
        if (prof.subLevel > 0.0f)
        {
            subOsc.setFrequency(baseFreq * 0.5f);
            center += subOsc.getNextSample() * prof.subLevel;
        }

        // Noise / breath (mono, centre)
        if (prof.noiseAmount > 0.0f)
        {
            float white = rng.nextFloat() * 2.0f - 1.0f;
            noiseLP += 0.2f * (white - noiseLP);
            float colored = noiseLP + (white - noiseLP) * prof.noiseColor;
            center += colored * prof.noiseAmount * noiseEnv.getNextSample();
        }

        float left  = center + sideL;
        float right = center + sideR;

        // ── Drive / waveshaper (the grain) ──
        left  = shape(left  * driveAmt);
        right = shape(right * driveAmt);

        // ── Resonant filter with envelope + key-track + velocity ──
        float fenv = filterEnv.getNextSample();
        float cutoff = baseCutoff * std::exp2(prof.filterEnvAmt * fenv + keyOct + velOct);
        updateFilterCoefficients(cutoff);
        left  = svfL.processSample(0, left);
        right = svfR.processSample(0, right);

        // ── Body resonance (parallel formants) ──
        if (hasFormant1)
        {
            left  += formant1L.processSample(left)  * prof.formant1.gain;
            right += formant1R.processSample(right) * prof.formant1.gain;
        }
        if (hasFormant2)
        {
            left  += formant2L.processSample(left)  * prof.formant2.gain;
            right += formant2R.processSample(right) * prof.formant2.gain;
        }

        // ── Amp + final cohesion clip ──
        float env = envelope.getNextSample();
        float gain = env * velocity * ampMakeup;
        left  = std::tanh(left  * gain);
        right = std::tanh(right * gain);

        if (numChannels >= 2)
        {
            outputBuffer.addSample(0, startSample + sample, left);
            outputBuffer.addSample(1, startSample + sample, right);
        }
        else if (numChannels == 1)
        {
            outputBuffer.addSample(0, startSample + sample, (left + right) * 0.5f);
        }
    }
}

void OrientalVoice::setInstrumentProfile(const InstrumentProfile& profile)
{
    currentProfile = &profile;

    // Build the oscillator spectrum once, share across the unison stack.
    if (!profile.harmonics.empty())
    {
        oscillators[0].generateFromHarmonics(profile.harmonics);
    }
    else
    {
        switch (profile.waveType)
        {
            case InstrumentProfile::WaveType::Sine:     oscillators[0].generateSineTable();     break;
            case InstrumentProfile::WaveType::Sawtooth: oscillators[0].generateSawtoothTable();  break;
            case InstrumentProfile::WaveType::Triangle: oscillators[0].generateTriangleTable();  break;
            case InstrumentProfile::WaveType::Square:   oscillators[0].generateSquareTable();    break;
        }
    }
    // Copy the generated table to the other unison voices.
    for (int i = 1; i < kUnison; ++i)
    {
        if (!profile.harmonics.empty())
            oscillators[i].generateFromHarmonics(profile.harmonics);
        else
            oscillators[i].generateSineTable();
    }

    baseCutoff = profile.filterFreq;
    baseResonance = profile.filterQ;
}

void OrientalVoice::setMaqamTuning(const MaqamTuning& tuning)
{
    maqamTuning = tuning;
}

void OrientalVoice::setGlide(float glideTime)
{
    if (glideTime < 0.01f)
        glideRate = 1.0f;
    else
        glideRate = 1.0f / (glideTime * static_cast<float>(sampleRate) * 0.5f);
}

void OrientalVoice::setFilterParams(float cutoff, float resonance)
{
    baseCutoff = cutoff;
    baseResonance = resonance;
    float safeRes = juce::jlimit(0.5f, 8.0f, resonance);
    svfL.setResonance(safeRes);
    svfR.setResonance(safeRes);
}

void OrientalVoice::setOrientAmount(float amount) { orientAmount = amount; }
void OrientalVoice::setDepth(float depth)         { depthAmount = depth; }

void OrientalVoice::setADSRParameters(const ADSREnvelope::Parameters& params)
{
    presetADSR = params;
    hasPresetADSR = true;
}

void OrientalVoice::changeNoteLegato(int midiNoteNumber)
{
    // Slur to the new pitch without restarting the envelopes (true legato).
    targetFrequency = maqamTuning.getFrequencyForNote(midiNoteNumber);
    if (glideRate <= 0.0f)
        currentFrequency = targetFrequency; // no portamento → jump cleanly
}
