//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Plugin Registration
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "VisualizerNode.h"
#include "VisualizerNodeDrawOverride.h"

#include <maya/MFnPlugin.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MDrawRegistry.h>

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

MStatus initializePlugin( MObject obj )
{
    MStatus   status;
    MFnPlugin plugin( obj, PLUGIN_COMPANY, "3.0", "Any");

    status = plugin.registerNode(
                "visualizerNode",
                VisualizerNode::id,
                &VisualizerNode::creator,
                &VisualizerNode::initialize,
                MPxNode::kLocatorNode,
                &VisualizerNode::drawDbClassification);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        VisualizerNode::drawDbClassification,
        VisualizerNode::drawRegistrantId,
        VisualizerNodeDrawOverride::Creator);
    if (!status) {
        status.perror("registerDrawOverrideCreator");
        return status;
    }

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status;
    MFnPlugin plugin( obj );

    status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
        VisualizerNode::drawDbClassification,
        VisualizerNode::drawRegistrantId);

    if (!status) {
        status.perror("deregisterDrawOverrideCreator");
            return status;
    }

    status = plugin.deregisterNode( VisualizerNode::id );
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }
    return status;
}