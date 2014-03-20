#include "colordepthTexture.h"
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

MTypeId colordepth::id( 0x0011EF47 );

MObject colordepth::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject colordepth::depth;
MObject colordepth::Kt;
MObject colordepth::luxOutColor;
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
void colordepth::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
colordepth::colordepth()
{
}

//
// DESCRIPTION:
colordepth::~colordepth()
{
}

//
// DESCRIPTION:
void *colordepth::creator()
{
    return new colordepth();
}

//
// DESCRIPTION:
MStatus colordepth::initialize()
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
	depth = nAttr.create("depth", "depth",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( depth ));

	Kt = nAttr.createColor("Kt", "Kt");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.0,0.0,0.0);
	CHECK_MSTATUS(addAttribute( Kt ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( depth, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( depth, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus colordepth::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
