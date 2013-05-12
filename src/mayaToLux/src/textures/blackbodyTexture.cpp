#include "blackbodyTexture.h"
#include <math.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>

MTypeId blackbody::id( 0x0011CF61 );

MObject blackbody::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject blackbody::temperature;
MObject blackbody::luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------
	

#define MAKE_INPUT(attr)                                \
 CHECK_MSTATUS ( attr.setKeyable(true) );               \
 CHECK_MSTATUS ( attr.setStorable(true) );              \
 CHECK_MSTATUS ( attr.setReadable(true) );              \
 CHECK_MSTATUS ( attr.setWritable(true) );

#define MAKE_OUTPUT(attr)                               \
  CHECK_MSTATUS ( attr.setKeyable(false) );             \
  CHECK_MSTATUS ( attr.setStorable(false) );            \
  CHECK_MSTATUS ( attr.setReadable(true) );             \
  CHECK_MSTATUS ( attr.setWritable(false) );

//
// DESCRIPTION:
void blackbody::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
blackbody::blackbody()
{
}

//
// DESCRIPTION:
blackbody::~blackbody()
{
}

//
// DESCRIPTION:
void *blackbody::creator()
{
    return new blackbody();
}

//
// DESCRIPTION:
MStatus blackbody::initialize()
{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus status;

	outColor = nAttr.createColor("outColor", "outColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( outColor ));

//---------------------------- automatically created attributes start ------------------------------------
	temperature = nAttr.createColor("temperature", "temperature");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(6500.0,6500.0,6500.0);
	CHECK_MSTATUS(addAttribute( temperature ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( temperature, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( temperature, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus blackbody::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
