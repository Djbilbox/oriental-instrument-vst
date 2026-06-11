#include "OrientalSynthesiser.h"
#include "OrientalSound.h"
#include <algorithm>

OrientalSynthesiser::OrientalSynthesiser()
{
    for (int i = 0; i < OrientalConstants::MAX_VOICES; ++i)
        synth.addVoice(new OrientalVoice());

    auto* s = new OrientalSound();
    soundPtr = s;
    synth.addSound(s);
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

    monoVoice = dynamic_cast<OrientalVoice*>(synth.getVoice(0));
    monoStack.clear();

    updateVoices();
}

void OrientalSynthesiser::renderNextBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer& midiMessages,
                                           int startSample, int numSamples)
{
    if (monoMode && monoVoice != nullptr)
        renderMono(buffer, midiMessages, startSample, numSamples);
    else
        synth.renderNextBlock(buffer, midiMessages, startSample, numSamples);
}

void OrientalSynthesiser::setMonoMode(bool shouldBeMono)
{
    if (monoMode == shouldBeMono)
        return;
    monoMode = shouldBeMono;
    // Clear any sounding notes when switching engines to avoid stuck voices.
    monoStack.clear();
    synth.allNotesOff(0, false);
    if (monoVoice != nullptr)
        monoVoice->stopNote(0.0f, false);
}

void OrientalSynthesiser::setLegato(bool shouldBeLegato)
{
    legatoMode = shouldBeLegato;
}

void OrientalSynthesiser::handleMonoMessage(const juce::MidiMessage& m)
{
    auto removeNote = [this](int n)
    {
        monoStack.erase(std::remove_if(monoStack.begin(), monoStack.end(),
                                       [n](const HeldNote& h) { return h.note == n; }),
                        monoStack.end());
    };

    if (m.isNoteOn())
    {
        const int note = m.getNoteNumber();
        const float vel = m.getFloatVelocity();
        const bool wasPlaying = ! monoStack.empty();
        removeNote(note);
        monoStack.push_back({ note, vel });

        if (wasPlaying && legatoMode)
            monoVoice->changeNoteLegato(note);
        else
            monoVoice->startNote(note, vel, soundPtr, monoPitchWheel);
    }
    else if (m.isNoteOff())
    {
        const int note = m.getNoteNumber();
        const bool wasTop = (! monoStack.empty() && monoStack.back().note == note);
        removeNote(note);

        if (monoStack.empty())
        {
            monoVoice->stopNote(0.0f, true);
        }
        else if (wasTop)
        {
            const auto top = monoStack.back();
            if (legatoMode)
                monoVoice->changeNoteLegato(top.note);
            else
                monoVoice->startNote(top.note, top.vel, soundPtr, monoPitchWheel);
        }
    }
    else if (m.isPitchWheel())
    {
        monoPitchWheel = m.getPitchWheelValue();
        monoVoice->pitchWheelMoved(monoPitchWheel);
    }
    else if (m.isController() && m.getControllerNumber() == 1)
    {
        monoVoice->controllerMoved(1, m.getControllerValue());
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        monoStack.clear();
        monoVoice->stopNote(0.0f, true);
    }
}

void OrientalSynthesiser::renderMono(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi,
                                      int startSample, int numSamples)
{
    int pos = startSample;
    const int end = startSample + numSamples;

    for (const auto metadata : midi)
    {
        const int t = juce::jlimit(startSample, end, metadata.samplePosition);
        const int gap = t - pos;
        if (gap > 0)
        {
            monoVoice->renderNextBlock(buffer, pos, gap);
            pos = t;
        }
        handleMonoMessage(metadata.getMessage());
    }

    const int remaining = end - pos;
    if (remaining > 0)
        monoVoice->renderNextBlock(buffer, pos, remaining);
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

void OrientalSynthesiser::setADSR(const ADSREnvelope::Parameters& params)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<OrientalVoice*>(synth.getVoice(i)))
            voice->setADSRParameters(params);
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
