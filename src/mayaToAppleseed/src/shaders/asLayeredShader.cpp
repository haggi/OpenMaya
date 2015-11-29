#include "asLayeredShader.h"

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
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MDrawRegistry.h>
#include <maya/MDGModifier.h>

MTypeId	asLayeredShader::id(0x0011CF44);

void asLayeredShader::postConstructor()
{
	setMPSafe(true);
	this->setExistWithoutInConnections(true);
}

MObject asLayeredShader::baseMaterial;
MObject asLayeredShader::materialEntryMtl;
MObject asLayeredShader::materialEntryMsk;
MObject asLayeredShader::materialEntryMode;

MObject asLayeredShader::outColor;

asLayeredShader::asLayeredShader() { }
asLayeredShader::~asLayeredShader() { }

void* asLayeredShader::creator()
{
	return new asLayeredShader();
}

MStatus asLayeredShader::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	//MFnMessageAttribute mAttr;
	MFnCompoundAttribute cAttr;

	MStatus status;

	baseMaterial = nAttr.createColor("baseMaterial", "baseMaterial");
	CHECK_MSTATUS(addAttribute(baseMaterial));

	outColor = nAttr.createColor("outColor", "outColor");
	CHECK_MSTATUS(addAttribute(outColor));

	materialEntryMtl = nAttr.createColor("materialEntryMtl", "materialEntryMtl");
	nAttr.setMin(0.0f);
	nAttr.setSoftMax(1.0f);
	nAttr.setKeyable(true);
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntryMtl));

	materialEntryMsk = nAttr.create("materialEntryMsk", "materialEntryMsk", MFnNumericData::kFloat, 0.0f);
	nAttr.setMin(0.0f);
	nAttr.setMax(1.0f);
	nAttr.setKeyable(true);
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntryMsk));

	materialEntryMode = eAttr.create("materialEntryMode", "materialEntryMode", 0);
	eAttr.setKeyable(true);
	eAttr.addField("over", 0);
	eAttr.addField("multiply", 1);
	eAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntryMode));

	CHECK_MSTATUS(attributeAffects(baseMaterial, outColor));
	return(MS::kSuccess);
}

MStatus asLayeredShader::compute(const MPlug& plug, MDataBlock& block)
{
	if (plug == outColor || plug.parent() == outColor)
	{
		MStatus status;
		MFloatVector resultColor(0.0, 0.0, 0.0);

		MFloatVector& color = block.inputValue(baseMaterial, &status).asFloatVector();
		resultColor = color;

		MDataHandle outColorHandle = block.outputValue(outColor, &status);
		MFloatVector& outColorValue = outColorHandle.asFloatVector();
		outColorValue = resultColor;
		outColorHandle.setClean();
	}
	else
	{
		return(MS::kUnknownParameter);
	}

	return(MS::kSuccess);
}


