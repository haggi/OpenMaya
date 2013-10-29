#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtfu_renderGlobalsNode.h"

MTypeId	MayaToFujiGlobals::id(0x0011CF7A);

//	------------- automatically created attributes start ----------- // 

MObject MayaToFujiGlobals::sampleJitter;
MObject MayaToFujiGlobals::cast_shadow;
MObject MayaToFujiGlobals::max_reflect_depth;
MObject MayaToFujiGlobals::max_refract_depth;
MObject MayaToFujiGlobals::raymarch_step;
MObject MayaToFujiGlobals::raymarch_shadow_step;
MObject MayaToFujiGlobals::raymarch_reflect_ste;
MObject MayaToFujiGlobals::raymarch_refract_ste;
MObject MayaToFujiGlobals::sample_time_range_min;
MObject MayaToFujiGlobals::sample_time_range_max;
MObject MayaToFujiGlobals::use_max_thread;
//	------------- automatically created attributes end ----------- // 


MayaToFujiGlobals::MayaToFujiGlobals()
{
	imageFormatList.append("exr");

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

	sampleJitter = nAttr.create("sampleJitter", "sampleJitter",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( sampleJitter ));

	cast_shadow = nAttr.create("cast_shadow", "cast_shadow",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( cast_shadow ));

	max_reflect_depth = nAttr.create("max_reflect_depth", "max_reflect_depth",  MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( max_reflect_depth ));

	max_refract_depth = nAttr.create("max_refract_depth", "max_refract_depth",  MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( max_refract_depth ));

	raymarch_step = nAttr.create("raymarch_step", "raymarch_step",  MFnNumericData::kFloat, 0.05);
	CHECK_MSTATUS(addAttribute( raymarch_step ));

	raymarch_shadow_step = nAttr.create("raymarch_shadow_step", "raymarch_shadow_step",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( raymarch_shadow_step ));

	raymarch_reflect_ste = nAttr.create("raymarch_reflect_ste", "raymarch_reflect_ste",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( raymarch_reflect_ste ));

	raymarch_refract_ste = nAttr.create("raymarch_refract_ste", "raymarch_refract_ste",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( raymarch_refract_ste ));

	sample_time_range_min = nAttr.create("sample_time_range_min", "sample_time_range_min",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( sample_time_range_min ));

	sample_time_range_max = nAttr.create("sample_time_range_max", "sample_time_range_max",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( sample_time_range_max ));

	use_max_thread = nAttr.create("use_max_thread", "use_max_thread",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( use_max_thread ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

