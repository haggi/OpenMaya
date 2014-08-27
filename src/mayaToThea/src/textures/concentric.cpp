#include "concentric.h"
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

MTypeId Concentric::id( 0x0011EF60 );

MObject Concentric::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
//---------------------------- automatically created static attributes end ------------------------------------
	

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
void Concentric::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Concentric::Concentric()
{
}

//
// DESCRIPTION:
Concentric::~Concentric()
{
}

//
// DESCRIPTION:
void *Concentric::creator()
{
    return new Concentric();
}

//
// DESCRIPTION:
MStatus Concentric::initialize()
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
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Concentric::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
