// PluginEditor.cpp
#include "PluginEditor.h"

KBassGuitarAudioProcessorEditor::KBassGuitarAudioProcessorEditor (KBassGuitarAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (260, 180);

    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    driveSlider.setRange (1.0, 30.0, 0.1);
    addAndMakeVisible (driveSlider);

    driveLabel.setText ("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (driveLabel);

    driveAttachment = std::make_unique<Attachment>(audioProcessor.apvts, KBassGuitarAudioProcessor::kDriveID, driveSlider);
}

void KBassGuitarAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawFittedText ("KBassGuitar", getLocalBounds().removeFromTop(34),
                      juce::Justification::centred, 1);

    g.setFont (12.0f);
    g.setColour (juce::Colours::grey);
    g.drawFittedText ("True-stereo envelope drive", getLocalBounds().removeFromBottom(22),
                      juce::Justification::centred, 1);
}

void KBassGuitarAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (40);

    driveLabel.setBounds (area.removeFromTop (18));
    driveSlider.setBounds (area.reduced (10));
}
