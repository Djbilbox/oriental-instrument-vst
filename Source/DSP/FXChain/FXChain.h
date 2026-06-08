#pragma once
#include <JuceHeader.h>

class FXBase
{
public:
    virtual ~FXBase() = default;
    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;
    virtual void setAmount(float amount) = 0;

    void setEnabled(bool enabled) { active = enabled; }
    bool isEnabled() const { return active; }

protected:
    bool active = false;
    float amount = 0.5f;
};

class FXChain
{
public:
    FXChain();
    ~FXChain() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDryWetMix(float mix);

    // Access individual FX
    FXBase* getReverb()     { return effects[0].get(); }
    FXBase* getDelay()      { return effects[1].get(); }
    FXBase* getChorus()     { return effects[2].get(); }
    FXBase* getDistortion() { return effects[3].get(); }
    FXBase* getCompressor() { return effects[4].get(); }
    FXBase* getEQ()         { return effects[5].get(); }
    FXBase* getPhaser()     { return effects[6].get(); }
    FXBase* getBitcrusher() { return effects[7].get(); }

private:
    std::vector<std::unique_ptr<FXBase>> effects;
    float dryWetMix = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FXChain)
};
