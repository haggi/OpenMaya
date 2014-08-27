#include "wood.h"
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

MTypeId Wood::id( 0x0011EF68 );

MObject Wood::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Wood::high;
MObject	Wood::low;
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
void Wood::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Wood::Wood()
{
}

//
// DESCRIPTION:
Wood::~Wood()
{
}

//
// DESCRIPTION:
void *Wood::creator()
{
    return new Wood();
}

//
// DESCRIPTION:
MStatus Wood::initialize()
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
	high = nAttr.createColor("high", "high");
	nAttr.setDefault(0.4,0.4,0.4);
	CHECK_MSTATUS(addAttribute( high ));


	low = nAttr.createColor("low", "low");
	nAttr.setDefault(0.95,0.95,0.95);
	CHECK_MSTATUS(addAttribute( low ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Wood::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
