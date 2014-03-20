#include "blender_cloudsTexture.h"
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

MTypeId blender_clouds::id( 0x0011EF41 );

MObject blender_clouds::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject blender_clouds::noisesize;
MObject blender_clouds::noisebasis;
MObject blender_clouds::scale;
MObject blender_clouds::noisedepth;
MObject blender_clouds::coordinates;
MObject blender_clouds::bright;
MObject blender_clouds::rotate;
MObject blender_clouds::translate;
MObject blender_clouds::luxOutFloat;
MObject blender_clouds::contrast;
MObject blender_clouds::noisetype;
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
void blender_clouds::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
blender_clouds::blender_clouds()
{
}

//
// DESCRIPTION:
blender_clouds::~blender_clouds()
{
}

//
// DESCRIPTION:
void *blender_clouds::creator()
{
    return new blender_clouds();
}

//
// DESCRIPTION:
MStatus blender_clouds::initialize()
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

	noisedepth = nAttr.create("noisedepth", "noisedepth",  MFnNumericData::kInt, 2);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( noisedepth ));

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

	contrast = nAttr.create("contrast", "contrast",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( contrast ));

	noisetype = eAttr.create("noisetype", "noisetype", 0, &status);
	status = eAttr.addField( "soft_noise", 0 );
	status = eAttr.addField( "hard_noise", 1 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( noisetype ));

	CHECK_MSTATUS ( attributeAffects( noisesize, luxOutFloat));
	CHECK_MSTATUS ( attributeAffects( noisesize, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus blender_clouds::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
