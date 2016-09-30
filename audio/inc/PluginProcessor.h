/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "JuceHeader.h"
#include "SynthParams.h"

#include "ssr code/controller.h"
#include "ssr code/binauralrenderer.h"

//==============================================================================
/**
 * Common JUCE PluginProcessor. Derived from SynthParams for communication with UI.
 * The SSR is incorporated in this class.
 */
class PluginAudioProcessor : public AudioProcessor, public SynthParams
{
public:
    PluginAudioProcessor();
    ~PluginAudioProcessor();

    //==============================================================================

    /// plugin audio processing callbacks
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================

    /// audio processor editor stuff
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================

    /// plugin information about name, routing and midi
    const String getName() const override;

    const String getInputChannelName(int channelIndex) const override;
    const String getOutputChannelName(int channelIndex) const override;
    bool isInputChannelStereoPair(int index) const override;
    bool isOutputChannelStereoPair(int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================

    /// plugin programm getter and setter
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================

    /// host serializiation functions to read and write patches
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

private:
    std::unique_ptr<ssr::BinauralRenderer> renderer; //!< SSR renderer
    int sourceID = 0; //!< ID of created SSR source for accessing parameters

    ScopedPointer<TemporaryFile> tempHrirFile; //!< temporary hrir file to be created from memory
    bool setupSuccessful = false; //!< only create and use SSR renderer if loading hrir file was successful

    /**
     * Update host information by updating information of host audio playhead. 
     * This way you can get access to bpm or transport state of host.
     */
    void updateHostInfo();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
