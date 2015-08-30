#include "coronaAO.h"

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

MTypeId	CoronaAO::id(0x0011CF44);


// the postConstructor() function is called immediately after the objects
// constructor. It is not safe to call MPxNode member functions from the
// constructor, instead they should be called here.
//
void CoronaAO::postConstructor( )
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


MObject CoronaAO::maxDist;
MObject CoronaAO::maxSamplesCount;
MObject CoronaAO::phongExponent;
MObject CoronaAO::mixExponent;
MObject CoronaAO::invert;
MObject CoronaAO::excludeMode;
MObject CoronaAO::useOffset;
MObject CoronaAO::offset;

MObject CoronaAO::outColor;

CoronaAO::CoronaAO() { }
CoronaAO::~CoronaAO() { }

void* CoronaAO::creator()
{
    return new CoronaAO();
}

MStatus CoronaAO::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; 

	maxDist = nAttr.create("maxDist", "maxDist", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(1000.0);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(maxDist));

	maxSamplesCount = nAttr.create("maxSamplesCount", "maxSamplesCount", MFnNumericData::kInt, 8);
	nAttr.setMin(1);
	nAttr.setSoftMax(32);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(maxSamplesCount));

	phongExponent = nAttr.create("phongExponent", "phongExponent", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(1000.0);
	nAttr.setMax(10000000.0);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(phongExponent));

	mixExponent = nAttr.create("mixExponent", "mixExponent", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.01);
	nAttr.setMax(100.0);
	nAttr.setKeyable(true);
	CHECK_MSTATUS(addAttribute(mixExponent));

	invert = nAttr.create("invert", "invert", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(invert));

	excludeMode = eAttr.create("excludeMode", "excludeMode", 0, &status);
	status = eAttr.addField("List", 0);
	status = eAttr.addField("Exclude Self", 1);
	status = eAttr.addField("Exclude Other", 2);
	CHECK_MSTATUS(addAttribute(excludeMode));

	MObject offsetX = nAttr.create("offsetX", "offsetX", MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute(offsetX));
	MObject offsetY = nAttr.create("offsetY", "offsetY", MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute(offsetY));
	MObject offsetZ = nAttr.create("offsetZ", "offsetZ", MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute(offsetZ));
	MObject offset = nAttr.create("offsetZ", "offsetZ", offsetX, offsetY, offsetZ);

	useOffset = nAttr.create("useOffset", "useOffset", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(useOffset));

	outColor = nAttr.createColor("outColor", "outColor");
	nAttr.setKeyable(false);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	CHECK_MSTATUS(addAttribute(outColor));

	CHECK_MSTATUS(attributeAffects(mixExponent, outColor));

    return( MS::kSuccess );
}

MStatus CoronaAO::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug == outColor)
    {
    }
    else
    {
        return( MS::kUnknownParameter );
    }

    return( MS::kSuccess );
}


