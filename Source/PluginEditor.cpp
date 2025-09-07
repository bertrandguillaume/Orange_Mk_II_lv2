/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ToneStackAudioProcessorEditor::ToneStackAudioProcessorEditor (ToneStackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    titleLabel.setText("Orange Mk II 1971", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    trebleSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    trebleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(trebleSlider);
    trebleSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "treble", trebleSlider);
    trebleLabel.setText("Treble", juce::dontSendNotification);
    trebleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trebleLabel);

    
    bassSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(bassSlider);
    bassLabel.setText("Bass", juce::dontSendNotification);
    bassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassLabel);
    bassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "bass", bassSlider);


    setSize (200, 150);
}

ToneStackAudioProcessorEditor::~ToneStackAudioProcessorEditor()
{
}

//==============================================================================
void ToneStackAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void ToneStackAudioProcessorEditor::resized()
{
    int sliderWidth = 80;
    int sliderHeight = 80;
    int padding = 20;

    titleLabel.setBounds(0, 5, getWidth(), 20);

    bassSlider.setBounds(padding, 50, sliderWidth, sliderHeight); // x, y, width, height
    bassLabel.setBounds(padding, 30, sliderWidth, 20);
    
    trebleSlider.setBounds(200 - sliderWidth - padding, 50, sliderWidth, sliderHeight); // x, y, width, height
    trebleLabel.setBounds(200 - sliderWidth - padding, 30, sliderWidth, 20);
}
