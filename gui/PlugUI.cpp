/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.2.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "panels/SourceMenuPanel.h"
//[/Headers]

#include "PlugUI.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PlugUI::PlugUI (SynthParams &p)
    : PanelBase(p), params(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    startTimerHz (60);
    //[/Constructor_pre]

    addAndMakeVisible (debugText = new Label ("debug",
                                              String::empty));
    debugText->setFont (Font (15.00f, Font::plain));
    debugText->setJustificationType (Justification::topLeft);
    debugText->setEditable (false, false, false);
    debugText->setColour (TextEditor::textColourId, Colours::black);
    debugText->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (levelMeterRight = new Slider ("level right"));
    levelMeterRight->setRange (0, 1, 0);
    levelMeterRight->setSliderStyle (Slider::LinearVertical);
    levelMeterRight->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    levelMeterRight->setColour (Slider::thumbColourId, Colour (0xff60ff60));
    levelMeterRight->addListener (this);

    addAndMakeVisible (levelMeterLeft = new Slider ("level left"));
    levelMeterLeft->setRange (0, 1, 0);
    levelMeterLeft->setSliderStyle (Slider::LinearVertical);
    levelMeterLeft->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    levelMeterLeft->setColour (Slider::thumbColourId, Colour (0xff60ff60));
    levelMeterLeft->addListener (this);

    addAndMakeVisible (zoomSlider = new ZoomSlider ("zoom slider"));
    zoomSlider->setRange (25, 200, 1);
    zoomSlider->setSliderStyle (Slider::LinearBar);
    zoomSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 24);
    zoomSlider->addListener (this);

    addAndMakeVisible (listener = new ListenerComponent (params));
    listener->setName ("listener");

    addAndMakeVisible (sourceVolSlider = new VolLevelSlider ("source vol slider"));
    sourceVolSlider->setRange (-96, 12, 0);
    sourceVolSlider->setSliderStyle (Slider::LinearBar);
    sourceVolSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    sourceVolSlider->addListener (this);
    sourceVolSlider->setSkewFactor (3);

    addAndMakeVisible (sourceBackground = new SourceBackgroundComponent());
    sourceBackground->setName ("source background");

    addAndMakeVisible (sourceMenu = new DocumentWindow ("source menu", Colours::white, DocumentWindow::closeButton));
    sourceMenu->setName ("source menu");

    addAndMakeVisible (sourceNode = new SourceNodeComponent (params, sourceVolSlider, sourceBackground, sourceMenu));
    sourceNode->setName ("source node");

    drawable1 = Drawable::createFromImageData (BinaryData::ssr_logo_png, BinaryData::ssr_logo_pngSize);

    //[UserPreSize]
    sourceMenu->setTitleBarHeight(0);
    sourceMenu->setDraggable(false);
    sourceMenu->setVisible(false);
    sourceMenu->setContentOwned(new SourceMenuPanel(params, sourceNode), true);

    zoomSlider->setValue(100.0);
    zoomSlider->setTextValueSuffix(params.zoomFactor.getUnit());

    /// \todo create actual output vol level component
    levelMeterLeft->setSliderStyle(Slider::LinearBarVertical);
    levelMeterLeft->setInterceptsMouseClicks(false, false);
    levelMeterRight->setSliderStyle(Slider::LinearBarVertical);
    levelMeterRight->setInterceptsMouseClicks(false, false);
    debugText->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (900, 600);


    //[Constructor] You can add your own custom stuff here..
    registerListener(listener, &params.referenceX, &params.referenceY, &params.referenceOrientation, getWidth(), getHeight());
    registerSource(sourceNode, &params.sourceX, &params.sourceY, &params.sourceVol, getWidth(), getHeight());

    lnf = new CustomLookAndFeel();
    LookAndFeel::setDefaultLookAndFeel(lnf);
    //[/Constructor]
}

PlugUI::~PlugUI()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    lnf = nullptr;
    sourceMenu = nullptr;
    //[/Destructor_pre]

    debugText = nullptr;
    levelMeterRight = nullptr;
    levelMeterLeft = nullptr;
    zoomSlider = nullptr;
    listener = nullptr;
    sourceVolSlider = nullptr;
    sourceBackground = nullptr;
    sourceMenu = nullptr;
    sourceNode = nullptr;
    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PlugUI::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffedede6));

    g.setColour (Colours::black);
    jassert (drawable1 != 0);
    if (drawable1 != 0)
        drawable1->drawWithin (g, Rectangle<float> (23, 520, 64, 64),
                               RectanglePlacement::centred, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    float centerPointSize = 3.5f * params.zoomFactor.get() / 100.0f;

    Path centerPoint;
    centerPoint.startNewSubPath(getWidth() / 2.0f - centerPointSize, getHeight() / 2.0f);
    centerPoint.lineTo(getWidth() / 2.0f + centerPointSize, getHeight() / 2.0f);
    centerPoint.startNewSubPath(getWidth() / 2.0f, getHeight() / 2.0f - centerPointSize);
    centerPoint.lineTo(getWidth() / 2.0f, getHeight() / 2.0f + centerPointSize);

    g.setColour(Colours::grey);
    g.strokePath(centerPoint, PathStrokeType(jmin(1.0f, 2.5f * params.zoomFactor.get() / 100.0f)));
    //[/UserPaint]
}

void PlugUI::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    debugText->setBounds (648, 8, 250, 584);
    levelMeterRight->setBounds (856, 150, 24, 300);
    levelMeterLeft->setBounds (824, 150, 24, 300);
    zoomSlider->setBounds (808, 568, 80, 24);
    listener->setBounds (405, 255, 90, 90);
    sourceVolSlider->setBounds (418, 155, 64, 16);
    sourceBackground->setBounds (360, 10, 180, 180);
    sourceMenu->setBounds (512, 100, 250, 225);
    sourceNode->setBounds (405, 54, 90, 90);
    //[UserResized] Add your own custom resize handling here..
    int listenerW = static_cast<int>(listener->getWidth() * params.zoomFactor.get() / 100.0f);
    int listenerH = static_cast<int>(listener->getHeight() * params.zoomFactor.get() / 100.0f);
    juce::Point<int> pixPosRef = params.pos2pix(params.referenceX.get(), params.referenceY.get(), getWidth(), getHeight());
    listener->setBounds(pixPosRef.x - listenerW / 2, pixPosRef.y - listenerH / 2, listenerW, listenerH);

    int sourceNodeSize = static_cast<int>(sourceNode->getWidth() * params.zoomFactor.get() / 100.0f);
    juce::Point<int> pixPosSource = params.pos2pix(params.sourceX.get(), params.sourceY.get(), getWidth(), getHeight());
    sourceNode->setBounds(pixPosSource.x - sourceNodeSize / 2, pixPosSource.y - sourceNodeSize / 2, sourceNodeSize, sourceNodeSize);
    //[/UserResized]
}

void PlugUI::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    float val = static_cast<float>(sliderThatWasMoved->getValue());
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == levelMeterRight)
    {
        //[UserSliderCode_levelMeterRight] -- add your slider handling code here..
        //[/UserSliderCode_levelMeterRight]
    }
    else if (sliderThatWasMoved == levelMeterLeft)
    {
        //[UserSliderCode_levelMeterLeft] -- add your slider handling code here..
        //[/UserSliderCode_levelMeterLeft]
    }
    else if (sliderThatWasMoved == zoomSlider)
    {
        //[UserSliderCode_zoomSlider] -- add your slider handling code here..
        params.zoomFactor.setUI(val);
        resized();
        //[/UserSliderCode_zoomSlider]
    }
    else if (sliderThatWasMoved == sourceVolSlider)
    {
        //[UserSliderCode_sourceVolSlider] -- add your slider handling code here..
        params.sourceVol.setUI(val);
        //[/UserSliderCode_sourceVolSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PlugUI::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    ignoreUnused(e);
    zoomSlider->setValue(100.0);
    //[/UserCode_mouseDoubleClick]
}

void PlugUI::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    //[UserCode_mouseWheelMove] -- Add your code here...
    ignoreUnused(e);
    float delta = wheel.deltaY;
    double currZoom = zoomSlider->getValue();
    zoomSlider->setValue(currZoom + 15.0 * delta);
    //[/UserCode_mouseWheelMove]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PlugUI::childBoundsChanged(Component *child)
{
    if (child == listener || child == sourceNode)
    {
        if (child == sourceNode)
        {
            int sourceNodeSize = static_cast<int>(sourceNode->getWidth());

            // sourceMenu, sourceVolSlider and sourceBackground should always follow sourceNode
            int offsetX = sourceNode->getX() + sourceNodeSize + 25;
            int offsetY = sourceNode->getY() + sourceNodeSize / 2;
            sourceMenu->setBounds(offsetX, offsetY, sourceMenu->getWidth(), sourceMenu->getHeight());

            sourceVolSlider->setSize(sourceNodeSize * 3 / 4, sourceNodeSize / 3);
            offsetX = sourceNode->getX() + (sourceNodeSize - sourceVolSlider->getWidth()) / 2;
            offsetY = sourceNode->getY() + static_cast<int>(sourceNodeSize * 1.1625f);
            sourceVolSlider->setBounds(offsetX, offsetY, sourceVolSlider->getWidth(), sourceVolSlider->getHeight());

            offsetX = sourceNode->getX() - sourceNodeSize / 2;
            offsetY = sourceNode->getY() - sourceNodeSize / 2;
            sourceBackground->setBounds(offsetX, offsetY, sourceNodeSize * 2, sourceNodeSize * 2);
            sourceBackground->setBackgroundProperties(sourceNodeSize * 0.5f, sourceNode->getNodeColour());
        }

        // refresh plane wave of source background
        juce::Point<int> pixPosRef = params.pos2pix(params.referenceX.get(), params.referenceY.get(), getWidth(), getHeight());
        juce::Point<int> pixPosSource = params.pos2pix(params.sourceX.get(), params.sourceY.get(), getWidth(), getHeight());
        float angle = pixPosRef.getAngleToPoint(pixPosSource);
        sourceNode->updatePlaneWave(radiansToDegrees(angle), params.sourceType.getStep() == eSourceType::ePlane);
    }
}

void PlugUI::timerCallback()
{
    PanelBase::timerCallback();

    levelMeterLeft->setValue(params.outputLevelLeft.get(), dontSendNotification);
    levelMeterRight->setValue(params.outputLevelRight.get(), dontSendNotification);

#if 0
    debugText->setText(
        "SourceX = " + String(params.sourceX.get()) +
        "\nSourceY = " + String(params.sourceY.get()) +
        "\nSourceOri = " + String(params.sourceOrientation.get()) +
        "\nsourceVol = " + String(params.sourceVol.get()) +
        "\nSourceLevel = " + String(params.sourceLevel.get()) +
        "\nSourceMute = " + String(params.sourceMute.getUIString()) +
        "\nSourceType = " + String(params.sourceType.getUIString()) +

        "\n\nRefX = " + String(params.referenceX.get()) +
        "\nRefY = " + String(params.referenceY.get()) +
        "\nRefOri = " + String(params.referenceOrientation.get()) +
        "\nAmpliRefDist = " + String(params.amplitudeReferenceDistance.get()) +

        "\n\nInputChannel = " + String(params.inputChannel.getUIString()) +
        "\nOutputLevelLeft = " + String(params.outputLevelLeft.get()) +
        "\nOutputLevelRight = " + String(params.outputLevelRight.get()) +

        "\n\nZoomfactor = " + String(params.zoomFactor.get())
        , dontSendNotification);
#endif
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PlugUI" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p), params(p)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="900" initialHeight="600">
  <METHODS>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
    <METHOD name="mouseWheelMove (const MouseEvent&amp; e, const MouseWheelDetails&amp; wheel)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffedede6">
    <IMAGE pos="23 520 64 64" resource="BinaryData::ssr_logo_png" opacity="1"
           mode="1"/>
  </BACKGROUND>
  <LABEL name="debug" id="3b44d9ef5ee9c1a" memberName="debugText" virtualName=""
         explicitFocusOrder="0" pos="648 8 250 584" edTextCol="ff000000"
         edBkgCol="0" labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="9"/>
  <SLIDER name="level right" id="a744d1a2c21ea6d8" memberName="levelMeterRight"
          virtualName="" explicitFocusOrder="0" pos="856 150 24 300" thumbcol="ff60ff60"
          min="0" max="1" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="level left" id="13e0962dc81dca1" memberName="levelMeterLeft"
          virtualName="" explicitFocusOrder="0" pos="824 150 24 300" thumbcol="ff60ff60"
          min="0" max="1" int="0" style="LinearVertical" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="zoom slider" id="c8e0b018d0c69bbf" memberName="zoomSlider"
          virtualName="ZoomSlider" explicitFocusOrder="0" pos="808 568 80 24"
          min="25" max="200" int="1" style="LinearBar" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="24" skewFactor="1"/>
  <GENERICCOMPONENT name="listener" id="a34b6db6e6ed2361" memberName="listener" virtualName="ListenerComponent"
                    explicitFocusOrder="0" pos="405 255 90 90" class="Component"
                    params="params"/>
  <SLIDER name="source vol slider" id="9fdf624eba9dd99e" memberName="sourceVolSlider"
          virtualName="VolLevelSlider" explicitFocusOrder="0" pos="418 155 64 16"
          min="-96" max="12" int="0" style="LinearBar" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="3"/>
  <GENERICCOMPONENT name="source background" id="65be2d0e1ef0c83a" memberName="sourceBackground"
                    virtualName="" explicitFocusOrder="0" pos="360 10 180 180" class="SourceBackgroundComponent"
                    params=""/>
  <GENERICCOMPONENT name="source menu" id="36a5225af81e8044" memberName="sourceMenu"
                    virtualName="" explicitFocusOrder="0" pos="512 100 250 225" class="DocumentWindow"
                    params="&quot;source menu&quot;, Colours::white, DocumentWindow::closeButton"/>
  <GENERICCOMPONENT name="source node" id="6d034e540e6cd90" memberName="sourceNode"
                    virtualName="" explicitFocusOrder="0" pos="405 54 90 90" class="SourceNodeComponent"
                    params="params, sourceVolSlider, sourceBackground, sourceMenu"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
