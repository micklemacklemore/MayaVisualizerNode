#pragma once

#include "FootPrint.h"

#include <maya/MGlobal.h>
#include <maya/MUserData.h>
#include <maya/MPointArray.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MEventMessage.h>

// Foot Data
//
static float sole[][3] = { {  0.00f, 0.0f, -0.70f },
                           {  0.04f, 0.0f, -0.69f },
                           {  0.09f, 0.0f, -0.65f },
                           {  0.13f, 0.0f, -0.61f },
                           {  0.16f, 0.0f, -0.54f },
                           {  0.17f, 0.0f, -0.46f },
                           {  0.17f, 0.0f, -0.35f },
                           {  0.16f, 0.0f, -0.25f },
                           {  0.15f, 0.0f, -0.14f },
                           {  0.13f, 0.0f,  0.00f },
                           {  0.00f, 0.0f,  0.00f },
                           { -0.13f, 0.0f,  0.00f },
                           { -0.15f, 0.0f, -0.14f },
                           { -0.16f, 0.0f, -0.25f },
                           { -0.17f, 0.0f, -0.35f },
                           { -0.17f, 0.0f, -0.46f },
                           { -0.16f, 0.0f, -0.54f },
                           { -0.13f, 0.0f, -0.61f },
                           { -0.09f, 0.0f, -0.65f },
                           { -0.04f, 0.0f, -0.69f },
                           { -0.00f, 0.0f, -0.70f } };
static float heel[][3] = { {  0.00f, 0.0f,  0.06f },
                           {  0.13f, 0.0f,  0.06f },
                           {  0.14f, 0.0f,  0.15f },
                           {  0.14f, 0.0f,  0.21f },
                           {  0.13f, 0.0f,  0.25f },
                           {  0.11f, 0.0f,  0.28f },
                           {  0.09f, 0.0f,  0.29f },
                           {  0.04f, 0.0f,  0.30f },
                           {  0.00f, 0.0f,  0.30f },
                           { -0.04f, 0.0f,  0.30f },
                           { -0.09f, 0.0f,  0.29f },
                           { -0.11f, 0.0f,  0.28f },
                           { -0.13f, 0.0f,  0.25f },
                           { -0.14f, 0.0f,  0.21f },
                           { -0.14f, 0.0f,  0.15f },
                           { -0.13f, 0.0f,  0.06f },
                           { -0.00f, 0.0f,  0.06f } };
static int soleCount = 21;
static int heelCount = 17;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Viewport 2.0 override implementation
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class FootPrintData : public MUserData
{
public:
    MColor       fColor{1.f, 0.f, 0.f, 1.f};
    unsigned int fDepthPriority;
    MPointArray  fLineList;
    MPointArray  fTriangleList;
};

class FootPrintDrawOverride : public MHWRender::MPxDrawOverride
{
public:
    static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
    {
        return new FootPrintDrawOverride(obj);
    }

    ~FootPrintDrawOverride() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    bool isBounded(
        const MDagPath& objPath,
        const MDagPath& cameraPath) const override;

    MBoundingBox boundingBox(
        const MDagPath& objPath,
        const MDagPath& cameraPath) const override;

    MUserData* prepareForDraw(
        const MDagPath& objPath,
        const MDagPath& cameraPath,
        const MHWRender::MFrameContext& frameContext,
        MUserData* oldData) override;

    bool hasUIDrawables() const override { return true; }

    void addUIDrawables(
        const MDagPath& objPath,
        MHWRender::MUIDrawManager& drawManager,
        const MHWRender::MFrameContext& frameContext,
        const MUserData* data) override;

    bool traceCallSequence() const override
    {
        // Return true if internal tracing is desired.
        return false;
    }
    void handleTraceMessage( const MString &message ) const override
    {
        MGlobal::displayInfo("footPrintDrawOverride: " + message);

        // Some simple custom message formatting.
        fputs("footPrintDrawOverride: ",stderr);
        fputs(message.asChar(),stderr);
        fputs("\n",stderr);
    }


private:
    FootPrintDrawOverride(const MObject& obj);
    float getMultiplier(const MDagPath& objPath) const;

    static void OnModelEditorChanged(void *clientData);

    footPrint*  fFootPrint;
    MCallbackId fModelEditorChangedCbId;
};
