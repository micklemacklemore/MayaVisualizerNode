#include "VisualizeCmd.h"
#include "VisualizerNode.h"

#include <maya/MIOStream.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MViewport2Renderer.h>

MStatus VisualizeCmd::doIt( const MArgList & ) {

    MStatus status; 

    // Create the node
    MFnDagNode mfnNode;
    MObject transformNode = mfnNode.create(VisualizerNode::id, MObject::kNullObj, &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status);
    
    // find the visualizer node because mfnDagNode create returns the 
    // parent transform (for some STUPID reason)
    MObject child = mfnNode.child(0, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    mfnNode.setObject(child); 

    MPlug lineDataPlug = mfnNode.findPlug("lineData", true, &status); 
    CHECK_MSTATUS_AND_RETURN_IT(status); 

    // debug, initially there wont be an MPointArray assigned to it. 
    MGlobal::displayInfo(lineDataPlug.asMObject().apiTypeStr()); 

    MPointArray points; 
    points.append(MPoint(0., 0.));
    points.append(MPoint(1., 0.));

    points.append(MPoint(1., 0.));
    points.append(MPoint(1., 1.));

    points.append(MPoint(1., 1.));
    points.append(MPoint(0., 1.));

    points.append(MPoint(0., 1.));
    points.append(MPoint(0., 0.));

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

        // I'm not sure if I actually need to call this..
        //MHWRender::MRenderer::setGeometryDrawDirty(child); 
    }


    return status; 
}