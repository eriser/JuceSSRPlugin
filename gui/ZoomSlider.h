/*
  ==============================================================================

    ZoomSlider.h
    Created: 10 Aug 2016 11:33:20pm
    Author:  Nutty

  ==============================================================================
*/

#ifndef ZOOMSLIDER_H_INCLUDED
#define ZOOMSLIDER_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
/**
 * Slider class which uses a custom drag mechanism. Designed according to SSR's zoom slider.
 */
class ZoomSlider    : public Slider
{
public:
    ZoomSlider(const juce::String &componentName)
        : Slider(componentName)
    {
    }

    ~ZoomSlider()
    {
    }

    //==============================================================================
    
    void paint (Graphics& g)
    {
        g.fillAll(isMouseOver ? Colours::white.darker(0.025f) : Colours::white);
        g.setColour(Colours::grey.brighter());
        g.drawRect(0.0f, 0.0f, static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.5f);

        AffineTransform trans(AffineTransform::scale(static_cast<float>(getWidth()), static_cast<float>(getHeight())));
        Path arrow;
        arrow.startNewSubPath(0.75f, 0.425f);
        arrow.lineTo(0.805f, 0.25f);
        arrow.lineTo(0.86f, 0.425f);
        arrow.startNewSubPath(0.75f, 0.575f);
        arrow.lineTo(0.805f, 0.75f);
        arrow.lineTo(0.86f, 0.575f);

        g.setColour(Colours::black);
        g.strokePath(arrow, PathStrokeType(1.5f), trans);
    }

    //==============================================================================

    /// redraw to indicate mouse is over this component
    void mouseEnter(const MouseEvent& e)
    {
        ignoreUnused(e);
        isMouseOver = true;
        repaint();
    }

    void mouseExit(const MouseEvent& e)
    {
        ignoreUnused(e);
        isMouseOver = false;
        repaint();
    }

    /// vertical drag of this slider component
    void mouseDown(const MouseEvent& e)
    {
        ignoreUnused(e);
        dragStartValue = getValue();
    }

    void mouseDrag(const MouseEvent& e)
    {
        double delta = e.getDistanceFromDragStartY() / 2.0;
        setValue(dragStartValue - delta);
    }

    //==============================================================================

private:
    double dragStartValue = 0.0;
    bool isMouseOver = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZoomSlider)
};


#endif  // ZOOMSLIDER_H_INCLUDED
