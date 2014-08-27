#include "LayeredMaterial.h"

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

// IFF type ID
// Each node requires a unique identifier which is used by
// MFnDependencyNode::create() to identify which node to create, and by
// the Maya file format.
//
// For local testing of nodes you can use any identifier between
// 0x00000000 and 0x0007ffff, but for any node that you plan to use for
// more permanent purposes, you should get a universally unique id from
// Autodesk Support. You will be assigned a unique range that you
// can manage on your own.
//
MTypeId	Layered::id(0x0011EF6D);


// the postConstructor() function is called immediately after the objects
// constructor. It is not safe to call MPxNode member functions from the
// constructor, instead they should be called here.
//
void Layered::postConstructor( )
{
    // setMPSafe indicates that this shader can be used for multiprocessor
    // rendering. For a shading node to be MP safe, it cannot access any
    // shared global data and should only use attributes in the datablock
    // to get input data and store output data.
    //
    setMPSafe( true );
}

MObject  Layered::baseShader;
MObject  Layered::layerWeight;
MObject  Layered::layerTexture;
MObject  Layered::layerShader;
MObject  Layered::outColor;

MObject  Layered::aTranslucenceCoeff;
MObject  Layered::aDiffuseReflectivity;
MObject  Layered::aInTransparency;
MObject  Layered::aColor;
MObject  Layered::aIncandescence;
MObject  Layered::aOutTransparency;
MObject  Layered::aNormalCamera;
MObject  Layered::aNormalCameraX;
MObject  Layered::aNormalCameraY;
MObject  Layered::aNormalCameraZ;
MObject  Layered::aLightData;
MObject  Layered::aLightDirection;
MObject  Layered::aLightDirectionX;
MObject  Layered::aLightDirectionY;
MObject  Layered::aLightDirectionZ;
MObject  Layered::aLightIntensity;
MObject  Layered::aLightIntensityR;
MObject  Layered::aLightIntensityG;
MObject  Layered::aLightIntensityB;
MObject  Layered::aLightAmbient;
MObject  Layered::aLightDiffuse;
MObject  Layered::aLightSpecular;
MObject  Layered::aLightShadowFraction;
MObject  Layered::aPreShadowIntensity;
MObject  Layered::aLightBlindData;


//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------


// This node does not need to perform any special actions on creation or
// destruction
//

Layered::Layered() { }
Layered::~Layered() { }


// The creator() method allows Maya to instantiate instances of this node.
// It is called every time a new instance of the node is requested by
// either the createNode command or the MFnDependencyNode::create()
// method.
//
// In this case creator simply returns a new Layered object.
//

void* Layered::creator()
{
    return new Layered();
}


// The initialize method is called only once when the node is first
// registered with Maya. In this method you define the attributes of the
// node, what data comes in and goes out of the node that other nodes may
// want to connect to.
//

MStatus Layered::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

    MStatus status; // Status will be used to hold the MStatus value
                    // returned by each api function call. It is important
                    // to check the status returned by a call to aid in
                    // debugging. Failed API calls can result in subtle
                    // errors that can be difficult to track down, you may
                    // wish to use the CHECK_MSTATUS macro for any API
                    // call where you do not need to provide your own
                    // error handling.
                    //

	baseShader = nAttr.createColor("baseShader", "baseShader");
	CHECK_MSTATUS(addAttribute(baseShader));

	layerWeight = nAttr.create("layerWeight", "layerWeight", MFnNumericData::kFloat, 0, &status);
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(layerWeight));

	layerTexture = mAttr.create("layerTexture", "layerTexture");
	mAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(layerTexture));

	layerShader = nAttr.createColor("layerShader", "layerShader");
	nAttr.setArray(true);
	CHECK_MSTATUS(addAttribute(layerShader));

	outColor = nAttr.createColor("outColor", "outColor");
	CHECK_MSTATUS(addAttribute(outColor));

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------


	aTranslucenceCoeff = nAttr.create("translucenceCoeff", "tc",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setDefault(0.0f));

	aDiffuseReflectivity = nAttr.create("diffuseReflectivity", "drfl",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setDefault(0.8f));

	aColor = nAttr.createColor("color", "c", &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.58824f, 0.644f));

	aIncandescence = nAttr.createColor("incandescence", "ic", &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.0f, 0.0f));

	aInTransparency = nAttr.createColor("transparency", "it", &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setKeyable(true));
	CHECK_MSTATUS(nAttr.setStorable(true));
	CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.0f, 0.0f));

	CHECK_MSTATUS(nAttr.setHidden(false));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));

	aOutTransparency = nAttr.createColor("outTransparency", "ot", &status);
	CHECK_MSTATUS(status);

	CHECK_MSTATUS(nAttr.setHidden(false));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));

	// Camera Normals
	//
	aNormalCameraX = nAttr.create("normalCameraX", "nx",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aNormalCameraY = nAttr.create("normalCameraY", "ny",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aNormalCameraZ = nAttr.create("normalCameraZ", "nz",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aNormalCamera = nAttr.create("normalCamera", "n", aNormalCameraX,
		aNormalCameraY, aNormalCameraZ, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));
	CHECK_MSTATUS(nAttr.setHidden(true));


	// Light Direction
	//
	aLightDirectionX = nAttr.create("lightDirectionX", "ldx",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightDirectionY = nAttr.create("lightDirectionY", "ldy",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightDirectionZ = nAttr.create("lightDirectionZ", "ldz",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightDirection = nAttr.create("lightDirection", "ld",
		aLightDirectionX, aLightDirectionY, aLightDirectionZ,
		&status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));


	// Light Intensity
	//
	aLightIntensityR = nAttr.create("lightIntensityR", "lir",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightIntensityG = nAttr.create("lightIntensityG", "lig",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightIntensityB = nAttr.create("lightIntensityB", "lib",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightIntensity = nAttr.create("lightIntensity", "li",
		aLightIntensityR, aLightIntensityG, aLightIntensityB,
		&status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));


	// Light
	//
	aLightAmbient = nAttr.create("lightAmbient", "la",
		MFnNumericData::kBoolean, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(true));

	aLightDiffuse = nAttr.create("lightDiffuse", "ldf",
		MFnNumericData::kBoolean, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(true));

	aLightSpecular = nAttr.create("lightSpecular", "ls",
		MFnNumericData::kBoolean, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(false));

	aLightShadowFraction = nAttr.create("lightShadowFraction", "lsf",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aPreShadowIntensity = nAttr.create("preShadowIntensity", "psi",
		MFnNumericData::kFloat, 0, &status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));
	CHECK_MSTATUS(nAttr.setDefault(1.0f));

	aLightBlindData = nAttr.createAddr("lightBlindData", "lbld",
		&status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(nAttr.setStorable(false));
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(nAttr.setReadable(true));
	CHECK_MSTATUS(nAttr.setWritable(false));

	aLightData = lAttr.create("lightDataArray", "ltd", aLightDirection,
		aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular,
		aLightShadowFraction, aPreShadowIntensity, aLightBlindData,
		&status);
	CHECK_MSTATUS(status);
	CHECK_MSTATUS(lAttr.setArray(true));
	CHECK_MSTATUS(lAttr.setStorable(false));
	CHECK_MSTATUS(lAttr.setHidden(true));
	CHECK_MSTATUS(lAttr.setDefault(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		true, true, false, 1.0f, 1.0f, NULL));


	// Next we will add the attributes we have defined to the node
	//
	CHECK_MSTATUS(addAttribute(aTranslucenceCoeff));
	CHECK_MSTATUS(addAttribute(aDiffuseReflectivity));
	CHECK_MSTATUS(addAttribute(aColor));
	CHECK_MSTATUS(addAttribute(aIncandescence));
	CHECK_MSTATUS(addAttribute(aInTransparency));
	CHECK_MSTATUS(addAttribute(outColor));
	CHECK_MSTATUS(addAttribute(aOutTransparency));
	CHECK_MSTATUS(addAttribute(aNormalCamera));

	// Only add the parent of the compound
	CHECK_MSTATUS(addAttribute(aLightData));


	CHECK_MSTATUS(attributeAffects(layerTexture, outColor));
	CHECK_MSTATUS(attributeAffects(layerWeight, outColor));

	CHECK_MSTATUS(attributeAffects(aTranslucenceCoeff, outColor));
	CHECK_MSTATUS(attributeAffects(aDiffuseReflectivity, outColor));
	CHECK_MSTATUS(attributeAffects(aColor, outColor));
	CHECK_MSTATUS(attributeAffects(aInTransparency, aOutTransparency));
	CHECK_MSTATUS(attributeAffects(aInTransparency, outColor));
	CHECK_MSTATUS(attributeAffects(aIncandescence, outColor));
	CHECK_MSTATUS(attributeAffects(aLightIntensityR, outColor));
	CHECK_MSTATUS(attributeAffects(aLightIntensityB, outColor));
	CHECK_MSTATUS(attributeAffects(aLightIntensityG, outColor));
	CHECK_MSTATUS(attributeAffects(aLightIntensity, outColor));
	CHECK_MSTATUS(attributeAffects(aNormalCameraX, outColor));
	CHECK_MSTATUS(attributeAffects(aNormalCameraY, outColor));
	CHECK_MSTATUS(attributeAffects(aNormalCameraZ, outColor));
	CHECK_MSTATUS(attributeAffects(aNormalCamera, outColor));
	CHECK_MSTATUS(attributeAffects(aLightDirectionX, outColor));
	CHECK_MSTATUS(attributeAffects(aLightDirectionY, outColor));
	CHECK_MSTATUS(attributeAffects(aLightDirectionZ, outColor));
	CHECK_MSTATUS(attributeAffects(aLightDirection, outColor));
	CHECK_MSTATUS(attributeAffects(aLightAmbient, outColor));
	CHECK_MSTATUS(attributeAffects(aLightSpecular, outColor));
	CHECK_MSTATUS(attributeAffects(aLightDiffuse, outColor));
	CHECK_MSTATUS(attributeAffects(aLightShadowFraction, outColor));
	CHECK_MSTATUS(attributeAffects(aPreShadowIntensity, outColor));
	CHECK_MSTATUS(attributeAffects(aLightBlindData, outColor));
	CHECK_MSTATUS(attributeAffects(aLightData, outColor));


    return( MS::kSuccess );
}


// Compute takes two parameters: plug and data.
// - Plug is the the data value that needs to be recomputed
// - Data provides handles to all of the nodes attributes, only these
//   handles should be used when performing computations.
//
MStatus Layered::compute( const MPlug& plug, MDataBlock& block )
{
	// The plug parameter will allow us to determine which output attribute
	// needs to be calculated.
	//
	if (plug == outColor || plug == aOutTransparency || plug.parent() == outColor || plug.parent() == aOutTransparency)
	{
		MStatus status;
		MFloatVector resultColor(0.0, 0.0, 0.0);

		// Get surface shading parameters from input block
		//
		MFloatVector& surfaceNormal = block.inputValue(aNormalCamera, &status).asFloatVector();
		CHECK_MSTATUS(status);

		MFloatVector& surfaceColor = block.inputValue(aColor, &status).asFloatVector();
		CHECK_MSTATUS(status);

		MFloatVector& incandescence = block.inputValue(aIncandescence, &status).asFloatVector();
		CHECK_MSTATUS(status);

		float diffuseReflectivity = block.inputValue(aDiffuseReflectivity, &status).asFloat();
		CHECK_MSTATUS(status);

		//      float translucenceCoeff = block.inputValue( aTranslucenceCoeff,
		//              &status ).asFloat();
		//      CHECK_MSTATUS( status );


		// Get light list
		//
		MArrayDataHandle lightData = block.inputArrayValue(aLightData, &status);
		CHECK_MSTATUS(status);

		int numLights = lightData.elementCount(&status);
		CHECK_MSTATUS(status);


		// Calculate the effect of the lights in the scene on the color
		//

		// Iterate through light list and get ambient/diffuse values
		//
		for (int count = 1; count <= numLights; count++)
		{
			// Get the current light out of the array
			//
			MDataHandle currentLight = lightData.inputValue(&status);
			CHECK_MSTATUS(status);


			// Get the intensity of that light
			//
			MFloatVector& lightIntensity = currentLight.child(aLightIntensity).asFloatVector();


			// Find ambient component
			//
			if (currentLight.child(aLightAmbient).asBool())
			{
				resultColor += lightIntensity;
			}


			// Find diffuse component
			//
			if (currentLight.child(aLightDiffuse).asBool())
			{
				MFloatVector& lightDirection = currentLight.child(aLightDirection).asFloatVector();
				float cosln = lightDirection * surfaceNormal;

				if (cosln > 0.0f)
				{
					resultColor += lightIntensity * (cosln * diffuseReflectivity);
				}
			}


			// Advance to the next light.
			//
			if (count < numLights) {
				status = lightData.next();
				CHECK_MSTATUS(status);
			}
		}


		// Factor incident light with surface color and add incandescence
		//
		resultColor[0] = resultColor[0] * surfaceColor[0] + incandescence[0];
		resultColor[1] = resultColor[1] * surfaceColor[1] + incandescence[1];
		resultColor[2] = resultColor[2] * surfaceColor[2] + incandescence[2];


		// Set ouput color attribute
		//
		if (plug == outColor || plug.parent() == outColor)
		{
			// Get the handle to the attribute
			//
			MDataHandle outColorHandle = block.outputValue(outColor, &status);
			CHECK_MSTATUS(status);
			MFloatVector& outColor = outColorHandle.asFloatVector();

			outColor = resultColor;     // Set the output value
			outColorHandle.setClean(); // Mark the output value as clean
		}


		// Set ouput transparency
		//
		if (plug == aOutTransparency || plug.parent() == aOutTransparency)
		{
			MFloatVector& transparency = block.inputValue(aInTransparency, &status).asFloatVector();
			CHECK_MSTATUS(status);


			// Get the handle to the attribute
			//
			MDataHandle outTransHandle = block.outputValue(aOutTransparency, &status);
			CHECK_MSTATUS(status);
			MFloatVector& outTrans = outTransHandle.asFloatVector();

			outTrans = transparency;   // Set the output value
			outTransHandle.setClean(); // Mark the output value as clean
		}
	}
	else
	{
		return(MS::kUnknownParameter); // We got an unexpected plug
	}

	return(MS::kSuccess);
}


