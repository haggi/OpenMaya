#include "wireframe.h"
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

MTypeId Wireframe::id( 0x0011EF67 );

MObject Wireframe::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Wireframe::innerColor;
MObject	Wireframe::wireColor;
MObject	Wireframe::thickness;
MObject	Wireframe::fadeout;
MObject	Wireframe::angle;
//---------------------------- automatically created static attributes end ------------------------------------
	

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
void Wireframe::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Wireframe::Wireframe()
{
}

//
// DESCRIPTION:
Wireframe::~Wireframe()
{
}

//
// DESCRIPTION:
void *Wireframe::creator()
{
    return new Wireframe();
}

//
// DESCRIPTION:
MStatus Wireframe::initialize()
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
	innerColor = nAttr.createColor("innerColor", "innerColor");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( innerColor ));


	wireColor = nAttr.createColor("wireColor", "wireColor");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( wireColor ));


	thickness = nAttr.create("thickness", "thickness",  MFnNumericData::kFloat, 0.01);
	CHECK_MSTATUS(addAttribute( thickness ));


	fadeout = nAttr.create("fadeout", "fadeout",  MFnNumericData::kFloat, 0.01);
	CHECK_MSTATUS(addAttribute( fadeout ));


	angle = nAttr.create("angle", "angle",  MFnNumericData::kFloat, 45.0);
	CHECK_MSTATUS(addAttribute( angle ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Wireframe::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
