#pragma once

#include "JuceHeader.h"
#include "Param.h"

enum class eOnOffState : int {
    eOff = 0,
    eOn = 1,
    nSteps = 2
};

enum class eSourceType : int {
    ePoint = 0,
    ePlane = 1,
    nSteps = 2
};

enum class eInputChannel : int {
    eLeftChannel = 0,
    eRightChannel = 1,
    nSteps = 2
};

class SynthParams {
public:
    SynthParams();
    ~SynthParams();

    //==============================================================================

    // source parameter
    Param sourceX;
    Param sourceY;
    Param sourceOrientation;
    Param sourceVol;
    Param sourceLevel;
    ParamStepped<eOnOffState> sourceMute;
    ParamStepped<eSourceType> sourceType;
    ParamStepped<eOnOffState> sourcePositionLock;

    // reference listener parameter
    Param referenceX;
    Param referenceY;
    Param referenceOrientation;
    Param amplitudeReferenceDistance;

    // routing and output level parameter
    ParamStepped<eInputChannel> inputChannel;
    Param outputLevelLeft;
    Param outputLevelRight;

    // GUI scaling and dragging parameter
    Param zoomFactor;
    const int pixelPerMeter;
    Param sceneOffsetX;
    Param sceneOffsetY;

    //==============================================================================

    // original SSR colour codes
    static const Colour sourceColourBrown;
    static const Colour sourceColourBlue;
    static const Colour sourceColourGreen;
    static const Colour sourceColourViolet;
    static const Colour sourceColourRed;
    static const Colour sceneBackgroundColour;
    static const Colour sourceLevelColour;

    //==============================================================================

    /**
        Converts a meter position into a pixel coordinate of a scene with given width and height.
        Reference point is the center of that scene, where 1m is equivalent to 100px.
        Scaling can be used by setting the zoomFactor variable of SynthParams.
    
        @param meterCenterX centerX position in meter of an object in the scene GUI
        @param meterCenterY centerY position in meter of an object in the scene GUI
        @param sceneWidth width of the scene GUI in pixel
        @param sceneHeight height of the scene GUI in pixel
        @return coordinate point in pixel with inverse y
     */
    juce::Point<int> pos2pix(float meterCenterX, float meterCenterY, int sceneWidth, int sceneHeight);

    /**
        Converts a pixel coordinate of a scene with given width and height into a meter position.
        Reference point is the center of that scene, where 1m is equivalent to 100px.
        Scaling can be used by setting the zoomFactor variable of SynthParams.

        @param pixCenterX centerX pixel of an object in the scene GUI
        @param pixCenterY centerY pixel of an object in the scene GUI
        @param sceneWidth width of the scene GUI in pixel
        @param sceneHeight height of the scene GUI in pixel
        @return position point in meter with inverse y
    */
    juce::Point<float> pix2pos(int pixCenterX, int pixCenterY, int sceneWidth, int sceneHeight);

    /**
        Round float value to not more than 2 numbers after comma.
    */
    float roundNearest(float val);

    //==============================================================================

    std::vector<Param*> serializeParams; //!< vector of params to be serialized
    String patchName = "";
    bool patchNameDirty = false;
    const String appName = ProjectInfo::projectName;
    const String appVersion = ProjectInfo::versionString; //!< application version of the patch, to be written into the xml

    /**
    * Store host state by creating XML file to serialize parameters by using writeXMLPatchTree().
    @param destData host data
    */
    void writeXMLPatchHost(MemoryBlock& destData);

    /**
    * Create XML file to serialize parameters by using writeXMLPatchTree().
    */
    void writeXMLPatchStandalone();

    /**
    * Restore host state by converting binary data into a XML file and set serialized parameters by using fillValues().
    @param data binary data to return to XML
    @param sizeInBytes data size
    */
    void readXMLPatchHost(const void * data, int sizeInBytes);

    /**
    * Read XML file to set serialized parameters by using fillValues().
    */
    void readXMLPatchStandalone();

    //==============================================================================

private:

    /**
    * Adds an element to the XML tree.
    @param patch XML pstch to work on
    @param name parameter name
    @param value  value of parameter to be added
    */
    void addElement(XmlElement* patch, String name, float value);

    /**
    * Write the XML patch tree for parameters to be serialized.
    @param patch XML patch to work on
    */
    void writeXMLPatchTree(XmlElement * patch);

    /**
    * Set the parameters if they exist in the XML patch.
    @param patch XML patch to work on
    @param paramName name to check whether parameter exist in XML patch
    @param param parameter to set
    */
    void fillValueIfExists(XmlElement * patch, String paramName, Param& param);

    /**
    * Iterate over parameters and set the values in the xml by using fillValueIfExists().
    @param patch XML patch to work on
    */
    void fillValues(XmlElement * patch);
};
