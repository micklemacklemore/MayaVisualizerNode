//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Plugin Registration
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "VisualizerNode.h"
#include "VisualizerNodeDrawOverride.h"
#include "VisualizerDrawDataNode.h"
#include "VisualizeCmd.h"

#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>


MStatus initializePlugin( MObject obj )
{
    MStatus   status;
    MFnPlugin plugin( obj, "Michael Mason", "0.1", "Any");

    // register visualizer node
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

    // register draw override
    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        VisualizerNode::drawDbClassification,
        VisualizerNode::drawRegistrantId,
        VisualizerNodeDrawOverride::Creator);
    if (!status) {
        status.perror("registerDrawOverrideCreator");
        return status;
    }

    // register custom draw data node
    status = plugin.registerNode( "visualizerDrawDataNode", VisualizerDrawDataNode::id,
                          VisualizerDrawDataNode::creator, VisualizerDrawDataNode::initialize );
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    // register the test command
    status = plugin.registerCommand( "visualizeCmd", VisualizeCmd::creator );
    if (!status) {
        status.perror("register command");
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

    status = plugin.deregisterNode( VisualizerDrawDataNode::id );
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    status = plugin.deregisterCommand("visualizeCmd"); 

    if (!status) {
        status.perror("deregister command");
        return status;
    }

    return status;
}