#pragma once
#include <JuceHeader.h>

class ChannelStrip
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        inputGain.prepare(spec);
        low.prepare(spec);
        mid.prepare(spec);
        high.prepare(spec);
        compressor.prepare(spec);
        reverb.prepare(spec);
        outputGain.prepare(spec);

        low.state = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 120.0f, 0.7071f, 1.0f);
        mid.state = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1200.0f, 0.8f, 1.0f);
        high.state = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 7000.0f, 0.7071f, 1.0f);
    }

    void reset()
    {
        inputGain.reset(); low.reset(); mid.reset(); high.reset();
        compressor.reset(); reverb.reset(); outputGain.reset();
    }

    void update(float inDb, float lowDb, float midDb, float highDb,
                float compAmount, float reverbAmount, float outDb)
    {
        inputGain.setGainDecibels(inDb);
        outputGain.setGainDecibels(outDb);

        *low.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, 120.0f, 0.7071f, juce::Decibels::decibelsToGain(lowDb));
        *mid.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, 1200.0f, 0.8f, juce::Decibels::decibelsToGain(midDb));
        *high.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, 7000.0f, 0.7071f, juce::Decibels::decibelsToGain(highDb));

        compressor.setThreshold(-6.0f - compAmount * 24.0f);
        compressor.setRatio(1.0f + compAmount * 7.0f);
        compressor.setAttack(12.0f);
        compressor.setRelease(100.0f);

        juce::dsp::Reverb::Parameters p;
        p.roomSize = 0.25f + reverbAmount * 0.45f;
        p.damping = 0.55f;
        p.wetLevel = reverbAmount * 0.35f;
        p.dryLevel = 1.0f;
        p.width = 1.0f;
        reverb.setParameters(p);
    }

    void process(juce::dsp::AudioBlock<float>& block, float saturation)
    {
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        inputGain.process(ctx);
        low.process(ctx); mid.process(ctx); high.process(ctx);
        compressor.process(ctx);

        if (saturation > 0.0001f)
        {
            const float drive = 1.0f + saturation * 5.0f;
            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            {
                auto* data = block.getChannelPointer(ch);
                for (size_t i = 0; i < block.getNumSamples(); ++i)
                    data[i] = std::tanh(data[i] * drive) / std::tanh(drive);
            }
        }

        reverb.process(ctx);
        outputGain.process(ctx);
    }

private:
    double sampleRate = 44100.0;
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>> low, mid, high;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Reverb reverb;
};
