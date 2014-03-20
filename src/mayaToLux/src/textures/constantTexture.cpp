#include "constantTexture.h"
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

MTypeId constant::id( 0x0011EF48 );

MObject constant::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject constant::color;
MObject constant::luxOutFloat;
MObject constant::luxOutColor;
MObject constant::luxOutFresnel;
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
void constant::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
constant::constant()
{
}

//
// DESCRIPTION:
constant::~constant()
{
}

//
// DESCRIPTION:
void *constant::creator()
{
    return new constant();
}

//
// DESCRIPTION:
MStatus constant::initialize()
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
	color = nAttr.createColor("color", "color");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1.0,1.0,1.0);
	CHECK_MSTATUS(addAttribute( color ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	luxOutFresnel = nAttr.create("luxOutFresnel", "luxOutFresnel",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFresnel ));

	CHECK_MSTATUS ( attributeAffects( color, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( color, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( color, luxOutFresnel));
	CHECK_MSTATUS ( attributeAffects( color, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus constant::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
