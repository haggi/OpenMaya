#include "brickTexture.h"
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

MTypeId brick::id( 0x0011EF46 );

MObject brick::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject brick::scale;
MObject brick::brickbevel;
MObject brick::brickbond;
MObject brick::mortarsize;
MObject brick::brickwidth;
MObject brick::bricktex;
MObject brick::brickdepth;
MObject brick::coordinates;
MObject brick::brickmodtex;
MObject brick::rotate;
MObject brick::motartex;
MObject brick::brickrun;
MObject brick::translate;
MObject brick::brickheight;
MObject brick::luxOutFloat;
MObject brick::luxOutColor;
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
void brick::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
brick::brick()
{
}

//
// DESCRIPTION:
brick::~brick()
{
}

//
// DESCRIPTION:
void *brick::creator()
{
    return new brick();
}

//
// DESCRIPTION:
MStatus brick::initialize()
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

	brickbevel = nAttr.create("brickbevel", "brickbevel",  MFnNumericData::kFloat, 0.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickbevel ));

	brickbond = eAttr.create("brickbond", "brickbond", 0, &status);
	status = eAttr.addField( "stacked", 0 );
	status = eAttr.addField( "flemish", 1 );
	status = eAttr.addField( "english", 2 );
	status = eAttr.addField( "herringbone", 3 );
	status = eAttr.addField( "basket", 4 );
	status = eAttr.addField( "chain link", 5 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( brickbond ));

	mortarsize = nAttr.create("mortarsize", "mortarsize",  MFnNumericData::kFloat, 0.01);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( mortarsize ));

	brickwidth = nAttr.create("brickwidth", "brickwidth",  MFnNumericData::kFloat, 0.3);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickwidth ));

	bricktex = nAttr.createColor("bricktex", "bricktex");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1.0,1.0,1.0);
	CHECK_MSTATUS(addAttribute( bricktex ));

	brickdepth = nAttr.create("brickdepth", "brickdepth",  MFnNumericData::kFloat, 0.15);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickdepth ));

	coordinates = eAttr.create("coordinates", "coordinates", 0, &status);
	status = eAttr.addField( "global", 0 );
	status = eAttr.addField( "local", 1 );
	status = eAttr.addField( "global normal", 2 );
	status = eAttr.addField( "localnormal", 3 );
	status = eAttr.addField( "uv", 4 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( coordinates ));

	brickmodtex = nAttr.create("brickmodtex", "brickmodtex",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickmodtex ));

	MObject rotateX = nAttr.create("rotateX", "rotatex", MFnNumericData::kDouble, 0.0);
	MObject rotateY = nAttr.create("rotateY", "rotatey", MFnNumericData::kDouble, 0.0);
	MObject rotateZ = nAttr.create("rotateZ", "rotatez", MFnNumericData::kDouble, 0.0);
	rotate = nAttr.create("rotate", "rotate", rotateX, rotateY, rotateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( rotate ));

	motartex = nAttr.createColor("motartex", "motartex");
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0.2,0.2,0.2);
	CHECK_MSTATUS(addAttribute( motartex ));

	brickrun = nAttr.create("brickrun", "brickrun",  MFnNumericData::kFloat, 0.75);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickrun ));

	MObject translateX = nAttr.create("translateX", "translatex", MFnNumericData::kDouble, 0.0);
	MObject translateY = nAttr.create("translateY", "translatey", MFnNumericData::kDouble, 0.0);
	MObject translateZ = nAttr.create("translateZ", "translatez", MFnNumericData::kDouble, 0.0);
	translate = nAttr.create("translate", "translate", translateX, translateY, translateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( translate ));

	brickheight = nAttr.create("brickheight", "brickheight",  MFnNumericData::kFloat, 0.1);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( brickheight ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	luxOutColor = nAttr.createColor("luxOutColor", "luxOutColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutColor ));

	CHECK_MSTATUS ( attributeAffects( scale, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( scale, luxOutColor));
	CHECK_MSTATUS ( attributeAffects( scale, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus brick::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
