#include "CoronaLayeredMaterial.h"

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

MTypeId	CoronaLayered::id(0x0011CF72);

void CoronaLayered::postConstructor( )
{
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}

MObject CoronaLayered::baseMaterial;
MObject CoronaLayered::materialEntry;
MObject CoronaLayered::materialEntryMtl;
MObject CoronaLayered::materialEntryAmount;

MObject CoronaLayered::outColor;

CoronaLayered::CoronaLayered() { }
CoronaLayered::~CoronaLayered() { }

void* CoronaLayered::creator()
{
    return new CoronaLayered();
}

MStatus CoronaLayered::initialize()
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

	MObject entryAmount = nAttr.createColor("entryAmount", "entryAmount");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(entryAmount));

	MObject entryMaterial = nAttr.createColor("entryMaterial", "entryMaterial");
	CHECK_MSTATUS(addAttribute(entryMaterial));

	materialEntry = cAttr.create("materialEntry", "materialEntry");
	cAttr.addChild(entryMaterial);
	cAttr.addChild(entryAmount);
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


    return( MS::kSuccess );
}

MStatus CoronaLayered::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug == outColor || plug.parent() == outColor )
    {
        MStatus status;
        MFloatVector resultColor( 0.0, 0.0, 0.0 );
		
		MFloatVector& color = block.inputValue(baseMaterial, &status).asFloatVector();
		resultColor = color;

        MDataHandle outColorHandle = block.outputValue( outColor, &status );
        MFloatVector& outColorValue = outColorHandle.asFloatVector();
		outColorValue = resultColor;  
        outColorHandle.setClean(); 
    }
    else
    {
        return( MS::kUnknownParameter ); 
    }

    return( MS::kSuccess );
}


