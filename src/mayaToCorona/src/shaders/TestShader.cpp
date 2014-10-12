#include "TestShader.h"

MTypeId TestShader::id(0x81000);

void TestShader::postConstructor()
{
	setMPSafe(true);
}

MObject  TestShader::aColor;
MObject  TestShader::aOutColor;

TestShader::TestShader(){}
TestShader::~TestShader(){}

void* TestShader::creator()
{
	return new TestShader();
}

MStatus TestShader::initialize()
{
	MFnNumericAttribute nAttr;

	MStatus status; 

	aColor = nAttr.createColor("color", "color");
	CHECK_MSTATUS(addAttribute(aColor));

	aOutColor = nAttr.createColor("outColor", "oc");
	CHECK_MSTATUS(addAttribute(aOutColor));
	CHECK_MSTATUS( attributeAffects( aColor, aOutColor ) );
	
	return(MS::kSuccess);
}

MStatus TestShader::compute(const MPlug& plug, MDataBlock& block)
{
	MStatus status;
	if (plug == aOutColor || plug.parent() == aOutColor)
	{
		MDataHandle outColorHandle = block.outputValue(aOutColor, &status);
		CHECK_MSTATUS(status);
		MFloatVector& outColor = outColorHandle.asFloatVector();
		MDataHandle iColor = block.inputValue(aColor, &status);
		MFloatVector& inColor = iColor.asFloatVector();
		outColor = inColor;
		outColorHandle.setClean();
		return(MS::kSuccess);
	}
	else
	{
		return(MS::kUnknownParameter); // We got an unexpected plug
	}

	return(MS::kSuccess);
}

