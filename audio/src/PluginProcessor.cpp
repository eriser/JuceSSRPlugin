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

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
{
    // add automation params for host
    // NOTE: make sure automation in host is not interpolated for more precision
    // e.g. in Cubase by setting reduction level to 0%
    addParameter(new HostParam<Param>(sourceX));
    addParameter(new HostParam<Param>(sourceY));
    addParameter(new HostParam<Param>(sourceVol));
    addParameter(new HostParam<ParamStepped<eOnOffState>>(sourceMute));

    addParameter(new HostParam<Param>(referenceX));
    addParameter(new HostParam<Param>(referenceY));
    addParameter(new HostParam<Param>(referenceOrientation));
}

PluginAudioProcessor::~PluginAudioProcessor()
{
    tempHrirFile = nullptr;
}

//==============================================================================
const String PluginAudioProcessor::getName() const
{
#ifdef JucePlugin_Name
    return JucePlugin_Name;
#else
    return "juceSSR plugin";
#endif
}

const String PluginAudioProcessor::getInputChannelName(int channelIndex) const
{
    return String(channelIndex + 1);
}

const String PluginAudioProcessor::getOutputChannelName(int channelIndex) const
{
    return String(channelIndex + 1);
}

bool PluginAudioProcessor::isInputChannelStereoPair(int index) const
{
    ignoreUnused(index);
    return true;
}

bool PluginAudioProcessor::isOutputChannelStereoPair(int index) const
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

//==============================================================================

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram(int index)
{
    ignoreUnused(index);
}

const String PluginAudioProcessor::getProgramName(int index)
{
    ignoreUnused(index);
    return String();
}

void PluginAudioProcessor::changeProgramName(int index, const String& newName)
{
    ignoreUnused(index, newName);
}

//==============================================================================

// SSR configuration args stuff
namespace {
    char* ssr_argv[] = { "ssr_juce", "--binaural", "--no-ip-server", "--no-gui", "--no-tracker", "--verbose" };
    int ssr_argc = sizeof(ssr_argv) / sizeof(ssr_argv[0]);
}

void PluginAudioProcessor::prepareToPlay(double sRate, int samplesPerBlock)
{
    // read default_hrirs_wav from memory into hrirFileBuffer
    MemoryInputStream *in = new MemoryInputStream(BinaryData::default_hrirs_wav, BinaryData::default_hrirs_wavSize, false);
    ScopedPointer<AudioFormatReader> hrirFileReader = WavAudioFormat().createReaderFor(in, true);
    double hrirFileSampleRate = hrirFileReader->sampleRate;
    int numHrirFileChannels = static_cast<int>(hrirFileReader->numChannels);
    int hrirFileLengthInSamples = static_cast<int>(hrirFileReader->lengthInSamples);

    AudioSampleBuffer hrirFileBuffer(numHrirFileChannels, hrirFileLengthInSamples);
    hrirFileReader->read(&hrirFileBuffer, 0, hrirFileLengthInSamples, 0, true, true);

    // resample default hrir buffer if necessary
    AudioSampleBuffer resampledHrirFileBuffer;
    if (sRate != hrirFileSampleRate)
    {
        ScopedPointer<LagrangeInterpolator> resampler = new LagrangeInterpolator();
        double speedRatio = hrirFileSampleRate / sRate;
        int resampledLengthInSamples = static_cast<int>(hrirFileLengthInSamples / speedRatio);
        resampledHrirFileBuffer = AudioSampleBuffer(numHrirFileChannels, resampledLengthInSamples);

        const float **readBuffer = hrirFileBuffer.getArrayOfReadPointers();
        float **writeBuffer = resampledHrirFileBuffer.getArrayOfWritePointers();
        for (int i = 0; i < numHrirFileChannels; ++i)
        {
            resampler->process(speedRatio, readBuffer[i], writeBuffer[i], resampledLengthInSamples);
        }
    }

    // create interleaved audio data buffer for writing multi channel wav with libsndfile
    AudioSampleBuffer *bufferToUse = sRate == hrirFileSampleRate ? &hrirFileBuffer : &resampledHrirFileBuffer;
    int numFrames = numHrirFileChannels * bufferToUse->getNumSamples();
    float *interleavedBuffer = (float*)malloc(numFrames * sizeof(float));
    long k = 0;

    for (long i = 0; i < bufferToUse->getNumSamples(); i++)
    {
        for (int j = 0; j < numHrirFileChannels; j++)
        {
            interleavedBuffer[k + j] = bufferToUse->getSample(j, i);
        }
        k += numHrirFileChannels;
    }

    // create temporary file and save path of randomly named tempFile
    tempHrirFile = new TemporaryFile(".wav");
    String tempHrirFilePath = tempHrirFile->getFile().getFullPathName();

    // write wav into TemporaryFile
    const char *path = tempHrirFilePath.getCharPointer();
    auto format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    SndfileHandle sndFile = SndfileHandle(path, SFM_WRITE, format, numHrirFileChannels, static_cast<int>(sRate));
    sndFile.write(interleavedBuffer, numFrames);

    // configure SSR
    auto conf = ssr::configuration(ssr_argc, ssr_argv);
    conf.renderer_params.set<int>("sample_rate", static_cast<int>(sRate));
    conf.renderer_params.set<int>("block_size", samplesPerBlock);
    conf.renderer_params.set<int>("in_channels", getNumInputChannels());
    conf.renderer_params.set<int>("out_channels", getNumOutputChannels());

    conf.renderer_params.set("hrir_size", 0); // "0" means use all that are there
    conf.renderer_params.set("hrir_file", tempHrirFilePath);

    try
    {
        // create renderer
        renderer.reset(new ssr::BinauralRenderer(conf.renderer_params));
        renderer->load_reproduction_setup();

        // add mono input source
        apf::parameter_map sourceParam;
        sourceID = renderer->add_source(sourceParam);

        setupSuccessful = true;
    }
    catch (...)
    {
        setupSuccessful = false;
    }
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void PluginAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ignoreUnused(midiMessages);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // I've added this to avoid people getting screaming feedback
    // when they first compile the plugin, but obviously you don't need to
    // this code if your algorithm already fills all the output channels.
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (setupSuccessful)
    {
        /// \todo how to handle mono input from host correctly?
        // choose between left or right channel for stereo input
        int channelIndex = static_cast<int>(inputChannel.getStep());

        // get source input level
        float inputLevel;
        bool isInputMuted = sourceMute.getStep() == eOnOffState::eOn;
        isInputMuted ? inputLevel = 0.0f : inputLevel = buffer.getRMSLevel(channelIndex, 0, buffer.getNumSamples());
        sourceLevel.set(Param::toDb(inputLevel), true);

        // set listener parameter
        Position listenerPos = Position(referenceX.get(), referenceY.get());
        renderer->state.reference_position.setRT(listenerPos);
        renderer->state.reference_orientation.setRT(Orientation(referenceOrientation.get() + refOrientationOffset));
        renderer->state.amplitude_reference_distance.setRT(amplitudeReferenceDistance.get());

        // set source parameter
        ssr::RendererBase<ssr::BinauralRenderer>::Source *source = renderer->get_source(sourceID);
        Position sourcePos = Position(sourceX.get(), sourceY.get());
        source->position.setRT(sourcePos);
        source->orientation.setRT((listenerPos - sourcePos).orientation()); // quserinterface.cpp
        Source::model_t type;
        sourceType.getStep() == eSourceType::ePlane ? type = Source::model_t::plane : type = Source::model_t::point;
        source->model.setRT(type);
        source->mute.setRT(isInputMuted);
        source->gain.setRT(Param::fromDb(sourceVol.get()));

        // calculate angle from which the source is seen
        // and confine angle to interval ]-180, 180], see qsourceproperties.cpp
        float ang = source->orientation.get().azimuth - referenceOrientation.get() + refOrientationOffset;
        ang = std::fmod(ang, 360.0f);
        if (ang > 180.0f)
        {
            ang -= 360.0f;
        }
        else if (ang <= -180.0f)
        {
            ang += 360.0f;
        }
        sourceOrientation.setUI(ang);

        // call internal ssr process function of renderer
        renderer->audio_callback(getBlockSize()
            , buffer.getArrayOfWritePointers() + channelIndex // NOTE: write = read pointer, read is just const
            , buffer.getArrayOfWritePointers());

        // get ssr output level
        outputLevelLeft.set(Param::toDb(buffer.getRMSLevel(0, 0, buffer.getNumSamples())), true);
        outputLevelRight.set(Param::toDb(buffer.getRMSLevel(1, 0, buffer.getNumSamples())), true);
    }
    else
    {
        buffer.clear();
    }
    /// \todo getCPUUsage() einbauen
}

//==============================================================================

bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginAudioProcessorEditor(*this);
}

//==============================================================================

void PluginAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    SynthParams::writeXMLPatchHost(destData);
}

void PluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    SynthParams::readXMLPatchHost(data, sizeInBytes);
}

//==============================================================================

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}
