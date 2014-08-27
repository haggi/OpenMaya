#include "windy.h"
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

MTypeId Windy::id( 0x0011EF66 );

MObject Windy::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Windy::high;
MObject	Windy::low;
MObject	Windy::strengthOctaves;
MObject	Windy::strengthOmega;
MObject	Windy::heightOctaves;
MObject	Windy::heightOmega;
MObject	Windy::symmetric;
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
void Windy::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Windy::Windy()
{
}

//
// DESCRIPTION:
Windy::~Windy()
{
}

//
// DESCRIPTION:
void *Windy::creator()
{
    return new Windy();
}

//
// DESCRIPTION:
MStatus Windy::initialize()
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


	strengthOctaves = nAttr.create("strengthOctaves", "strengthOctaves",  MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( strengthOctaves ));


	strengthOmega = nAttr.create("strengthOmega", "strengthOmega",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( strengthOmega ));


	heightOctaves = nAttr.create("heightOctaves", "heightOctaves",  MFnNumericData::kInt, 6);
	CHECK_MSTATUS(addAttribute( heightOctaves ));


	heightOmega = nAttr.create("heightOmega", "heightOmega",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( heightOmega ));


	symmetric = nAttr.create("symmetric", "symmetric",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( symmetric ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Windy::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
