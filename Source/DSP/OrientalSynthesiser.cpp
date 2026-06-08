#include "OrientalSynthesiser.h"
#include "OrientalSound.h"

OrientalSynthesiser::OrientalSynthesiser()
{
    for (int i = 0; i < OrientalConstants::MAX_VOICES; ++i)
        synth.addVoice(new OrientalVoice());

    synth.addSound(new OrientalSound());
}

void OrientalSynthesiser::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }

    updateVoices();
}

void OrientalSynthesiser::renderNextBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer& midiMessages,
                                           int startSample, int numSamples)
{
    synth.renderNextBlock(buffer, midiMessages, startSample, numSamples);
}

void OrientalSynthesiser::setInstrument(OrientalConstants::Instrument instrument)
{
    if (currentInstrument != instrument)
    {
        currentInstrument = instrument;
        updateVoices();
    }
}

void OrientalSynthesiser::setMaqam(MaqamTuning::Maqam maqam)
{
    maqamTuning.setMaqam(maqam);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setMaqamTuning(maqamTuning);
}

void OrientalSynthesiser::setRootNote(int midiNote)
{
    maqamTuning.setRootNote(midiNote);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setMaqamTuning(maqamTuning);
}

void OrientalSynthesiser::setBaseTuning(float tuningHz)
{
    maqamTuning.setBaseTuning(tuningHz);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setMaqamTuning(maqamTuning);
}

void OrientalSynthesiser::setGlide(float glideTime)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setGlide(glideTime);
}

void OrientalSynthesiser::setFilterCutoff(float cutoff)
{
    currentFilterCutoff = cutoff;
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setFilterParams(currentFilterCutoff, currentFilterQ);
}

void OrientalSynthesiser::setFilterResonance(float resonance)
{
    currentFilterQ = resonance;
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setFilterParams(currentFilterCutoff, currentFilterQ);
}

void OrientalSynthesiser::setOrientAmount(float amount)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setOrientAmount(amount);
}

void OrientalSynthesiser::setDepth(float depth)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setDepth(depth);
}

void OrientalSynthesiser::setPitchWheel(int pitchWheelValue)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->pitchWheelMoved(pitchWheelValue);
}

void OrientalSynthesiser::setModWheel(float normalizedValue)
{
    int cc1 = static_cast<int>(normalizedValue * 127.0f);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->controllerMoved(1, cc1);
}

void OrientalSynthesiser::noteOn(int channel, int midiNote, float velocity)
{
    synth.noteOn(channel, midiNote, velocity);
}

void OrientalSynthesiser::noteOff(int channel, int midiNote, float velocity)
{
    synth.noteOff(channel, midiNote, velocity, true);
}

void OrientalSynthesiser::allNotesOff()
{
    synth.allNotesOff(0, true);
}

void OrientalSynthesiser::updateVoices()
{
    const auto& profile = InstrumentProfiles::getProfile(currentInstrument);
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
        {
            voice->setInstrumentProfile(profile);
            voice->setMaqamTuning(maqamTuning);
        }
    }
}
