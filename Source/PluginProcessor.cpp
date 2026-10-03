#include "PluginProcessor.h"
#include "PluginEditor.h"

PreDrippAudioProcessor::PreDrippAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", predripp::params::createLayout())
{
}

void PreDrippAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    strip.prepare(spec);
}

void PreDrippAudioProcessor::releaseResources()
{
    strip.reset();
}

bool PreDrippAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
        && out == layouts.getMainInputChannelSet();
}

void PreDrippAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (apvts.getRawParameterValue("bypass")->load() > 0.5f)
        return;

    strip.update(
        apvts.getRawParameterValue("inputGain")->load(),
        apvts.getRawParameterValue("lowGain")->load(),
        apvts.getRawParameterValue("midGain")->load(),
        apvts.getRawParameterValue("highGain")->load(),
        apvts.getRawParameterValue("compressor")->load(),
        apvts.getRawParameterValue("reverb")->load(),
        apvts.getRawParameterValue("outputGain")->load());

    juce::dsp::AudioBlock<float> block(buffer);
    strip.process(block, apvts.getRawParameterValue("saturation")->load());

    const float width = apvts.getRawParameterValue("width")->load();
    if (buffer.getNumChannels() >= 2 && width != 1.0f)
    {
        auto* l = buffer.getWritePointer(0);
        auto* r = buffer.getWritePointer(1);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float mid = 0.5f * (l[i] + r[i]);
            const float side = 0.5f * (l[i] - r[i]) * width;
            l[i] = mid + side;
            r[i] = mid - side;
        }
    }
}

juce::AudioProcessorEditor* PreDrippAudioProcessor::createEditor()
{
    return new PreDrippAudioProcessorEditor(*this);
}

void PreDrippAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void PreDrippAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PreDrippAudioProcessor();
}
