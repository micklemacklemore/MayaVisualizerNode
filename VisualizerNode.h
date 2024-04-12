#pragma once 

#include <maya/MPxLocatorNode.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Node implementation with standard viewport draw
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class VisualizerNode : public MPxLocatorNode
{
public:
    VisualizerNode();
    ~VisualizerNode() override;

    MStatus   		compute( const MPlug& plug, MDataBlock& data ) override;

    bool            isBounded() const override;
    MBoundingBox    boundingBox() const override;

    MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;
    void    getCacheSetup(const MEvaluationNode& evalNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const override;

    static  void *          creator();
    static  MStatus         initialize();

    static  MObject         size;         // The size of the foot

public:
    static	MTypeId		id;
    static	MString		drawDbClassification;
    static	MString		drawRegistrantId;

    static MObject      worldS;
};