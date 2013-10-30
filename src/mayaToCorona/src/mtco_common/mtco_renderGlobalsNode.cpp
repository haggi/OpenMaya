#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtco_renderGlobalsNode.h"

MTypeId	MayaToCoronaGlobals::id(0x00106EF3);

//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 


MayaToCoronaGlobals::MayaToCoronaGlobals()
{
	imageFormatList.append("Exr");
}

MayaToCoronaGlobals::~MayaToCoronaGlobals()
{}


void *MayaToCoronaGlobals::creator()
{
	return new MayaToCoronaGlobals();
}

MStatus	MayaToCoronaGlobals::initialize()
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

