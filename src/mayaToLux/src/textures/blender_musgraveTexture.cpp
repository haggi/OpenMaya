#include "blender_musgraveTexture.h"
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

MTypeId blender_musgrave::id( 0x0011EF44 );

MObject blender_musgrave::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject blender_musgrave::noisesize;
MObject blender_musgrave::outscale;
MObject blender_musgrave::scale;
MObject blender_musgrave::h;
MObject blender_musgrave::octs;
MObject blender_musgrave::coordinates;
MObject blender_musgrave::bright;
MObject blender_musgrave::rotate;
MObject blender_musgrave::lacu;
MObject blender_musgrave::gain;
MObject blender_musgrave::offset;
MObject blender_musgrave::noisebasis;
MObject blender_musgrave::translate;
MObject blender_musgrave::type;
MObject blender_musgrave::contrast;
MObject blender_musgrave::luxOutFloat;
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
void blender_musgrave::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
blender_musgrave::blender_musgrave()
{
}

//
// DESCRIPTION:
blender_musgrave::~blender_musgrave()
{
}

//
// DESCRIPTION:
void *blender_musgrave::creator()
{
    return new blender_musgrave();
}

//
// DESCRIPTION:
MStatus blender_musgrave::initialize()
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

	outscale = nAttr.create("outscale", "outscale",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( outscale ));

	MObject scaleX = nAttr.create("scaleX", "scalex", MFnNumericData::kDouble, 0.0);
	MObject scaleY = nAttr.create("scaleY", "scaley", MFnNumericData::kDouble, 0.0);
	MObject scaleZ = nAttr.create("scaleZ", "scalez", MFnNumericData::kDouble, 0.0);
	scale = nAttr.create("scale", "scale", scaleX, scaleY, scaleZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( scale ));

	h = nAttr.create("h", "h",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( h ));

	octs = nAttr.create("octs", "octs",  MFnNumericData::kFloat, 2.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( octs ));

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

	lacu = nAttr.create("lacu", "lacu",  MFnNumericData::kFloat, 2.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( lacu ));

	gain = nAttr.create("gain", "gain",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( gain ));

	offset = nAttr.create("offset", "offset",  MFnNumericData::kFloat, 1.0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( offset ));

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

	MObject translateX = nAttr.create("translateX", "translatex", MFnNumericData::kDouble, 0.0);
	MObject translateY = nAttr.create("translateY", "translatey", MFnNumericData::kDouble, 0.0);
	MObject translateZ = nAttr.create("translateZ", "translatez", MFnNumericData::kDouble, 0.0);
	translate = nAttr.create("translate", "translate", translateX, translateY, translateZ);
	MAKE_INPUT(nAttr);
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( translate ));

	type = eAttr.create("type", "type", 0, &status);
	status = eAttr.addField( "multifractal", 0 );
	status = eAttr.addField( "ridged_multifractal", 1 );
	status = eAttr.addField( "hybrid_multifractal", 2 );
	status = eAttr.addField( "hetero_terrain", 3 );
	status = eAttr.addField( "fbm", 4 );
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
MStatus blender_musgrave::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
