#include "mediumclearTexture.h"
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

MTypeId mediumclear::id( 0x0011EF50 );

MObject mediumclear::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject mediumclear::absorption;
MObject mediumclear::fresnel;
MObject mediumclear::luxOutColor;
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
void mediumclear::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
mediumclear::mediumclear()
{
}

//
// DESCRIPTION:
mediumclear::~mediumclear()
{
}

//
// DESCRIPTION:
void *mediumclear::creator()
{
    return new mediumclear();
}

//
// DESCRIPTION:
MStatus mediumclear::initialize()
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
	absorption = nAttr.createColor("absorption", "absorption");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.0,0.0,0.0);
	CHECK_MSTATUS(addAttribute( absorption ));

	fresnel = nAttr.create("fresnel", "fresnel",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( fresnel ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( absorption, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( absorption, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus mediumclear::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
