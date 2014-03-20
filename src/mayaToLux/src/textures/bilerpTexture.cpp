#include "bilerpTexture.h"
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

MTypeId bilerp::id( 0x0011CF7F );

MObject bilerp::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject bilerp::v11;
MObject bilerp::v01;
MObject bilerp::v00;
MObject bilerp::vdelta;
MObject bilerp::udelta;
MObject bilerp::mapping;
MObject bilerp::v1;
MObject bilerp::v2;
MObject bilerp::uscale;
MObject bilerp::v10;
MObject bilerp::vscale;
MObject bilerp::luxOutFloat;
MObject bilerp::luxOutColor;
MObject bilerp::luxOutFresnel;
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
void bilerp::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
bilerp::bilerp()
{
}

//
// DESCRIPTION:
bilerp::~bilerp()
{
}

//
// DESCRIPTION:
void *bilerp::creator()
{
    return new bilerp();
}

//
// DESCRIPTION:
MStatus bilerp::initialize()
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
	v11 = nAttr.createColor("v11", "v11");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( v11 ));

	v01 = nAttr.createColor("v01", "v01");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( v01 ));

	v00 = nAttr.createColor("v00", "v00");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( v00 ));

	vdelta = nAttr.create("vdelta", "vdelta",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( vdelta ));

	udelta = nAttr.create("udelta", "udelta",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( udelta ));

	mapping = eAttr.create("mapping", "mapping", 0, &status);
	status = eAttr.addField( "uv", 0 );
	status = eAttr.addField( "spherical", 1 );
	status = eAttr.addField( "cylindrical", 2 );
	status = eAttr.addField( "planar", 3 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( mapping ));

	MObject v1X = nAttr.create("v1X", "v1x", MFnNumericData::kDouble, 0.0);
	MObject v1Y = nAttr.create("v1Y", "v1y", MFnNumericData::kDouble, 0.0);
	MObject v1Z = nAttr.create("v1Z", "v1z", MFnNumericData::kDouble, 0.0);
	v1 = nAttr.create("v1", "v1", v1X, v1Y, v1Z);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1,0,0);
	CHECK_MSTATUS(addAttribute( v1 ));

	MObject v2X = nAttr.create("v2X", "v2x", MFnNumericData::kDouble, 0.0);
	MObject v2Y = nAttr.create("v2Y", "v2y", MFnNumericData::kDouble, 0.0);
	MObject v2Z = nAttr.create("v2Z", "v2z", MFnNumericData::kDouble, 0.0);
	v2 = nAttr.create("v2", "v2", v2X, v2Y, v2Z);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,1,0);
	CHECK_MSTATUS(addAttribute( v2 ));

	uscale = nAttr.create("uscale", "uscale",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( uscale ));

	v10 = nAttr.createColor("v10", "v10");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( v10 ));

	vscale = nAttr.create("vscale", "vscale",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( vscale ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	luxOutFresnel = nAttr.create("luxOutFresnel", "luxOutFresnel",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFresnel ));

	CHECK_MSTATUS ( attributeAffects( v11, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( v11, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( v11, luxOutFresnel));
	CHECK_MSTATUS ( attributeAffects( v11, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus bilerp::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
