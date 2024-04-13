#include "VisualizeCmd.h"
#include "VisualizerDrawDataNode.h"
#include "VisualizerNode.h"

#include <maya/MIOStream.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>

MStatus VisualizeCmd::doIt( const MArgList & ) {
    MFnDependencyNode mfnNode; 
    MStatus status; 

    MObject vsnode = mfnNode.create(VisualizerNode::id, &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status); 

    MObject vsdata = mfnNode.create(VisualizerDrawDataNode::id , &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status); 
    
    bool hasAttr = mfnNode.hasAttribute("lineArray", &status);
    CHECK_MSTATUS_AND_RETURN_IT(status); 

    if (hasAttr) {
        MGlobal::displayInfo("lineArray found");

        MPlug lineArrayPlug = mfnNode.findPlug("lineArray", true, &status); 
        CHECK_MSTATUS_AND_RETURN_IT(status); 

        // debug, initially there wont be an MPointArray assigned to it. 
        MGlobal::displayInfo(lineArrayPlug.asMObject().apiTypeStr()); 

        MPointArray points; 
        points.append(MPoint(1., 2., 3., 4.));

        MFnPointArrayData fnPointArrayData;
        MObject paData = fnPointArrayData.create(points, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        // set MPointArray to plug
        lineArrayPlug.setValue(paData);

        // test that we've successfully added data to this node.
        MObject test = lineArrayPlug.asMObject(); 
        if (test.hasFn(MFn::Type::kPointArrayData)) {
            fnPointArrayData.setObject(test); 
            MPointArray arr = fnPointArrayData.array(); 

            for (MPoint &p : arr) {
                cout << p.x << " " << p.y << " " << p.z << " " << p.w << endl; 
            }

            MGlobal::displayInfo(test.apiTypeStr()); 
        }

    } else {
        MGlobal::displayError("Could not find attribute: lineArray");
        return MStatus::kFailure; 
    }

    return status; 
}