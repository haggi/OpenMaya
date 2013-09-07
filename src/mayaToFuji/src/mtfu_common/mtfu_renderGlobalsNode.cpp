#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtfu_renderGlobalsNode.h"

MTypeId	MayaToFujiGlobals::id(0x0011CF7A);

//	------------- automatically created attributes start ----------- // 

MObject MayaToFujiGlobals::pixelFilter;
//	------------- automatically created attributes end ----------- // 


MayaToFujiGlobals::MayaToFujiGlobals()
{
	imageFormatList.append("png");
	imageFormatList.append("exr");
	imageFormatList.append("tif");

	filterTypeList.append("gauss");
	filterTypeList.append("box");
	filterTypeList.append("triangle");
}

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

	pixelFilter = eAttr.create("pixelFilter", "pixelFilter", 0, &stat);
	stat = eAttr.addField( "gauss", 0 );
	stat = eAttr.addField( "mitchell", 1 );
	stat = eAttr.addField( "triangle", 2 );
	CHECK_MSTATUS(addAttribute( pixelFilter ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

