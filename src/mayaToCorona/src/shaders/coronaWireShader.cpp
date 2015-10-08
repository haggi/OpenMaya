#include "coronaWireShader.h"

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

MTypeId	CoronaWire::id(0x0011CF45);

MObject CoronaWire::useWorldSpace;
MObject CoronaWire::showVertices;
MObject CoronaWire::showEdges;
MObject CoronaWire::allEdges;
MObject CoronaWire::edgeWidth;
MObject CoronaWire::vertexWidth;
MObject CoronaWire::outColor;

void CoronaWire::postConstructor( )
{
	MStatus stat;
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}

CoronaWire::CoronaWire() { }
CoronaWire::~CoronaWire() { }


void* CoronaWire::creator()
{
    return new CoronaWire();
}

MStatus CoronaWire::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;
    MStatus status; 

	useWorldSpace = nAttr.create("useWorldSpace", "useWorldSpace", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(useWorldSpace));

	showVertices = nAttr.create("showVertices", "showVertices", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(showVertices));

	showEdges = nAttr.create("showEdges", "showEdges", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(showEdges));

	allEdges = nAttr.create("allEdges", "allEdges", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute(allEdges));

	edgeWidth = nAttr.create("edgeWidth", "edgeWidth", MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute(edgeWidth));

	vertexWidth = nAttr.create("vertexWidth", "vertexWidth", MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute(vertexWidth));

	outColor = nAttr.createColor("outColor", "outColor");
	nAttr.setKeyable(false);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	CHECK_MSTATUS(addAttribute(outColor));

	CHECK_MSTATUS(attributeAffects(useWorldSpace, outColor));

	return(MS::kSuccess);
}

MStatus CoronaWire::compute( const MPlug& plug, MDataBlock& block )
{
    return( MS::kSuccess );
}


