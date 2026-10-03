#include "PluginEditor.h"

PreDrippAudioProcessorEditor::PreDrippAudioProcessorEditor(PreDrippAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(840, 520);
    setResizable(true, true);

    title.setText("PRE-DRIPP", juce::dontSendNotification);
    title.setFont(juce::Font(28.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(title);

    configureKnob(input, " dB");
    configureKnob(low, " dB");
    configureKnob(mid, " dB");
    configureKnob(high, " dB");
    configureKnob(comp, "");
    configureKnob(sat, "");
    configureKnob(reverb, "");
    configureKnob(width, "x");
    configureKnob(output, " dB");

    for (auto* s : { &input, &low, &mid, &high, &comp, &sat, &reverb, &width, &output })
        addAndMakeVisible(*s);

    addAndMakeVisible(bypass);

    auto& v = processor.apvts;
    inputA = std::make_unique<SliderAttachment>(v, "inputGain", input);
    lowA = std::make_unique<SliderAttachment>(v, "lowGain", low);
    midA = std::make_unique<SliderAttachment>(v, "midGain", mid);
    highA = std::make_unique<SliderAttachment>(v, "highGain", high);
    compA = std::make_unique<SliderAttachment>(v, "compressor", comp);
    satA = std::make_unique<SliderAttachment>(v, "saturation", sat);
    reverbA = std::make_unique<SliderAttachment>(v, "reverb", reverb);
    widthA = std::make_unique<SliderAttachment>(v, "width", width);
    outputA = std::make_unique<SliderAttachment>(v, "outputGain", output);
    bypassA = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(v, "bypass", bypass);
}

void PreDrippAudioProcessorEditor::configureKnob(juce::Slider& s, const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 22);
    s.setTextValueSuffix(suffix);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(0, 229, 255));
    s.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 229, 255));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void PreDrippAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient bg(juce::Colour::fromRGB(7, 12, 22), 0, 0,
                            juce::Colour::fromRGB(14, 24, 38), 0, (float)getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();

    g.setColour(juce::Colour::fromRGBA(0, 229, 255, 28));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 24.0f);

    g.setColour(juce::Colour::fromRGB(125, 145, 165));
    g.setFont(14.0f);
    g.drawText("Channel Strip Foundation", 28, 66, 260, 24, juce::Justification::left);
}

void PreDrippAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(28);
    title.setBounds(area.removeFromTop(42));
    bypass.setBounds(getWidth() - 120, 28, 90, 28);
    area.removeFromTop(34);

    const int gap = 8;
    const int w = (area.getWidth() - gap * 4) / 5;
    const int h = 170;

    auto top = area.removeFromTop(h);
    for (auto* s : { &input, &low, &mid, &high, &comp })
    {
        s->setBounds(top.removeFromLeft(w));
        top.removeFromLeft(gap);
    }

    area.removeFromTop(12);
    auto bottom = area.removeFromTop(h);
    for (auto* s : { &sat, &reverb, &width, &output })
    {
        s->setBounds(bottom.removeFromLeft(w));
        bottom.removeFromLeft(gap);
    }
}
