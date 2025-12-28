// PluginProcessor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

KBassGuitarAudioProcessor::KBassGuitarAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
KBassGuitarAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Drive dial: 1..30 (matches your ReaJS slider behavior)
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        kDriveID,
        "Drive",
        juce::NormalisableRange<float>(1.0f, 30.0f, 0.1f),
        20.0f
    ));

    return layout;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KBassGuitarAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (in != out) return false;
    return (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo());
}
#endif

void KBassGuitarAudioProcessor::prepareToPlay (double, int)
{
    // Match ReaJS init behavior (but true stereo)
    aL = 1.0f; bL = 0.0f;
    aR = 1.0f; bR = 0.0f;
}

void KBassGuitarAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int totalIn  = getTotalNumInputChannels();
    const int totalOut = getTotalNumOutputChannels();
    const int n        = buffer.getNumSamples();
    const int numCh    = buffer.getNumChannels();

    for (int ch = totalIn; ch < totalOut; ++ch)
        buffer.clear (ch, 0, n);

    const auto* driveParam = apvts.getRawParameterValue (kDriveID);
    const float drive = driveParam ? driveParam->load() : 20.0f;

    // EXACT constants from your ReaJS script
    constexpr float eps = 0.000001f;
    constexpr float cA  = 0.0001f;
    constexpr float cB  = 0.001f;

    // Left channel
    if (numCh >= 1)
    {
        float* x = buffer.getWritePointer (0);

        for (int i = 0; i < n; ++i)
        {
            const float l = x[i];

            // your equations (true stereo: L uses aL/bL)
            aL = (1.0f - cA) * aL + cA * (std::abs(l) * std::abs(l)) / (aL + eps);
            bL = (1.0f - cB) * bL + cB * (l / (aL + eps));

            x[i] = l + (bL * aL) * drive;
        }
    }

    // Right channel
    if (numCh >= 2)
    {
        float* x = buffer.getWritePointer (1);

        for (int i = 0; i < n; ++i)
        {
            const float r = x[i];

            // your equations (true stereo: R uses aR/bR)
            aR = (1.0f - cA) * aR + cA * (std::abs(r) * std::abs(r)) / (aR + eps);
            bR = (1.0f - cB) * bR + cB * (r / (aR + eps));

            x[i] = r + (bR * aR) * drive;
        }
    }
}

juce::AudioProcessorEditor* KBassGuitarAudioProcessor::createEditor()
{
    return new KBassGuitarAudioProcessorEditor (*this);
}

void KBassGuitarAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    apvts.state.writeToStream (mos);
}

void KBassGuitarAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto tree = juce::ValueTree::readFromData (data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState (tree);
}

//==============================================================================
// REQUIRED factory function (fixes common LNK2001 createPluginFilter errors)
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KBassGuitarAudioProcessor();
}
