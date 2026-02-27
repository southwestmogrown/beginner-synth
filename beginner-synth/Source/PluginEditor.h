#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class BeginnerSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BeginnerSynthAudioProcessorEditor(BeginnerSynthAudioProcessor&);
    ~BeginnerSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BeginnerSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeginnerSynthAudioProcessorEditor)
};
