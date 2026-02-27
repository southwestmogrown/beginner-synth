#include "PluginEditor.h"

BeginnerSynthAudioProcessorEditor::BeginnerSynthAudioProcessorEditor(BeginnerSynthAudioProcessor& processor)
    : AudioProcessorEditor(&processor), audioProcessor(processor)
{
    setSize(500, 300);
}

BeginnerSynthAudioProcessorEditor::~BeginnerSynthAudioProcessorEditor() = default;

void BeginnerSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("Beginner Synth (Day 1)", getLocalBounds(), juce::Justification::centred, 1);
}

void BeginnerSynthAudioProcessorEditor::resized()
{
}
