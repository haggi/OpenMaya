#include "coronaOSLNode.h"

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

MTypeId OSLNode::id( 0x0011EF57 );

MObject OSLNode::color;
MObject OSLNode::outColor;
MObject OSLNode::script;

	
//---------------------------- automatically created attributes start ------------------------------------
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
void OSLNode::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
OSLNode::OSLNode()
{
}

//
// DESCRIPTION:
OSLNode::~OSLNode()
{
}

//
// DESCRIPTION:
void *OSLNode::creator()
{
    return new OSLNode();
}

//
// DESCRIPTION:
MStatus OSLNode::initialize()
{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus status;

	color = nAttr.createColor("color", "color");
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute( color ));

	script = tAttr.create("script", "script", MFnNumericData::kString);
	MAKE_INPUT(tAttr);
	CHECK_MSTATUS(addAttribute( script ));

	outColor = nAttr.createColor("outColor", "outColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( outColor ));

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------

	attributeAffects(color, outColor);
	attributeAffects(script, outColor);

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus OSLNode::compute(const MPlug &plug, MDataBlock &block)
{

	if( plug == outColor )
	{
		// compile shader
	}
    return MS::kSuccess;
}
