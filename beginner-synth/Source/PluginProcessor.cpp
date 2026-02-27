#include "PluginProcessor.h"
#include "PluginEditor.h"

BeginnerSynthAudioProcessor::BeginnerSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
}

BeginnerSynthAudioProcessor::~BeginnerSynthAudioProcessor() = default;

const juce::String BeginnerSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BeginnerSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BeginnerSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BeginnerSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BeginnerSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BeginnerSynthAudioProcessor::getNumPrograms()
{
    return 1;
}

int BeginnerSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BeginnerSynthAudioProcessor::setCurrentProgram(int)
{
}

const juce::String BeginnerSynthAudioProcessor::getProgramName(int)
{
    return {};
}

void BeginnerSynthAudioProcessor::changeProgramName(int, const juce::String&)
{
}

void BeginnerSynthAudioProcessor::prepareToPlay(double, int)
{
}

void BeginnerSynthAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BeginnerSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    {
        return false;
    }

   #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    {
        return false;
    }
   #endif

    return true;
}
#endif

void BeginnerSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }

    for (int channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }
}

bool BeginnerSynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BeginnerSynthAudioProcessor::createEditor()
{
    return new BeginnerSynthAudioProcessorEditor(*this);
}

void BeginnerSynthAudioProcessor::getStateInformation(juce::MemoryBlock&)
{
}

void BeginnerSynthAudioProcessor::setStateInformation(const void*, int)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BeginnerSynthAudioProcessor();
}
