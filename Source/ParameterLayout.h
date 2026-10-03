#pragma once
#include <JuceHeader.h>

namespace predripp::params
{
inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    using APF = juce::AudioParameterFloat;
    using APB = juce::AudioParameterBool;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const std::array<juce::String, 5> stemIds { "lead", "acoustic", "sub", "electric", "drums" };
    const std::array<juce::String, 5> stemNames { "Lead", "Acoustic", "Sub", "Electric", "Drums" };

    for (size_t i = 0; i < stemIds.size(); ++i)
    {
        const auto& id = stemIds[i];
        const auto& name = stemNames[i];

        layout.add(std::make_unique<APF>(id + "Gain", name + " Gain",
            juce::NormalisableRange<float>(-60.0f, 12.0f, 0.01f), 0.0f));
        layout.add(std::make_unique<APF>(id + "Pan", name + " Pan",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.0f));
        layout.add(std::make_unique<APB>(id + "Mute", name + " Mute", false));
        layout.add(std::make_unique<APB>(id + "Solo", name + " Solo", false));
    }

    layout.add(std::make_unique<APF>("inputGain", "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<APF>("lowGain", "Low Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<APF>("midGain", "Mid Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<APF>("highGain", "High Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<APF>("compressor", "Compression",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.25f));
    layout.add(std::make_unique<APF>("saturation", "Saturation",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.15f));
    layout.add(std::make_unique<APF>("reverb", "Plate Reverb",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.12f));
    layout.add(std::make_unique<APF>("width", "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f), 1.0f));
    layout.add(std::make_unique<APF>("outputGain", "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<APB>("bypass", "Bypass", false));

    return layout;
}
}
