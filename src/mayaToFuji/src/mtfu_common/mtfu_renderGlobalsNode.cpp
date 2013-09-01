#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtfu_renderGlobalsNode.h"

MTypeId	MayaToFujiGlobals::id(0x00106EF3);

//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 


MayaToFujiGlobals::MayaToFujiGlobals()
{}

MayaToFujiGlobals::~MayaToFujiGlobals()
{}


void *MayaToFujiGlobals::creator()
{
	return new MayaToFujiGlobals();
}

MStatus	MayaToFujiGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 

	return stat;

}

