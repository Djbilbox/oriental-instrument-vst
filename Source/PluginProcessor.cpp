#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/ADSREnvelope.h"

OrientalInstrumentProcessor::OrientalInstrumentProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache raw parameter pointers for lock-free real-time access
    glideParam      = apvts.getRawParameterValue("glide");
    spaceParam      = apvts.getRawParameterValue("space");
    filterParam     = apvts.getRawParameterValue("filter");
    orientParam     = apvts.getRawParameterValue("orient");
    resoParam       = apvts.getRawParameterValue("reso");
    depthParam      = apvts.getRawParameterValue("depth");
    fxMixParam      = apvts.getRawParameterValue("fxmix");
    instrumentParam = apvts.getRawParameterValue("instrument");
    presetParam     = apvts.getRawParameterValue("preset");

    reverbAmtParam   = apvts.getRawParameterValue("reverbAmt");
    delayAmtParam    = apvts.getRawParameterValue("delayAmt");
    chorusAmtParam   = apvts.getRawParameterValue("chorusAmt");
    distAmtParam     = apvts.getRawParameterValue("distAmt");
    compAmtParam     = apvts.getRawParameterValue("compAmt");
    phaserAmtParam   = apvts.getRawParameterValue("phaserAmt");
    bitcrushAmtParam = apvts.getRawParameterValue("bitcrushAmt");

    attackParam  = apvts.getRawParameterValue("attack");
    decayParam   = apvts.getRawParameterValue("decay");
    sustainParam = apvts.getRawParameterValue("sustain");
    releaseParam = apvts.getRawParameterValue("release");

    reverbOnParam   = apvts.getRawParameterValue("reverbOn");
    delayOnParam    = apvts.getRawParameterValue("delayOn");
    chorusOnParam   = apvts.getRawParameterValue("chorusOn");
    distOnParam     = apvts.getRawParameterValue("distOn");
    compOnParam     = apvts.getRawParameterValue("compOn");
    eqOnParam       = apvts.getRawParameterValue("eqOn");
    eqAmtParam      = apvts.getRawParameterValue("eqAmt");
    phaserOnParam   = apvts.getRawParameterValue("phaserOn");
    bitcrushOnParam = apvts.getRawParameterValue("bitcrushOn");

    // Listen for instrument/preset changes
    apvts.addParameterListener("instrument", this);
    apvts.addParameterListener("preset", this);
}

OrientalInstrumentProcessor::~OrientalInstrumentProcessor()
{
    apvts.removeParameterListener("instrument", this);
    apvts.removeParameterListener("preset", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout OrientalInstrumentProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Macro knobs
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("glide", 1), "Glide", 0.0f, 100.0f, 25.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("space", 1), "Space", 0.0f, 100.0f, 60.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filter", 1), "Filter", 0.0f, 100.0f, 80.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("orient", 1), "Orient", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reso", 1), "Resonance", 0.0f, 100.0f, 35.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("depth", 1), "Depth", 0.0f, 100.0f, 70.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("fxmix", 1), "FX Mix", 0.0f, 100.0f, 50.0f));

    // Instrument & preset selection
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("instrument", 1), "Instrument", 0, 6, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("preset", 1), "Preset", 0, 69, 0));

    // FX amounts (0-100)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbAmt", 1), "Reverb Amount", 0.0f, 100.0f, 72.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayAmt", 1), "Delay Amount", 0.0f, 100.0f, 45.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("chorusAmt", 1), "Chorus Amount", 0.0f, 100.0f, 38.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("distAmt", 1), "Distortion Amount", 0.0f, 100.0f, 18.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compAmt", 1), "Compressor Amount", 0.0f, 100.0f, 60.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("phaserAmt", 1), "Phaser Amount", 0.0f, 100.0f, 30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bitcrushAmt", 1), "Bitcrusher Amount", 0.0f, 100.0f, 8.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("eqAmt", 1), "EQ Amount", 0.0f, 100.0f, 50.0f));

    // ADSR per-preset (secondes / 0-1 pour sustain)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("attack", 1), "Attack",
        juce::NormalisableRange<float>(0.001f, 3.0f, 0.001f, 0.3f), 0.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("decay", 1), "Decay",
        juce::NormalisableRange<float>(0.01f, 3.0f, 0.001f, 0.3f), 0.10f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("sustain", 1), "Sustain", 0.0f, 1.0f, 0.70f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("release", 1), "Release",
        juce::NormalisableRange<float>(0.01f, 5.0f, 0.001f, 0.3f), 0.30f));

    // FX on/off toggles
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("reverbOn", 1), "Reverb On", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("delayOn", 1), "Delay On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("chorusOn", 1), "Chorus On", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("distOn", 1), "Distortion On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("compOn", 1), "Compressor On", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("eqOn", 1), "EQ On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("phaserOn", 1), "Phaser On", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("bitcrushOn", 1), "Bitcrusher On", false));

    return { params.begin(), params.end() };
}

void OrientalInstrumentProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "instrument")
    {
        int instIdx = static_cast<int>(newValue);
        if (instIdx != lastInstrument)
        {
            lastInstrument = instIdx;
            synthesiser.setInstrument(static_cast<OrientalConstants::Instrument>(instIdx));
            sampleEngine.setInstrument(static_cast<OrientalConstants::Instrument>(instIdx));
        }
    }
    else if (parameterID == "preset")
    {
        int presetIdx = static_cast<int>(newValue);
        if (presetIdx != lastPreset)
        {
            lastPreset = presetIdx;
            presetManager.selectPreset(presetIdx);
            applyPresetToParameters(presetManager.getCurrentPreset());
        }
    }
}

void OrientalInstrumentProcessor::applyPresetToParameters(const PresetData& preset)
{
    // Apply preset values to APVTS (will propagate to UI knobs and DSP)
    if (auto* p = apvts.getParameter("glide"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.glide));
    if (auto* p = apvts.getParameter("space"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.space));
    if (auto* p = apvts.getParameter("filter"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.filter));
    if (auto* p = apvts.getParameter("orient"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.orient));
    if (auto* p = apvts.getParameter("reso"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.reso));
    if (auto* p = apvts.getParameter("depth"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.depth));
    if (auto* p = apvts.getParameter("fxmix"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.fxMix));

    // FX states
    if (auto* p = apvts.getParameter("reverbOn"))
        p->setValueNotifyingHost(preset.reverbOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("delayOn"))
        p->setValueNotifyingHost(preset.echoOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("chorusOn"))
        p->setValueNotifyingHost(preset.chorusOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("distOn"))
        p->setValueNotifyingHost(preset.distortOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("compOn"))
        p->setValueNotifyingHost(preset.compressorOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("phaserOn"))
        p->setValueNotifyingHost(preset.phaserOn ? 1.0f : 0.0f);
    if (auto* p = apvts.getParameter("bitcrushOn"))
        p->setValueNotifyingHost(preset.bitcrushOn ? 1.0f : 0.0f);

    // FX amounts
    if (auto* p = apvts.getParameter("reverbAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.reverbAmount * 100.0f));
    if (auto* p = apvts.getParameter("delayAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.echoAmount * 100.0f));
    if (auto* p = apvts.getParameter("chorusAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.chorusAmount * 100.0f));
    if (auto* p = apvts.getParameter("distAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.distortAmount * 100.0f));
    if (auto* p = apvts.getParameter("compAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.compressorAmount * 100.0f));
    if (auto* p = apvts.getParameter("phaserAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.phaserAmount * 100.0f));
    if (auto* p = apvts.getParameter("bitcrushAmt"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.bitcrushAmount * 100.0f));

    // ADSR per-preset
    if (auto* p = apvts.getParameter("attack"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.attack));
    if (auto* p = apvts.getParameter("decay"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.decay));
    if (auto* p = apvts.getParameter("sustain"))
        p->setValueNotifyingHost(preset.sustain); // 0-1, pas de conversion
    if (auto* p = apvts.getParameter("release"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.release));

    // Switch instrument and maqam
    synthesiser.setInstrument(preset.instrument);
    sampleEngine.setInstrument(preset.instrument);
    synthesiser.setMaqam(MaqamTuning::maqamFromString(preset.maqam));
}

const juce::String OrientalInstrumentProcessor::getName() const
{
    return OrientalConstants::PLUGIN_NAME;
}

bool OrientalInstrumentProcessor::acceptsMidi() const  { return true; }
bool OrientalInstrumentProcessor::producesMidi() const { return false; }
bool OrientalInstrumentProcessor::isMidiEffect() const { return false; }
double OrientalInstrumentProcessor::getTailLengthSeconds() const { return 2.0; }

int OrientalInstrumentProcessor::getNumPrograms() { return OrientalConstants::TOTAL_PRESETS; }
int OrientalInstrumentProcessor::getCurrentProgram() { return presetManager.getCurrentPresetIndex(); }

void OrientalInstrumentProcessor::setCurrentProgram(int index)
{
    presetManager.selectPreset(index);
    applyPresetToParameters(presetManager.getCurrentPreset());
}

const juce::String OrientalInstrumentProcessor::getProgramName(int index)
{
    if (index >= 0 && index < presetManager.getNumPresets())
        return presetManager.getPreset(index).name;
    return {};
}

void OrientalInstrumentProcessor::changeProgramName(int, const juce::String&) {}

void OrientalInstrumentProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthesiser.prepare(sampleRate, samplesPerBlock);
    sampleEngine.prepare(sampleRate, samplesPerBlock);
    sampleEngine.setInstrument(static_cast<OrientalConstants::Instrument>(
        static_cast<int>(instrumentParam->load())));
    fxChain.prepare(sampleRate, samplesPerBlock);
    masterLimiter.prepare(sampleRate, samplesPerBlock);
}

void OrientalInstrumentProcessor::releaseResources() {}

bool OrientalInstrumentProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void OrientalInstrumentProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Read all parameters atomically
    float glide  = glideParam->load();
    float space  = spaceParam->load();
    float filter = filterParam->load();
    float orient = orientParam->load();
    float reso   = resoParam->load();
    float depth  = depthParam->load();
    float fxmix  = fxMixParam->load();

    // === SYNTH PARAMETERS ===

    // GLIDE: portamento time (0=instant, 100=slow glide ~1s)
    synthesiser.setGlide(glide / 100.0f);

    // FILTER: cutoff frequency mapped to audible range 80Hz - 18kHz
    float cutoffHz = 80.0f * std::pow(225.0f, filter / 100.0f); // exponential mapping
    synthesiser.setFilterCutoff(cutoffHz);

    // RESO: filter resonance Q (0.5 to 8.0)
    float q = 0.5f + (reso / 100.0f) * 7.5f;
    synthesiser.setFilterResonance(q);

    // ORIENT: vibrato/maqam micro-tuning intensity
    // Scales vibrato depth and quarter-tone deviation strength
    synthesiser.setOrientAmount(orient / 100.0f);

    // DEPTH: envelope and harmonic richness
    synthesiser.setDepth(depth / 100.0f);

    // ADSR — lecture atomique et propagation aux voix
    ADSREnvelope::Parameters adsrParams;
    adsrParams.attack  = attackParam->load();
    adsrParams.decay   = decayParam->load();
    adsrParams.sustain = sustainParam->load();
    adsrParams.release = releaseParam->load();
    synthesiser.setADSR(adsrParams);

    // SPACE: controls reverb room size + delay feedback together
    if (auto* reverb = fxChain.getReverb())
        reverb->setAmount(space / 100.0f);
    if (auto* delay = fxChain.getDelay())
        delay->setAmount(space / 200.0f); // delay is subtler

    // === FX CHAIN PARAMETERS ===
    fxChain.setDryWetMix(fxmix / 100.0f);

    // Individual FX on/off
    fxChain.getReverb()->setEnabled(reverbOnParam->load() > 0.5f);
    fxChain.getDelay()->setEnabled(delayOnParam->load() > 0.5f);
    fxChain.getChorus()->setEnabled(chorusOnParam->load() > 0.5f);
    fxChain.getDistortion()->setEnabled(distOnParam->load() > 0.5f);
    fxChain.getCompressor()->setEnabled(compOnParam->load() > 0.5f);
    fxChain.getEQ()->setEnabled(eqOnParam->load() > 0.5f);
    fxChain.getPhaser()->setEnabled(phaserOnParam->load() > 0.5f);
    fxChain.getBitcrusher()->setEnabled(bitcrushOnParam->load() > 0.5f);

    // Individual FX amounts
    fxChain.getReverb()->setAmount(reverbAmtParam->load() / 100.0f);
    fxChain.getDelay()->setAmount(delayAmtParam->load() / 100.0f);
    fxChain.getChorus()->setAmount(chorusAmtParam->load() / 100.0f);
    fxChain.getDistortion()->setAmount(distAmtParam->load() / 100.0f);
    fxChain.getCompressor()->setAmount(compAmtParam->load() / 100.0f);
    fxChain.getEQ()->setAmount(eqAmtParam->load() / 100.0f);
    fxChain.getPhaser()->setAmount(phaserAmtParam->load() / 100.0f);
    fxChain.getBitcrusher()->setAmount(bitcrushAmtParam->load() / 100.0f);

    // Render the instrument: real multisamples when the user has dropped wavs in
    // the instrument's Samples folder, otherwise the synthesis engine.
    if (sampleEngine.hasSamples())
        sampleEngine.render(buffer, midiMessages, 0, buffer.getNumSamples());
    else
        synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Process FX chain
    fxChain.process(buffer);

    // Master output protection — guarantees |out| <= ceiling on every sample
    masterLimiter.process(buffer);
}

void OrientalInstrumentProcessor::setPitchBend(float normalizedValue)
{
    // Convert -1..+1 to MIDI pitch wheel 0..16383
    int pitchWheelValue = static_cast<int>((normalizedValue + 1.0f) * 0.5f * 16383.0f);
    synthesiser.setPitchWheel(pitchWheelValue);
}

void OrientalInstrumentProcessor::setModWheel(float normalizedValue)
{
    // Send CC#1 to all voices
    synthesiser.setModWheel(normalizedValue);
}

void OrientalInstrumentProcessor::panicAllNotes()
{
    synthesiser.allNotesOff();
    sampleEngine.allNotesOff();
}

void OrientalInstrumentProcessor::setMonoMode(bool mono)
{
    synthesiser.setMonoMode(mono);
}

void OrientalInstrumentProcessor::setLegato(bool legato)
{
    synthesiser.setLegato(legato);
}

bool OrientalInstrumentProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* OrientalInstrumentProcessor::createEditor()
{
    return new OrientalInstrumentEditor(*this);
}

void OrientalInstrumentProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OrientalInstrumentProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrientalInstrumentProcessor();
}
