#include "blackBody.h"
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

MTypeId Blackbody::id( 0x0011EF6C );

MObject Blackbody::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Blackbody::temperature;
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
void Blackbody::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Blackbody::Blackbody()
{
}

//
// DESCRIPTION:
Blackbody::~Blackbody()
{
}

//
// DESCRIPTION:
void *Blackbody::creator()
{
    return new Blackbody();
}

//
// DESCRIPTION:
MStatus Blackbody::initialize()
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
	temperature = nAttr.create("temperature", "temperature",  MFnNumericData::kFloat, 6500.0);
	CHECK_MSTATUS(addAttribute( temperature ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Blackbody::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
