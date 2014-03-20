#include "uvTexture.h"
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

MTypeId uv::id( 0x0011EF53 );

MObject uv::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject uv::luxOutColor;
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
void uv::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
uv::uv()
{
}

//
// DESCRIPTION:
uv::~uv()
{
}

//
// DESCRIPTION:
void *uv::creator()
{
    return new uv();
}

//
// DESCRIPTION:
MStatus uv::initialize()
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
	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus uv::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
