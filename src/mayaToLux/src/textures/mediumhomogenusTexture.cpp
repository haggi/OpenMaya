#include "mediumhomogenusTexture.h"
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

MTypeId mediumhomogenus::id( 0x0011EF51 );

MObject mediumhomogenus::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject mediumhomogenus::g2;
MObject mediumhomogenus::g1;
MObject mediumhomogenus::sigma_s;
MObject mediumhomogenus::g;
MObject mediumhomogenus::sigma_a;
MObject mediumhomogenus::luxOutColor;
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
void mediumhomogenus::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
mediumhomogenus::mediumhomogenus()
{
}

//
// DESCRIPTION:
mediumhomogenus::~mediumhomogenus()
{
}

//
// DESCRIPTION:
void *mediumhomogenus::creator()
{
    return new mediumhomogenus();
}

//
// DESCRIPTION:
MStatus mediumhomogenus::initialize()
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
	g2 = nAttr.create("g2", "g2",  MFnNumericData::kFloat, 0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( g2 ));

	g1 = nAttr.create("g1", "g1",  MFnNumericData::kFloat, 0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( g1 ));

	sigma_s = nAttr.createColor("sigma_s", "sigma_s");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( sigma_s ));

	g = nAttr.create("g", "g",  MFnNumericData::kFloat, 0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( g ));

	sigma_a = nAttr.createColor("sigma_a", "sigma_a");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( sigma_a ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( g2, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( g2, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus mediumhomogenus::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
