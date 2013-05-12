#include "bandTexture.h"
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

MTypeId band::id( 0x0011CF74 );

MObject band::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject band::tex;
MObject band::amount;
MObject band::offsets;
MObject band::luxOutFloat;
MObject band::luxOutColor;
MObject band::luxOutFresnel;
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
void band::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
band::band()
{
}

//
// DESCRIPTION:
band::~band()
{
}

//
// DESCRIPTION:
void *band::creator()
{
    return new band();
}

//
// DESCRIPTION:
MStatus band::initialize()
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
	tex = nAttr.createColor("tex", "tex");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.0,0.0,0.0);
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute( tex ));

	amount = nAttr.create("amount", "amount",  MFnNumericData::kFloat, 0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( amount ));

	offsets = nAttr.create("offsets", "offsets",  MFnNumericData::kFloat, 0.0);
	nAttr.setArray(true);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( offsets ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	luxOutFresnel = nAttr.create("luxOutFresnel", "luxOutFresnel",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFresnel ));

	CHECK_MSTATUS ( attributeAffects( tex, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( tex, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( tex, luxOutFresnel));
	CHECK_MSTATUS ( attributeAffects( tex, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus band::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
