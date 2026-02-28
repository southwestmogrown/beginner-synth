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
    return "BeginnerSynth";
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

void BeginnerSynthAudioProcessor::prepareToPlay(double sampleRate, int)
{
  currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
  phase = 0.0;

  constexpr double testFrequencyHz = 220.0;
  phaseIncrement = testFrequencyHz / currentSampleRate;
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

void BeginnerSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    constexpr float amplitude = 0.08f;
    const int numSamples = buffer.getNumSamples();
    const int numOutputChannels = getTotalNumOutputChannels();

    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();

        if (message.isNoteOn())
            noteIsOn = true;
        else if (message.isNoteOff())
            noteIsOn = false;
    }

    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }

    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
    {

        float sampleValue = 0.0f;
        if (noteIsOn)
            sampleValue = amplitude * static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * phase));

        phase += phaseIncrement;
        if (phase >= 1.0)
            phase -= 1.0;

        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            buffer.setSample(channel, sampleIndex, sampleValue);
        }
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
