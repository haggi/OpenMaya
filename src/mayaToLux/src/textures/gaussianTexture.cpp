#include "gaussianTexture.h"
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

MTypeId gaussian::id( 0x0011EF4D );

MObject gaussian::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject gaussian::wavelength;
MObject gaussian::energy;
MObject gaussian::width;
MObject gaussian::luxOutColor;
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
void gaussian::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
gaussian::gaussian()
{
}

//
// DESCRIPTION:
gaussian::~gaussian()
{
}

//
// DESCRIPTION:
void *gaussian::creator()
{
    return new gaussian();
}

//
// DESCRIPTION:
MStatus gaussian::initialize()
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
	wavelength = nAttr.create("wavelength", "wavelength",  MFnNumericData::kFloat, 550.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( wavelength ));

	energy = nAttr.create("energy", "energy",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( energy ));

	width = nAttr.create("width", "width",  MFnNumericData::kFloat, 50.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( width ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( wavelength, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( wavelength, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus gaussian::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
