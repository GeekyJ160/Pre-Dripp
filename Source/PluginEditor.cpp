#include "PluginEditor.h"

namespace
{
constexpr std::array<const char*, 5> stemIds { "lead", "acoustic", "sub", "electric", "drums" };
constexpr std::array<const char*, 5> stemNames { "LEAD", "ACOUSTIC", "SUB", "ELECTRIC", "DRUMS" };
}

PreDrippAudioProcessorEditor::PreDrippAudioProcessorEditor(PreDrippAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(980, 660);
    setResizable(true, true);
    setResizeLimits(760, 560, 1500, 980);

    title.setText("PRE-DRIPP", juce::dontSendNotification);
    title.setFont(juce::Font(30.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(title);

    subtitle.setText("STEM MIXER + CHANNEL STRIP", juce::dontSendNotification);
    subtitle.setFont(juce::Font(13.0f, juce::Font::bold));
    subtitle.setColour(juce::Label::textColourId, juce::Colour::fromRGB(93, 226, 255));
    addAndMakeVisible(subtitle);

    auto& v = processor.apvts;

    for (size_t i = 0; i < stemIds.size(); ++i)
    {
        const juce::String id(stemIds[i]);

        stemLabels[i].setText(stemNames[i], juce::dontSendNotification);
        stemLabels[i].setJustificationType(juce::Justification::centred);
        stemLabels[i].setFont(juce::Font(12.0f, juce::Font::bold));
        stemLabels[i].setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(stemLabels[i]);

        configureKnob(stemGain[i], " dB");
        configureKnob(stemPan[i], "");
        stemPan[i].setDoubleClickReturnValue(true, 0.0);
        addAndMakeVisible(stemGain[i]);
        addAndMakeVisible(stemPan[i]);

        stemMute[i].setButtonText("M");
        stemSolo[i].setButtonText("S");
        addAndMakeVisible(stemMute[i]);
        addAndMakeVisible(stemSolo[i]);

        stemGainA[i] = std::make_unique<SliderAttachment>(v, id + "Gain", stemGain[i]);
        stemPanA[i] = std::make_unique<SliderAttachment>(v, id + "Pan", stemPan[i]);
        stemMuteA[i] = std::make_unique<ButtonAttachment>(v, id + "Mute", stemMute[i]);
        stemSoloA[i] = std::make_unique<ButtonAttachment>(v, id + "Solo", stemSolo[i]);
    }

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

    inputA = std::make_unique<SliderAttachment>(v, "inputGain", input);
    lowA = std::make_unique<SliderAttachment>(v, "lowGain", low);
    midA = std::make_unique<SliderAttachment>(v, "midGain", mid);
    highA = std::make_unique<SliderAttachment>(v, "highGain", high);
    compA = std::make_unique<SliderAttachment>(v, "compressor", comp);
    satA = std::make_unique<SliderAttachment>(v, "saturation", sat);
    reverbA = std::make_unique<SliderAttachment>(v, "reverb", reverb);
    widthA = std::make_unique<SliderAttachment>(v, "width", width);
    outputA = std::make_unique<SliderAttachment>(v, "outputGain", output);
    bypassA = std::make_unique<ButtonAttachment>(v, "bypass", bypass);
}

void PreDrippAudioProcessorEditor::configureKnob(juce::Slider& s, const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 20);
    s.setTextValueSuffix(suffix);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(0, 229, 255));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB(31, 48, 65));
    s.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(130, 245, 255));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void PreDrippAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient bg(juce::Colour::fromRGB(5, 9, 16), 0, 0,
                            juce::Colour::fromRGB(13, 24, 38), 0, (float)getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();

    auto outer = getLocalBounds().toFloat().reduced(16.0f);
    g.setColour(juce::Colour::fromRGB(18, 30, 45));
    g.fillRoundedRectangle(outer, 24.0f);
    g.setColour(juce::Colour::fromRGBA(0, 229, 255, 42));
    g.drawRoundedRectangle(outer, 24.0f, 1.5f);

    auto stemPanel = juce::Rectangle<float>(28.0f, 100.0f, getWidth() - 56.0f, 250.0f);
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 80));
    g.fillRoundedRectangle(stemPanel, 18.0f);

    g.setColour(juce::Colour::fromRGB(118, 139, 158));
    g.setFont(12.0f);
    g.drawText("MULTI-BUS STEM MIXER", stemPanel.reduced(14.0f).removeFromTop(20.0f),
               juce::Justification::left);

    g.setColour(juce::Colour::fromRGB(118, 139, 158));
    g.drawText("CHANNEL STRIP", 32, 380, 220, 20, juce::Justification::left);
}

void PreDrippAudioProcessorEditor::resized()
{
    title.setBounds(30, 24, 230, 40);
    subtitle.setBounds(32, 61, 260, 24);
    bypass.setBounds(getWidth() - 125, 34, 90, 26);

    auto stemArea = juce::Rectangle<int>(34, 124, getWidth() - 68, 212);
    const int cardW = stemArea.getWidth() / 5;

    for (int i = 0; i < 5; ++i)
    {
        auto card = stemArea.removeFromLeft(cardW).reduced(5);
        stemLabels[(size_t)i].setBounds(card.removeFromTop(24));

        auto knobArea = card.removeFromTop(118);
        stemGain[(size_t)i].setBounds(knobArea.removeFromLeft(knobArea.getWidth() / 2));
        stemPan[(size_t)i].setBounds(knobArea);

        auto buttons = card.removeFromTop(28).withSizeKeepingCentre(76, 28);
        stemMute[(size_t)i].setBounds(buttons.removeFromLeft(34));
        buttons.removeFromLeft(8);
        stemSolo[(size_t)i].setBounds(buttons.removeFromLeft(34));
    }

    auto stripArea = juce::Rectangle<int>(30, 408, getWidth() - 60, getHeight() - 438);
    const int gap = 5;
    const int w = (stripArea.getWidth() - gap * 8) / 9;

    for (auto* s : { &input, &low, &mid, &high, &comp, &sat, &reverb, &width, &output })
    {
        s->setBounds(stripArea.removeFromLeft(w));
        stripArea.removeFromLeft(gap);
    }
}
