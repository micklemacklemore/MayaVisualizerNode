#include <maya/MPxNode.h>

// The circle class defines the attributes
// and methods necessary for the circleNode plugin
//
class VisualizerDrawDataNode : public MPxNode
{
public:
                        VisualizerDrawDataNode();
                    ~VisualizerDrawDataNode() override; 
    MStatus     compute( const MPlug& plug, MDataBlock& data ) override;
    static  void*       creator();
    static  MStatus     initialize();
 
public:
    //static  MObject     input;      // The input value.
    //static  MObject     sOutput;    // The sinusoidal output value.
    //static  MObject     cOutput;    // The cosinusoidal output value.
    //static  MObject     frames;     // Number of frames for one circle.
    //static  MObject     scale;      // Size of circle.
    //static  MObject     drawData;     // array attrs data
    static  MObject     lineArray; 
    static  MObject     lineColorArray; 
    static  MObject     triArray; 
    static  MObject     triColArray; 
    static  MTypeId     id;
};