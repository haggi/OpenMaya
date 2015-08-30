#include "coronaWire.h"

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

MTypeId	CoronaWire::id(0x0011CF45);

void CoronaWire::postConstructor( )
{
	MStatus stat;
	setMPSafe( true );
	this->setExistWithoutInConnections(true);
}

CoronaWire::CoronaWire() { }
CoronaWire::~CoronaWire() { }


void* CoronaWire::creator()
{
    return new CoronaWire();
}

MStatus CoronaWire::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; 
    return( MS::kSuccess );
}

MStatus CoronaWire::compute( const MPlug& plug, MDataBlock& block )
{
    return( MS::kSuccess );
}


