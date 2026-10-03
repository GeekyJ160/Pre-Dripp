#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr std::array<const char*, 5> stemIds { "lead", "acoustic", "sub", "electric", "drums" };
}

PreDrippAudioProcessor::PreDrippAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Lead", juce::AudioChannelSet::stereo(), true)
        .withInput("Acoustic", juce::AudioChannelSet::stereo(), true)
        .withInput("Sub", juce::AudioChannelSet::stereo(), true)
        .withInput("Electric", juce::AudioChannelSet::stereo(), true)
        .withInput("Drums", juce::AudioChannelSet::stereo(), true)
        .withOutput("Mix", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", predripp::params::createLayout())
{
}

void PreDrippAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getMainBusNumOutputChannels()) };
    strip.prepare(spec);
}

void PreDrippAudioProcessor::releaseResources()
{
    strip.reset();
}

bool PreDrippAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    for (int i = 0; i < layouts.inputBuses.size(); ++i)
    {
        const auto set = layouts.getChannelSet(true, i);
        if (! set.isDisabled() && set != juce::AudioChannelSet::stereo())
            return false;
    }

    return true;
}

void PreDrippAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto output = getBusBuffer(buffer, false, 0);
    output.clear();

    bool anySolo = false;
    for (const auto* id : stemIds)
        anySolo = anySolo || apvts.getRawParameterValue(juce::String(id) + "Solo")->load() > 0.5f;

    for (int bus = 0; bus < juce::jmin(getBusCount(true), static_cast<int>(stemIds.size())); ++bus)
    {
        auto input = getBusBuffer(buffer, true, bus);
        const juce::String id(stemIds[static_cast<size_t>(bus)]);

        const bool muted = apvts.getRawParameterValue(id + "Mute")->load() > 0.5f;
        const bool soloed = apvts.getRawParameterValue(id + "Solo")->load() > 0.5f;
        if (muted || (anySolo && ! soloed))
            continue;

        const float gain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(id + "Gain")->load());
        const float pan = apvts.getRawParameterValue(id + "Pan")->load();
        const float leftGain = gain * (pan <= 0.0f ? 1.0f : 1.0f - pan);
        const float rightGain = gain * (pan >= 0.0f ? 1.0f : 1.0f + pan);

        if (input.getNumChannels() > 0)
            output.addFrom(0, 0, input, 0, 0, input.getNumSamples(), leftGain);
        if (input.getNumChannels() > 1)
            output.addFrom(1, 0, input, 1, 0, input.getNumSamples(), rightGain);
    }

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

    juce::dsp::AudioBlock<float> block(output);
    strip.process(block, apvts.getRawParameterValue("saturation")->load());

    const float width = apvts.getRawParameterValue("width")->load();
    if (output.getNumChannels() >= 2 && width != 1.0f)
    {
        auto* l = output.getWritePointer(0);
        auto* r = output.getWritePointer(1);
        for (int i = 0; i < output.getNumSamples(); ++i)
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
