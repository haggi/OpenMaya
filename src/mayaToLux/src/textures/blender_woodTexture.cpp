#include "blender_woodTexture.h"
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

MTypeId blender_wood::id( 0x0011CF75 );

MObject blender_wood::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject blender_wood::noisesize;
MObject blender_wood::noisebasis2;
MObject blender_wood::noisebasis;
MObject blender_wood::scale;
MObject blender_wood::coordinates;
MObject blender_wood::bright;
MObject blender_wood::rotate;
MObject blender_wood::noisetype;
MObject blender_wood::turbulence;
MObject blender_wood::translate;
MObject blender_wood::type;
MObject blender_wood::contrast;
MObject blender_wood::luxOutFloat;
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
void blender_wood::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
blender_wood::blender_wood()
{
}

//
// DESCRIPTION:
blender_wood::~blender_wood()
{
}

//
// DESCRIPTION:
void *blender_wood::creator()
{
    return new blender_wood();
}

//
// DESCRIPTION:
MStatus blender_wood::initialize()
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
	noisesize = nAttr.create("noisesize", "noisesize",  MFnNumericData::kFloat, 0.25);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( noisesize ));

	noisebasis2 = eAttr.create("noisebasis2", "noisebasis2", 0, &status);
	status = eAttr.addField( "sin", 0 );
	status = eAttr.addField( "saw", 1 );
	status = eAttr.addField( "tri", 2 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( noisebasis2 ));

	noisebasis = eAttr.create("noisebasis", "noisebasis", 0, &status);
	status = eAttr.addField( "blender_original", 0 );
	status = eAttr.addField( "original_perlin", 1 );
	status = eAttr.addField( "improved_perlin", 2 );
	status = eAttr.addField( "voronoi_f1", 3 );
	status = eAttr.addField( "voronoi_f2", 4 );
	status = eAttr.addField( "voronoi_f3", 5 );
	status = eAttr.addField( "voronoi_f4", 6 );
	status = eAttr.addField( "voronoi_f2f1", 7 );
	status = eAttr.addField( "voronoi_crackle", 8 );
	status = eAttr.addField( "cell_noise", 9 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( noisebasis ));

	MObject scaleX = nAttr.create("scaleX", "scalex", MFnNumericData::kDouble, 0.0);
	MObject scaleY = nAttr.create("scaleY", "scaley", MFnNumericData::kDouble, 0.0);
	MObject scaleZ = nAttr.create("scaleZ", "scalez", MFnNumericData::kDouble, 0.0);
	scale = nAttr.create("scale", "scale", scaleX, scaleY, scaleZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( scale ));

	coordinates = eAttr.create("coordinates", "coordinates", 0, &status);
	status = eAttr.addField( "global", 0 );
	status = eAttr.addField( "local", 1 );
	status = eAttr.addField( "global normal", 2 );
	status = eAttr.addField( "localnormal", 3 );
	status = eAttr.addField( "uv", 4 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( coordinates ));

	bright = nAttr.create("bright", "bright",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( bright ));

	MObject rotateX = nAttr.create("rotateX", "rotatex", MFnNumericData::kDouble, 0.0);
	MObject rotateY = nAttr.create("rotateY", "rotatey", MFnNumericData::kDouble, 0.0);
	MObject rotateZ = nAttr.create("rotateZ", "rotatez", MFnNumericData::kDouble, 0.0);
	rotate = nAttr.create("rotate", "rotate", rotateX, rotateY, rotateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( rotate ));

	noisetype = eAttr.create("noisetype", "noisetype", 1, &status);
	status = eAttr.addField( "soft_noise", 0 );
	status = eAttr.addField( "hard_noise", 1 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( noisetype ));

	turbulence = nAttr.create("turbulence", "turbulence",  MFnNumericData::kFloat, 5.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( turbulence ));

	MObject translateX = nAttr.create("translateX", "translatex", MFnNumericData::kDouble, 0.0);
	MObject translateY = nAttr.create("translateY", "translatey", MFnNumericData::kDouble, 0.0);
	MObject translateZ = nAttr.create("translateZ", "translatez", MFnNumericData::kDouble, 0.0);
	translate = nAttr.create("translate", "translate", translateX, translateY, translateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( translate ));

	type = eAttr.create("type", "type", 0, &status);
	status = eAttr.addField( "bands", 0 );
	status = eAttr.addField( "rings", 1 );
	status = eAttr.addField( "bandnoise", 2 );
	status = eAttr.addField( "ringnoise", 3 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( type ));

	contrast = nAttr.create("contrast", "contrast",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( contrast ));

	luxOutFloat = nAttr.create("luxOutFloat", "luxOutFloat",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFloat ));

	CHECK_MSTATUS ( attributeAffects( noisesize, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( noisesize, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus blender_wood::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
