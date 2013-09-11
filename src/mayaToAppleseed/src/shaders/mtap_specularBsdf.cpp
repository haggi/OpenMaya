#include "addTexture.h"
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

MTypeId add::id( 0x0011CF6D );

MObject add::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject add::tex2;
MObject add::tex1;
MObject add::luxOutFloat;
MObject add::luxOutColor;
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
void add::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
add::add()
{
}

//
// DESCRIPTION:
add::~add()
{
}

//
// DESCRIPTION:
void *add::creator()
{
    return new add();
}

//
// DESCRIPTION:
MStatus add::initialize()
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
	tex2 = nAttr.createColor("tex2", "tex2");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.0,0.0,0.0);
	CHECK_MSTATUS(addAttribute( tex2 ));

	tex1 = nAttr.createColor("tex1", "tex1");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.0,0.0,0.0);
	CHECK_MSTATUS(addAttribute( tex1 ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( tex2, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( tex2, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( tex2, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus add::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
