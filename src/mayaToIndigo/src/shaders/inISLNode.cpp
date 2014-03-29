#include "inISLNode.h"

#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MDrawRegistry.h>
#include <maya/MDGModifier.h>

MTypeId	inISLNode::id( 0x0011CF60 );


//void inISLNode::postConstructor( )
//{}


MObject  inISLNode::islData;
MObject  inISLNode::outColor;
MObject  inISLNode::outFloat;

inISLNode::inISLNode() { }
inISLNode::~inISLNode() { }

void* inISLNode::creator()
{
    return new inISLNode();
}

MStatus inISLNode::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; 

	islData = tAttr.create("islData", "islData",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( islData ));

	outColor = nAttr.createColor("outColor", "outColor");
	CHECK_MSTATUS(addAttribute( outColor ));

	outFloat = nAttr.create("outFloat", "outFloat", MFnNumericData::kFloat, 0.0f);
	CHECK_MSTATUS(addAttribute( outFloat ));

    return( MS::kSuccess );
}

MStatus inISLNode::compute( const MPlug& plug, MDataBlock& block )
{
    return( MS::kSuccess );
}


