// PluginEditor.h
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class KBassGuitarAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit KBassGuitarAudioProcessorEditor (KBassGuitarAudioProcessor&);
    ~KBassGuitarAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    KBassGuitarAudioProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Label  driveLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KBassGuitarAudioProcessorEditor)
};
