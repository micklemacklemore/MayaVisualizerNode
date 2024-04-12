#include "VisualizerNodeDrawOverride.h"

#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>

// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MEventMessage.h>
#include <maya/MFnDependencyNode.h>

#include <assert.h>


// By setting isAlwaysDirty to false in MPxDrawOverride constructor, the
// draw override will be updated (via prepareForDraw()) only when the node
// is marked dirty via DG evaluation or dirty propagation. Additional
// callback is also added to explicitly mark the node as being dirty (via
// MRenderer::setGeometryDrawDirty()) for certain circumstances. Note that
// the draw callback in MPxDrawOverride constructor is set to NULL in order
// to achieve better performance.
VisualizerNodeDrawOverride::VisualizerNodeDrawOverride(const MObject& obj)
: MHWRender::MPxDrawOverride(obj, NULL, false)
{
    fModelEditorChangedCbId = MEventMessage::addEventCallback(
        "modelEditorChanged", OnModelEditorChanged, this);

    MStatus status;
    MFnDependencyNode node(obj, &status);
    fVisualizerNode = status ? dynamic_cast<VisualizerNode*>(node.userNode()) : NULL;
}

VisualizerNodeDrawOverride::~VisualizerNodeDrawOverride()
{
    fVisualizerNode = NULL;

    if (fModelEditorChangedCbId != 0)
    {
        MMessage::removeCallback(fModelEditorChangedCbId);
        fModelEditorChangedCbId = 0;
    }
}

void VisualizerNodeDrawOverride::OnModelEditorChanged(void *clientData)
{
    // Mark the node as being dirty so that it can update on display appearance
    // switch among wireframe and shaded.
    VisualizerNodeDrawOverride *ovr = static_cast<VisualizerNodeDrawOverride*>(clientData);
    if (ovr && ovr->fVisualizerNode)
    {
        MHWRender::MRenderer::setGeometryDrawDirty(ovr->fVisualizerNode->thisMObject());
    }
}

MHWRender::DrawAPI VisualizerNodeDrawOverride::supportedDrawAPIs() const
{
    // this plugin supports both GL and DX
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

float VisualizerNodeDrawOverride::getMultiplier(const MDagPath& objPath) const
{
    // Retrieve value of the size attribute from the node
    MStatus status;
    MObject visNode = objPath.node(&status);
    if (status)
    {
        MPlug plug(visNode, VisualizerNode::size);
        if (!plug.isNull())
        {
            MDistance sizeVal;
            if (plug.getValue(sizeVal))
            {
                return (float)sizeVal.asCentimeters();
            }
        }
    }

    return 1.0f;
}

bool VisualizerNodeDrawOverride::isBounded(const MDagPath& /*objPath*/,
                                      const MDagPath& /*cameraPath*/) const
{
    return true;
}

MBoundingBox VisualizerNodeDrawOverride::boundingBox(
    const MDagPath& objPath,
    const MDagPath& cameraPath) const
{
    MPoint corner1( -0.17, 0.0, -0.7 );
    MPoint corner2( 0.17, 0.0, 0.3 );

    float multiplier = getMultiplier(objPath);
    corner1 = corner1 * multiplier;
    corner2 = corner2 * multiplier;

    return MBoundingBox( corner1, corner2 );
}

// Called by Maya each time the object needs to be drawn.
MUserData* VisualizerNodeDrawOverride::prepareForDraw(
    const MDagPath& objPath,
    const MDagPath& cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData* oldData)
{
    // Any data needed from the Maya dependency graph must be retrieved and cached in this stage.
    // There is one cache data for each drawable instance, if it is not desirable to allow Maya to handle data
    // caching, simply return null in this method and ignore user data parameter in draw callback method.
    // e.g. in this sample, we compute and cache the data for usage later when we create the 
    // MUIDrawManager to draw footprint in method addUIDrawables().
    FootPrintData* data = dynamic_cast<FootPrintData*>(oldData);
    if (!data)
    {
        data = new FootPrintData();
    }
    
    float fMultiplier = getMultiplier(objPath);

    data->fLineList.clear();

    for (int i = 0; i <= soleCount - 2; i++)
    {
        data->fLineList.append(sole[i][0] * fMultiplier, sole[i][1] * fMultiplier, sole[i][2] * fMultiplier);
        data->fLineList.append(sole[i+1][0] * fMultiplier, sole[i+1][1] * fMultiplier, sole[i+1][2] * fMultiplier);
    }

    for (int i = 0; i <= heelCount - 2; i++)
    {
        data->fLineList.append(heel[i][0] * fMultiplier, heel[i][1] * fMultiplier, heel[i][2] * fMultiplier);
        data->fLineList.append(heel[i+1][0] * fMultiplier, heel[i+1][1] * fMultiplier, heel[i+1][2] * fMultiplier);
    }

    data->fTriangleList.clear();

    for (int i = 1; i <= soleCount - 2; i++)
    {
        data->fTriangleList.append(sole[0][0] * fMultiplier, sole[0][1] * fMultiplier, sole[0][2] * fMultiplier);
        data->fTriangleList.append(sole[i][0] * fMultiplier, sole[i][1] * fMultiplier, sole[i][2] * fMultiplier);
        data->fTriangleList.append(sole[i+1][0] * fMultiplier, sole[i+1][1] * fMultiplier, sole[i+1][2] * fMultiplier);
    }

    for (int i = 1; i <= heelCount - 2; i++)
    {
        data->fTriangleList.append(heel[0][0] * fMultiplier, heel[0][1] * fMultiplier, heel[0][2] * fMultiplier);
        data->fTriangleList.append(heel[i][0] * fMultiplier, heel[i][1] * fMultiplier, heel[i][2] * fMultiplier);
        data->fTriangleList.append(heel[i+1][0] * fMultiplier, heel[i+1][1] * fMultiplier, heel[i+1][2] * fMultiplier);
    }

    // get correct color and depth priority based on the state of object, e.g. active or dormant
    //data->fColor = MHWRender::MGeometryUtilities::wireframeColor(objPath);
    switch (MHWRender::MGeometryUtilities::displayStatus(objPath))
    {
    case MHWRender::kLead:
    case MHWRender::kActive:
    case MHWRender::kHilite:
    case MHWRender::kActiveComponent:
        data->fDepthPriority = MHWRender::MRenderItem::sActiveWireDepthPriority;
        break;
    default:
        data->fDepthPriority = MHWRender::MRenderItem::sDormantFilledDepthPriority;
        break;
    }

    return data;
}

// addUIDrawables() provides access to the MUIDrawManager, which can be used
// to queue up operations for drawing simple UI elements such as lines, circles and
// text. To enable addUIDrawables(), override hasUIDrawables() and make it return true.
void VisualizerNodeDrawOverride::addUIDrawables(
        const MDagPath& objPath,
        MHWRender::MUIDrawManager& drawManager,
        const MHWRender::MFrameContext& frameContext,
        const MUserData* data)
{
    // Get data cached by prepareForDraw() for each drawable instance, then MUIDrawManager 
    // can draw simple UI by these data.
    FootPrintData* pLocatorData = (FootPrintData*)data;
    if (!pLocatorData)
    {
        return;
    }

    drawManager.beginDrawable();

    // Draw the foot print solid/wireframe
    drawManager.setColor( MColor{1.f, 0.f, 0.f, 1.f} );
    drawManager.setDepthPriority( pLocatorData->fDepthPriority );

    if (frameContext.getDisplayStyle() & MHWRender::MFrameContext::kGouraudShaded) {
        drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->fTriangleList);
    }

    drawManager.setColor( MColor{0.f, 1.f, 0.f, 1.f} );

    drawManager.mesh(MHWRender::MUIDrawManager::kLines, pLocatorData->fLineList);

    // Draw a text "Foot"
    MPoint pos( 0.0, 0.0, 0.0 ); // Position of the text
    MColor textColor( 0.1f, 0.8f, 0.8f, 1.0f ); // Text color

    drawManager.setColor( textColor );
    drawManager.setFontSize( MHWRender::MUIDrawManager::kSmallFontSize );
    drawManager.text( pos,  MString("Incredible Stuff!! Truly!!!! Wow."), MHWRender::MUIDrawManager::kCenter );
    drawManager.endDrawable();
}


