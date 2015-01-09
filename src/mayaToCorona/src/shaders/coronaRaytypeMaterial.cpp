#include "CoronaRaytypeMaterial.h"

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

MTypeId	CoronaRaytype::id(0x0011EF71);

void CoronaRaytype::postConstructor( )
{
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}

MObject CoronaRaytype::base;
MObject CoronaRaytype::reflect;
MObject CoronaRaytype::refract;
MObject CoronaRaytype::direct;
MObject CoronaRaytype::outColor;

CoronaRaytype::CoronaRaytype() { }
CoronaRaytype::~CoronaRaytype() { }

void* CoronaRaytype::creator()
{
    return new CoronaRaytype();
}

MStatus CoronaRaytype::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; 

	base = nAttr.createColor("base", "base");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(base));

	reflect = nAttr.createColor("reflect", "reflect");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(reflect));

	refract = nAttr.createColor("refract", "refract");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(refract));

	direct = nAttr.createColor("direct", "direct");
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	CHECK_MSTATUS(addAttribute(direct));

	outColor = nAttr.createColor("outColor", "outColor");
	CHECK_MSTATUS(addAttribute(outColor));

	CHECK_MSTATUS(attributeAffects(base, outColor));
	CHECK_MSTATUS(attributeAffects(reflect, outColor));
	CHECK_MSTATUS(attributeAffects(refract, outColor));
	CHECK_MSTATUS(attributeAffects(direct, outColor));

    return( MS::kSuccess );
}

MStatus CoronaRaytype::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug == outColor || plug.parent() == outColor )
    {
        MStatus status;
        MFloatVector resultColor( 0.0, 0.0, 0.0 );

        MFloatVector& baseValue = block.inputValue( base, &status ).asFloatVector();
        CHECK_MSTATUS( status );
		resultColor[0] = baseValue[0];
		resultColor[1] = baseValue[1];
		resultColor[2] = baseValue[2];

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


