#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "mtco_renderGlobalsNode.h"

MTypeId	MayaToCoronaGlobals::id(0x00106EF3);

//	------------- automatically created attributes start ----------- // 

MObject MayaToCoronaGlobals::samplingSubdivAA;
MObject MayaToCoronaGlobals::samplingAdaptiveSteps;
MObject MayaToCoronaGlobals::samplingAdaptiveThresh;
MObject MayaToCoronaGlobals::environmentType;
MObject MayaToCoronaGlobals::environmentColor;
//	------------- automatically created attributes end ----------- // 


MayaToCoronaGlobals::MayaToCoronaGlobals()
{}

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
	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 

	samplingSubdivAA = nAttr.create("samplingSubdivAA", "samplingSubdivAA",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( samplingSubdivAA ));

	samplingAdaptiveSteps = nAttr.create("samplingAdaptiveSteps", "samplingAdaptiveSteps",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( samplingAdaptiveSteps ));

	samplingAdaptiveThresh = nAttr.create("samplingAdaptiveThresh", "samplingAdaptiveThresh",  MFnNumericData::kFloat, 0.03);
	CHECK_MSTATUS(addAttribute( samplingAdaptiveThresh ));

	environmentType = eAttr.create("environmentType", "environmentType", 0, &stat);
	stat = eAttr.addField( "Gradient", 0 );
	stat = eAttr.addField( "PhysicalSky", 1 );
	CHECK_MSTATUS(addAttribute( environmentType ));

	environmentColor = nAttr.createColor("environmentColor", "environmentColor");
	nAttr.setDefault(.3,.7,.5);
	CHECK_MSTATUS(addAttribute( environmentColor ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

