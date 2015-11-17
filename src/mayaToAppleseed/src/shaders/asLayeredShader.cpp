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
MObject asLayeredShader::materialEntry;
MObject asLayeredShader::materialEntryMtl;
MObject asLayeredShader::materialEntryAmount;

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

	MObject entryMask = nAttr.createColor("entryMask", "entryMask");
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(entryMask));

	MObject entryMaterial = nAttr.createColor("entryMaterial", "entryMaterial");
	CHECK_MSTATUS(addAttribute(entryMaterial));

	MObject entryMixType = eAttr.create("entryMixType", "entryMixType", 0);
	eAttr.addField("Over", 0);
	eAttr.addField("Multiply", 1);
	CHECK_MSTATUS(addAttribute(entryMixType));

	materialEntry = cAttr.create("materialEntry", "materialEntry");
	cAttr.addChild(entryMaterial);
	cAttr.addChild(entryMask);
	cAttr.addChild(entryMixType);
	cAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntry));

	materialEntryMtl = nAttr.createColor("materialEntryMtl", "materialEntryMtl");
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntryMtl));

	materialEntryAmount = nAttr.createColor("materialEntryAmount", "materialEntryAmount");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(materialEntryAmount));
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


