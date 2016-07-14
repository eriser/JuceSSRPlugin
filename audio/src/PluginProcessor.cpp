/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "HostParam.h"

// UI header, should be hidden behind a factory
#include <PluginEditor.h>

#include "ssr code/configuration.h"

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
{
    positionInfo[0].resetToDefault();
    positionInfo[1].resetToDefault();
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const String PluginAudioProcessor::getName() const
{
#ifdef JucePlugin_Name
    return JucePlugin_Name;
#else
    // standalone
    return "plugin";
#endif
}

const String PluginAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PluginAudioProcessor::isInputChannelStereoPair (int index) const
{
    ignoreUnused(index);
    return true;
}

bool PluginAudioProcessor::isOutputChannelStereoPair (int index) const
{
    ignoreUnused(index);
    return true;
}

bool PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram (int index)
{
    ignoreUnused(index);
}

const String PluginAudioProcessor::getProgramName (int index)
{
    ignoreUnused(index);
    return String();
}

void PluginAudioProcessor::changeProgramName (int index, const String& newName)
{
    ignoreUnused(index,newName);
}

namespace {
    char* ssr_argv[] = { "ssr_juce", "--binaural", "--no-ip-server", "--no-gui", "--no-tracker", "--verbose" };
    int ssr_argc = sizeof(ssr_argv)/sizeof(ssr_argv[0]);
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay (double sRate, int samplesPerBlock)
{
    auto conf = ssr::configuration(ssr_argc, ssr_argv);
    conf.renderer_params.set<int>("sample_rate", static_cast<int>(sRate));
    conf.renderer_params.set<int>("block_size", samplesPerBlock);
    conf.renderer_params.set<int>("in_channels", getNumInputChannels());
    conf.renderer_params.set<int>("out_channels", getNumOutputChannels());
    
    renderer.reset(new ssr::BinauralRenderer(conf.renderer_params));
    renderer->load_reproduction_setup();

    apf::parameter_map sourceParam;
    //sourceParam.set("connect_to", "1");
    renderer->add_source(sourceParam); // NOTE: add_source kein output
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void PluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ignoreUnused(midiMessages);
    updateHostInfo();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // I've added this to avoid people getting screaming feedback
    // when they first compile the plugin, but obviously you don't need to
    // this code if your algorithm already fills all the output channels.
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //// test sinus input erstellen
    //float val = 0.f;
    //for (int s = 0; s < buffer.getNumSamples(); ++s) {
    //    val = sinf((2.f * float_Pi / static_cast<float>(getSampleRate())) * 440.f * static_cast<float>(s) + static_cast<float>(offset));
    //    buffer.setSample(0, s, val);
    //    buffer.setSample(1, s, val);
    //}
    //offset += buffer.getNumSamples();

    // testwise set position
    ssr::RendererBase<ssr::BinauralRenderer>::Source * source = renderer->get_source(1);
    Position pos(SynthParams::xPos.get(), SynthParams::yPos.get());
    source->position = pos;

    // mimoprocessor_file_io.h
    //renderer->activate();
    renderer->audio_callback(getBlockSize()
        , buffer.getArrayOfWritePointers() // NOTE: write ~ read pointer (selbe adresse), read aber const
        , buffer.getArrayOfWritePointers());
    //renderer->deactivate();

    // master volume
    for (int c = 0; c < buffer.getNumChannels(); ++c)
    {
        FloatVectorOperations::multiply(buffer.getWritePointer(c, 0), SynthParams::masterVol.get(), buffer.getNumSamples());
    }
}

void PluginAudioProcessor::updateHostInfo()
{
    // currentPositionInfo used for getting the bpm.
    if (AudioPlayHead* pHead = getPlayHead())
    {
        if (pHead->getCurrentPosition (positionInfo[getAudioIndex()])) {
            positionIndex.exchange(getGUIIndex());
            return;
        }
    }
    positionInfo[getAudioIndex()].resetToDefault();
}

//==============================================================================
bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginAudioProcessorEditor (*this);
}

//==============================================================================
void PluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ignoreUnused(destData);
}

void PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}
