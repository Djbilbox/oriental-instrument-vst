#pragma once
#include <JuceHeader.h>

/**
    Master output protection — last stage before the plugin output.

    Two cascaded stages (priority: real-time cost → gain safety → readability):

      1. SOFT  : juce::dsp::Limiter — transparent brick-wall peak control
                 (threshold -1 dBFS, fast attack, musical release). Tames the
                 summed polyphony (up to 16 voices) + FX makeup gain without
                 audible pumping.

      2. HARD  : absolute ceiling clamp at ±kCeilingLinear. Catches any overshoot
                 the limiter lets through during its attack, inter-sample peaks,
                 or pathological input. Guarantees |out| <= kCeilingLinear on
                 EVERY sample → protects speakers and hearing.

    Header-only and allocation-free in the audio thread (matches the project's
    BlurUtils.h / Typography.h convention → no CMake/Projucer registration).
*/
class MasterLimiter
{
public:
    void prepare(double sampleRate, int samplesPerBlock)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate       = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels      = 2;

        limiter.prepare(spec);
        limiter.setThreshold(kThresholdDb);
        limiter.setRelease(kReleaseMs);
    }

    void reset()
    {
        limiter.reset();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        // Stage 1 — soft brick-wall limiter.
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        limiter.process(context);

        // Stage 2 — absolute hard ceiling (safety net, transparent in normal use).
        const int numChannels = buffer.getNumChannels();
        const int numSamples  = buffer.getNumSamples();
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
                data[i] = juce::jlimit(-kCeilingLinear, kCeilingLinear, data[i]);
        }
    }

private:
    juce::dsp::Limiter<float> limiter;

    static constexpr float kThresholdDb   = -1.0f;   // soft limiter ceiling (dBFS)
    static constexpr float kReleaseMs     = 50.0f;   // musical release, no pumping
    static constexpr float kCeilingLinear = 0.98f;   // hard clamp ≈ -0.18 dBFS
};
