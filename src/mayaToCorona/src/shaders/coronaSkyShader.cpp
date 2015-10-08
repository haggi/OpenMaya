#include "CoronaSkyShader.h"

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

MTypeId	CoronaSkyShader::id(0x0011CF46);

MObject CoronaSkyShader::pSkyModel;
MObject CoronaSkyShader::pSkyMultiplier;
MObject CoronaSkyShader::pSkyHorizBlur;
MObject CoronaSkyShader::pSkyGroundColor;
MObject CoronaSkyShader::pSkyAffectGround;
MObject CoronaSkyShader::pSkyPreethamTurb;
MObject CoronaSkyShader::pSkySunFalloff;
MObject CoronaSkyShader::pSkyZenith;
MObject CoronaSkyShader::pSkyHorizon;
MObject CoronaSkyShader::pSkySunGlow;
MObject CoronaSkyShader::pSkySunSideGlow;
MObject CoronaSkyShader::pSkySunBleed;
MObject CoronaSkyShader::sunSizeMulti;
MObject CoronaSkyShader::outColor;

void CoronaSkyShader::postConstructor( )
{
	MStatus stat;
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}


CoronaSkyShader::CoronaSkyShader() { }
CoronaSkyShader::~CoronaSkyShader() { }

void* CoronaSkyShader::creator()
{
    return new CoronaSkyShader();
}

MStatus CoronaSkyShader::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus stat; 

	pSkyModel = eAttr.create("pSkyModel", "pSkyModel", 2, &stat);
	stat = eAttr.addField("Preetham", 0);
	stat = eAttr.addField("Rawafake", 1);
	stat = eAttr.addField("Hosek", 2);
	CHECK_MSTATUS(addAttribute(pSkyModel));

	pSkyMultiplier = nAttr.create("pSkyMultiplier", "pSkyMultiplier", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0001f);
	nAttr.setSoftMax(1.0f);
	CHECK_MSTATUS(addAttribute(pSkyMultiplier));

	pSkyHorizBlur = nAttr.create("pSkyHorizBlur", "pSkyHorizBlur", MFnNumericData::kFloat, 0.1);
	nAttr.setMin(0.0f);
	nAttr.setMax(1.0f);
	CHECK_MSTATUS(addAttribute(pSkyHorizBlur));

	pSkyGroundColor = nAttr.createColor("pSkyGroundColor", "pSkyGroundColor");
	nAttr.setDefault(0.25, 0.25, 0.25);
	CHECK_MSTATUS(addAttribute(pSkyGroundColor));

	pSkyAffectGround = nAttr.create("pSkyAffectGround", "pSkyAffectGround", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute(pSkyAffectGround));

	pSkyPreethamTurb = nAttr.create("pSkyPreethamTurb", "pSkyPreethamTurb", MFnNumericData::kFloat, 2.5);
	nAttr.setMin(1.7f);
	nAttr.setMax(10.0f);
	CHECK_MSTATUS(addAttribute(pSkyPreethamTurb));

	pSkySunFalloff = nAttr.create("pSkySunFalloff", "pSkySunFalloff", MFnNumericData::kFloat, 3.0);
	nAttr.setMin(0.0001f);
	nAttr.setSoftMax(10.0f);
	CHECK_MSTATUS(addAttribute(pSkySunFalloff));

	pSkyZenith = nAttr.createColor("pSkyZenith", "pSkyZenith");
	nAttr.setDefault(0.1, 0.1, 0.5);
	CHECK_MSTATUS(addAttribute(pSkyZenith));

	pSkyHorizon = nAttr.createColor("pSkyHorizon", "pSkyHorizon");
	nAttr.setDefault(0.25, 0.5, 0.5);
	CHECK_MSTATUS(addAttribute(pSkyHorizon));

	pSkySunGlow = nAttr.create("pSkySunGlow", "pSkySunGlow", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0f);
	nAttr.setMax(1.0f);
	CHECK_MSTATUS(addAttribute(pSkySunGlow));

	pSkySunSideGlow = nAttr.create("pSkySunSideGlow", "pSkySunSideGlow", MFnNumericData::kFloat, 0.2);
	nAttr.setMin(0.0f);
	nAttr.setMax(1.0f);
	CHECK_MSTATUS(addAttribute(pSkySunSideGlow));

	pSkySunBleed = nAttr.create("pSkySunBleed", "pSkySunBleed", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0f);
	nAttr.setMax(1.0f);
	CHECK_MSTATUS(addAttribute(pSkySunBleed));

	sunSizeMulti = nAttr.create("sunSizeMulti", "sunSizeMulti", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.1);
	nAttr.setSoftMax(64.0);
	CHECK_MSTATUS(addAttribute(sunSizeMulti));

	outColor = nAttr.createColor("outColor", "outColor");
	nAttr.setKeyable(false);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	CHECK_MSTATUS(addAttribute(outColor));

	CHECK_MSTATUS(attributeAffects(pSkyMultiplier, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyHorizBlur, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyGroundColor, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyAffectGround, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyPreethamTurb, outColor));
	CHECK_MSTATUS(attributeAffects(pSkySunFalloff, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyZenith, outColor));
	CHECK_MSTATUS(attributeAffects(pSkyHorizon, outColor));
	CHECK_MSTATUS(attributeAffects(pSkySunGlow, outColor));
	CHECK_MSTATUS(attributeAffects(pSkySunBleed, outColor));
	CHECK_MSTATUS(attributeAffects(sunSizeMulti, outColor));

    return( MS::kSuccess );
}

MStatus CoronaSkyShader::compute( const MPlug& plug, MDataBlock& block )
{

    return( MS::kSuccess );
}


