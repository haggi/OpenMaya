#include "fresnelcolorTexture.h"
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

MTypeId fresnelcolor::id( 0x0011EF4B );

MObject fresnelcolor::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject fresnelcolor::Kr;
MObject fresnelcolor::luxOutFresnel;
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
void fresnelcolor::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
fresnelcolor::fresnelcolor()
{
}

//
// DESCRIPTION:
fresnelcolor::~fresnelcolor()
{
}

//
// DESCRIPTION:
void *fresnelcolor::creator()
{
    return new fresnelcolor();
}

//
// DESCRIPTION:
MStatus fresnelcolor::initialize()
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
	Kr = nAttr.createColor("Kr", "Kr");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.5,0.5,0.5);
	CHECK_MSTATUS(addAttribute( Kr ));

	luxOutFresnel = nAttr.create("luxOutFresnel", "luxOutFresnel",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFresnel ));

	CHECK_MSTATUS ( attributeAffects( Kr, luxOutFresnel));
	CHECK_MSTATUS ( attributeAffects( Kr, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus fresnelcolor::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
