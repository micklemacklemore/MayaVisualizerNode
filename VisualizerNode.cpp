#include "VisualizerNode.h"

#include <maya/MDistance.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MEvaluationNode.h>

#include <maya/MFnUnitAttribute.h>

#include <cassert>

MObject VisualizerNode::size;
MTypeId VisualizerNode::id( 0x80007 );
MString	VisualizerNode::drawDbClassification("drawdb/geometry/visualizerNode");
MString	VisualizerNode::drawRegistrantId("VisualizerNodePlugin");
MObject VisualizerNode::worldS;

VisualizerNode::VisualizerNode() {}
VisualizerNode::~VisualizerNode() {}

MStatus VisualizerNode::compute( const MPlug& plug/*plug*/, MDataBlock& dataBlock/*data*/ )
{
    MStatus s;
    if (plug == worldS) 
    {
        if (plug.isElement())
        {
            MArrayDataHandle outputArrayHandle = dataBlock.outputArrayValue( worldS );
            outputArrayHandle.setAllClean();
        }

        dataBlock.setClean(plug);

        return MS::kSuccess;
    }
    
    return MS::kUnknownParameter;;
}

bool VisualizerNode::isBounded() const
{
    return true;
}

MBoundingBox VisualizerNode::boundingBox() const
{
    // Get the size
    //
    MObject thisNode = thisMObject();
    MPlug plug( thisNode, size );
    MDistance sizeVal;
    plug.getValue( sizeVal );

    double multiplier = sizeVal.asCentimeters();

    MPoint corner1( -0.17, 0.0, -0.7 );
    MPoint corner2( 0.17, 0.0, 0.3 );

    corner1 = corner1 * multiplier;
    corner2 = corner2 * multiplier;

    return MBoundingBox( corner1, corner2 );
}

// Called before this node is evaluated by Evaluation Manager
MStatus VisualizerNode::preEvaluation(
    const MDGContext& context,
    const MEvaluationNode& evaluationNode)
{
    if (context.isNormal())
    {
        MStatus status;
        if (evaluationNode.dirtyPlugExists(size, &status) && status)
        {
            MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
        }
    }

    return MStatus::kSuccess;
}

void VisualizerNode::getCacheSetup(const MEvaluationNode& evalNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
{
    MPxLocatorNode::getCacheSetup(evalNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
    assert(!disablingInfo.getCacheDisabled());
    cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, true);
}

void* VisualizerNode::creator()
{
    return new VisualizerNode();
}

MStatus VisualizerNode::initialize()
{
    MFnUnitAttribute unitFn;
    MStatus			 stat;

    size = unitFn.create( "size", "sz", MFnUnitAttribute::kDistance );
    unitFn.setDefault( 1.0 );

    stat = addAttribute( size );
    if (!stat) {
        stat.perror("addAttribute");
        return stat;
    }

    worldS = unitFn.create("worldS", "ws", MFnUnitAttribute::kDistance, 1.0);
    unitFn.setWritable(true);
    unitFn.setCached(false);
    unitFn.setArray( true );
    unitFn.setUsesArrayDataBuilder( true );
    unitFn.setWorldSpace( true );

    addAttribute( worldS );
    attributeAffects(size, worldS);

    return MS::kSuccess;
}