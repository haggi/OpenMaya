#include "fresnelTexture.h"
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

MTypeId fresnel::id( 0x0011CF64 );

MObject fresnel::outColor;

	
//---------------------------- automatically created attributes start ------------------------------------
MObject fresnel::filename;
MObject fresnel::name;
MObject fresnel::luxOutFresnel;
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
void fresnel::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
fresnel::fresnel()
{
}

//
// DESCRIPTION:
fresnel::~fresnel()
{
}

//
// DESCRIPTION:
void *fresnel::creator()
{
    return new fresnel();
}

//
// DESCRIPTION:
MStatus fresnel::initialize()
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
	filename = tAttr.create("filename", "filename",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( filename ));

	name = eAttr.create("name", "name", 0, &status);
	status = eAttr.addField( "aluminium", 0 );
	status = eAttr.addField( "amorphous carbon", 1 );
	status = eAttr.addField( "silver", 2 );
	status = eAttr.addField( "gold", 3 );
	status = eAttr.addField( "copper", 4 );
	MAKE_INPUT(eAttr);
	CHECK_MSTATUS(addAttribute( name ));

	luxOutFresnel = nAttr.create("luxOutFresnel", "luxOutFresnel",  MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( luxOutFresnel ));

	CHECK_MSTATUS ( attributeAffects( filename, luxOutFresnel));
	CHECK_MSTATUS ( attributeAffects( filename, outColor));
//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus fresnel::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
