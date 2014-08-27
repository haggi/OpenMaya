#include "gradient.h"
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

MTypeId Gradient::id( 0x0011EF63 );

MObject Gradient::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Gradient::high;
MObject	Gradient::low;
MObject	Gradient::direction;
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
void Gradient::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Gradient::Gradient()
{
}

//
// DESCRIPTION:
Gradient::~Gradient()
{
}

//
// DESCRIPTION:
void *Gradient::creator()
{
    return new Gradient();
}

//
// DESCRIPTION:
MStatus Gradient::initialize()
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
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( high ));


	low = nAttr.createColor("low", "low");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( low ));


	MObject directionX = nAttr.create("directionX", "directionx", MFnNumericData::kDouble, 0.0);
	MObject directionY = nAttr.create("directionY", "directiony", MFnNumericData::kDouble, 0.0);
	MObject directionZ = nAttr.create("directionZ", "directionz", MFnNumericData::kDouble, 0.0);
	direction = nAttr.create("direction", "direction", directionX, directionY, directionZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1,0,0);
	CHECK_MSTATUS(addAttribute( direction ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Gradient::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
