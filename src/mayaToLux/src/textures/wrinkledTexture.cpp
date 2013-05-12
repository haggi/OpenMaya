#include "wrinkledTexture.h"
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

MTypeId wrinkled::id( 0x0011CF78 );

MObject wrinkled::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject wrinkled::scale;
MObject wrinkled::octaves;
MObject wrinkled::coordinates;
MObject wrinkled::roughness;
MObject wrinkled::rotate;
MObject wrinkled::translate;
MObject wrinkled::luxOutFloat;
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
void wrinkled::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
wrinkled::wrinkled()
{
}

//
// DESCRIPTION:
wrinkled::~wrinkled()
{
}

//
// DESCRIPTION:
void *wrinkled::creator()
{
    return new wrinkled();
}

//
// DESCRIPTION:
MStatus wrinkled::initialize()
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
	MObject scaleX = nAttr.create("scaleX", "scalex", MFnNumericData::kDouble, 0.0);
	MObject scaleY = nAttr.create("scaleY", "scaley", MFnNumericData::kDouble, 0.0);
	MObject scaleZ = nAttr.create("scaleZ", "scalez", MFnNumericData::kDouble, 0.0);
	scale = nAttr.create("scale", "scale", scaleX, scaleY, scaleZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( scale ));

	octaves = nAttr.create("octaves", "octaves",  MFnNumericData::kInt, 8);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( octaves ));

	coordinates = eAttr.create("coordinates", "coordinates", 0, &status);
	status = eAttr.addField( "global", 0 );
	status = eAttr.addField( "local", 1 );
	status = eAttr.addField( "global normal", 2 );
	status = eAttr.addField( "localnormal", 3 );
	status = eAttr.addField( "uv", 4 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( coordinates ));

	roughness = nAttr.create("roughness", "roughness",  MFnNumericData::kFloat, 0.5);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( roughness ));

	MObject rotateX = nAttr.create("rotateX", "rotatex", MFnNumericData::kDouble, 0.0);
	MObject rotateY = nAttr.create("rotateY", "rotatey", MFnNumericData::kDouble, 0.0);
	MObject rotateZ = nAttr.create("rotateZ", "rotatez", MFnNumericData::kDouble, 0.0);
	rotate = nAttr.create("rotate", "rotate", rotateX, rotateY, rotateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( rotate ));

	MObject translateX = nAttr.create("translateX", "translatex", MFnNumericData::kDouble, 0.0);
	MObject translateY = nAttr.create("translateY", "translatey", MFnNumericData::kDouble, 0.0);
	MObject translateZ = nAttr.create("translateZ", "translatez", MFnNumericData::kDouble, 0.0);
	translate = nAttr.create("translate", "translate", translateX, translateY, translateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( translate ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	CHECK_MSTATUS ( attributeAffects( scale, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( scale, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus wrinkled::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
