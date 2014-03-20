#include "tabulateddataTexture.h"
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

MTypeId tabulateddata::id( 0x0011EF52 );

MObject tabulateddata::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject tabulateddata::filename;
MObject tabulateddata::luxOutColor;
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
void tabulateddata::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
tabulateddata::tabulateddata()
{
}

//
// DESCRIPTION:
tabulateddata::~tabulateddata()
{
}

//
// DESCRIPTION:
void *tabulateddata::creator()
{
    return new tabulateddata();
}

//
// DESCRIPTION:
MStatus tabulateddata::initialize()
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
	filename = tAttr.create("filename", "filename",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( filename ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( filename, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( filename, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus tabulateddata::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
