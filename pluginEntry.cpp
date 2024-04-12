//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Plugin Registration
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "FootPrint.h"
#include "FootPrintDrawOverride.h"

#include <maya/MFnPlugin.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MDrawRegistry.h>

MStatus footPrint::initialize()
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
                "footPrint",
                footPrint::id,
                &footPrint::creator,
                &footPrint::initialize,
                MPxNode::kLocatorNode,
                &footPrint::drawDbClassification);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        footPrint::drawDbClassification,
        footPrint::drawRegistrantId,
        FootPrintDrawOverride::Creator);
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
        footPrint::drawDbClassification,
        footPrint::drawRegistrantId);

    if (!status) {
        status.perror("deregisterDrawOverrideCreator");
            return status;
    }

    status = plugin.deregisterNode( footPrint::id );
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }
    return status;
}