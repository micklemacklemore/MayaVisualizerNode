#include "VisualizeCmd.h"
#include "VisualizerDrawDataNode.h"
#include "VisualizerNode.h"

#include <maya/MIOStream.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>

MStatus VisualizeCmd::doIt( const MArgList & ) {

    MStatus status; 

    // Create the node
    MFnDagNode mfnNode;
    MObject transformNode = mfnNode.create(VisualizerNode::id, MObject::kNullObj, &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // Get the first child of the transform node, which is the actual shape node
    MObject shapeNode;
    unsigned int numChildren = mfnNode.childCount(&status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    for (unsigned int i = 0; i < numChildren; i++) {
        MObject child = mfnNode.child(i, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        if (child.hasFn(MFn::kPluginLocatorNode) || child.hasFn(MFn::kLocator)) {
            shapeNode = child;
            break;
        }
    }

    if (shapeNode.isNull()) {
        MGlobal::displayError("Visualizer node not found under transform");
        return MStatus::kFailure; 
    }

    mfnNode.setObject(shapeNode); 

    MPlug lineDataPlug = mfnNode.findPlug("lineData", true, &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status); 

    // debug, initially there wont be an MPointArray assigned to it. 
    MGlobal::displayInfo(lineDataPlug.asMObject().apiTypeStr()); 

    MPointArray points; 
    points.append(MPoint(1., 2., 3., 4.));

    MFnPointArrayData fnPointArrayData;
    MObject paData = fnPointArrayData.create(points, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // set MPointArray to plug
    lineDataPlug.setValue(paData);

    // test that we've successfully added data to this node.
    MObject test = lineDataPlug.asMObject(); 
    if (test.hasFn(MFn::Type::kPointArrayData)) {
        fnPointArrayData.setObject(test); 
        MPointArray arr = fnPointArrayData.array(); 

        for (MPoint &p : arr) {
            cout << p.x << " " << p.y << " " << p.z << " " << p.w << endl; 
        }

        MGlobal::displayInfo(test.apiTypeStr()); 
    }


    return status; 
}