#include "footPrint.h"

#include <maya/MDistance.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MEvaluationNode.h>

#include <cassert>

MObject footPrint::size;
MTypeId footPrint::id( 0x80007 );
MString	footPrint::drawDbClassification("drawdb/geometry/footPrint");
MString	footPrint::drawRegistrantId("FootprintNodePlugin");
MObject footPrint::worldS;

footPrint::footPrint() {}
footPrint::~footPrint() {}

MStatus footPrint::compute( const MPlug& plug/*plug*/, MDataBlock& dataBlock/*data*/ )
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

bool footPrint::isBounded() const
{
    return true;
}

MBoundingBox footPrint::boundingBox() const
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
MStatus footPrint::preEvaluation(
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

void footPrint::getCacheSetup(const MEvaluationNode& evalNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
{
    MPxLocatorNode::getCacheSetup(evalNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
    assert(!disablingInfo.getCacheDisabled());
    cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, true);
}

void* footPrint::creator()
{
    return new footPrint();
}