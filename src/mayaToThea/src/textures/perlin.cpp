#include "perlin.h"
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

MTypeId Perlin::id( 0x0011EF64 );

MObject Perlin::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Perlin::high;
MObject	Perlin::low;
MObject	Perlin::octaves;
MObject	Perlin::omega;
MObject	Perlin::turbulence;
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
void Perlin::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Perlin::Perlin()
{
}

//
// DESCRIPTION:
Perlin::~Perlin()
{
}

//
// DESCRIPTION:
void *Perlin::creator()
{
    return new Perlin();
}

//
// DESCRIPTION:
MStatus Perlin::initialize()
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


	octaves = nAttr.create("octaves", "octaves",  MFnNumericData::kInt, 8);
	CHECK_MSTATUS(addAttribute( octaves ));


	omega = nAttr.create("omega", "omega",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( omega ));


	turbulence = nAttr.create("turbulence", "turbulence",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( turbulence ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Perlin::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
