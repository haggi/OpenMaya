#include "CoronaRoundCornersShader.h"

#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MDrawRegistry.h>
#include <maya/MDGModifier.h>

MTypeId	CoronaRoundCornersShader::id(0x0011CF73);


// the postConstructor() function is called immediately after the objects
// constructor. It is not safe to call MPxNode member functions from the
// constructor, instead they should be called here.
//
void CoronaRoundCornersShader::postConstructor( )
{
    // setMPSafe indicates that this shader can be used for multiprocessor
    // rendering. For a shading node to be MP safe, it cannot access any
    // shared global data and should only use attributes in the datablock
    // to get input data and store output data.
    //
	MStatus stat;
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}


MObject CoronaRoundCornersShader::samples;
MObject CoronaRoundCornersShader::radius;
MObject CoronaRoundCornersShader::normalCamera;
MObject CoronaRoundCornersShader::outNormal;

CoronaRoundCornersShader::CoronaRoundCornersShader() { }
CoronaRoundCornersShader::~CoronaRoundCornersShader() { }

void* CoronaRoundCornersShader::creator()
{
    return new CoronaRoundCornersShader();
}

MStatus CoronaRoundCornersShader::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; 

	samples = nAttr.create("samples", "samples", MFnNumericData::kInt, 10);
	nAttr.setMin(1);
	nAttr.setSoftMax(100.0);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(samples));

	radius = nAttr.create("radius", "radius", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(10.0);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(radius));

	MObject nX = nAttr.create("normalCameraX", "nx",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	MObject nY = nAttr.create("normalCameraY", "ny",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	MObject nZ = nAttr.create("normalCameraZ", "nz",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	normalCamera = nAttr.create("normalCamera", "n", nX, nY, nZ, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(addAttribute(normalCamera));

	MObject onX = nAttr.create("outNormalX", "onx",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	MObject onY = nAttr.create("outNormalY", "ony",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	MObject onZ = nAttr.create("outNormalZ", "onz",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	outNormal = nAttr.create("outNormal", "on", onX, onY, onZ, &status);
	nAttr.setKeyable(false);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	CHECK_MSTATUS(addAttribute(outNormal));

	CHECK_MSTATUS(attributeAffects(radius, outNormal));
	CHECK_MSTATUS(attributeAffects(normalCamera, outNormal));

    return( MS::kSuccess );
}

MStatus CoronaRoundCornersShader::compute( const MPlug& plug, MDataBlock& block )
{
	if (plug == outNormal)
	{
    }
    else
    {
        return( MS::kUnknownParameter );
    }

    return( MS::kSuccess );
}


