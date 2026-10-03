#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PreDrippAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PreDrippAudioProcessorEditor(PreDrippAudioProcessor&);
    ~PreDrippAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void configureKnob(juce::Slider&, const juce::String& suffix);

    PreDrippAudioProcessor& processor;
    juce::Label title, subtitle;

    std::array<juce::Label, 5> stemLabels;
    std::array<juce::Slider, 5> stemGain, stemPan;
    std::array<juce::ToggleButton, 5> stemMute, stemSolo;
    std::array<std::unique_ptr<SliderAttachment>, 5> stemGainA, stemPanA;
    std::array<std::unique_ptr<ButtonAttachment>, 5> stemMuteA, stemSoloA;

    juce::Slider input, low, mid, high, comp, sat, reverb, width, output;
    juce::ToggleButton bypass { "Bypass" };

    std::unique_ptr<SliderAttachment> inputA, lowA, midA, highA, compA, satA, reverbA, widthA, outputA;
    std::unique_ptr<ButtonAttachment> bypassA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreDrippAudioProcessorEditor)
};
