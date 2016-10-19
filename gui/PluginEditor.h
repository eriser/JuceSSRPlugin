/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "JuceHeader.h"
#include "PluginAudioProcessor.h"

#include "PlugUI.h"

//==============================================================================
/**
*/
class PluginAudioProcessorEditor  : public AudioProcessorEditor,
                                    public ButtonListener
{
public:
    PluginAudioProcessorEditor (PluginAudioProcessor&);
    ~PluginAudioProcessorEditor();

    //==============================================================================

    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================

    void buttonClicked(Button* buttonThatWasClicked);

    //==============================================================================

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginAudioProcessor& processor;

    ScopedPointer<PlugUI> ui;
    ScopedPointer<ImageButton> configButton;
    Image configImg; //!< from http://iconmonstr.com/

    /**
    * Show a configuration window to set amplitude_reference_distance.
    * Later, this should be used to replace the complete SSR configuration
    * like choosing rendering method and hrir or wfs_prefilter files etc.
    */
    void showConfigWindow();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
