#include "mtap_constantShader.h"

#include <maya/MIOStream.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MArrayDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h> 
#include <maya/MFnEnumAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>


MTypeId mtap_constantShader::id( 0x00106EF9 );

void mtap_constantShader::postConstructor( )
{
	setMPSafe( true );
}


///////////////////////////////////////////////////////
// DESCRIPTION: attribute information
///////////////////////////////////////////////////////
//
MObject  mtap_constantShader::aDiffuseReflectivity;
MObject  mtap_constantShader::aInTransparency;
MObject  mtap_constantShader::aInTransR;
MObject  mtap_constantShader::aInTransG;
MObject  mtap_constantShader::aInTransB;
MObject  mtap_constantShader::aColor;
MObject  mtap_constantShader::aColorR;
MObject  mtap_constantShader::aColorG;
MObject  mtap_constantShader::aColorB;
MObject  mtap_constantShader::aIncandescence;
MObject  mtap_constantShader::aIncandescenceR;
MObject  mtap_constantShader::aIncandescenceG;
MObject  mtap_constantShader::aIncandescenceB;
MObject  mtap_constantShader::aOutColor;
MObject  mtap_constantShader::aOutColorR;
MObject  mtap_constantShader::aOutColorG;
MObject  mtap_constantShader::aOutColorB;
MObject  mtap_constantShader::aOutTransparency;
MObject  mtap_constantShader::aOutTransR;
MObject  mtap_constantShader::aOutTransG;
MObject  mtap_constantShader::aOutTransB;
MObject  mtap_constantShader::aNormalCamera;
MObject  mtap_constantShader::aNormalCameraX;
MObject  mtap_constantShader::aNormalCameraY;
MObject  mtap_constantShader::aNormalCameraZ;
MObject  mtap_constantShader::aLightData;
MObject  mtap_constantShader::aLightDirection;
MObject  mtap_constantShader::aLightDirectionX;
MObject  mtap_constantShader::aLightDirectionY;
MObject  mtap_constantShader::aLightDirectionZ;
MObject  mtap_constantShader::aLightIntensity; 
MObject  mtap_constantShader::aLightIntensityR;
MObject  mtap_constantShader::aLightIntensityG;
MObject  mtap_constantShader::aLightIntensityB;
MObject  mtap_constantShader::aLightAmbient;
MObject  mtap_constantShader::aLightDiffuse;
MObject  mtap_constantShader::aLightSpecular;
MObject  mtap_constantShader::aLightShadowFraction;
MObject  mtap_constantShader::aPreShadowIntensity;
MObject  mtap_constantShader::aLightBlindData;

MObject  mtap_constantShader::color;


mtap_constantShader::mtap_constantShader() { }
mtap_constantShader::~mtap_constantShader() { }


void* mtap_constantShader::creator()
{
	return new mtap_constantShader();
}

MStatus mtap_constantShader::initialize()
{
	MFnNumericAttribute nAttr; 
	MFnLightDataAttribute lAttr;
	MFnEnumAttribute eAttr;

	MStatus status; 
	MStatus stat; 

	aOutColor = nAttr.createColor( "outColor", "oc", &status );
	CHECK_MSTATUS( status );	
	CHECK_MSTATUS( nAttr.setHidden( false ) );
	CHECK_MSTATUS( nAttr.setReadable( true ) );
	CHECK_MSTATUS( nAttr.setWritable( false ) );
	CHECK_MSTATUS(addAttribute( aOutColor ));

	color = nAttr.createColor( "ccolor", "ccolor", &status );
	CHECK_MSTATUS(addAttribute( color ));
	CHECK_MSTATUS( attributeAffects( color, aOutColor ) );

	// -------------- maya ---------------

	aDiffuseReflectivity = nAttr.create( "diffuseReflectivity", "drfl",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.8f ) );

	aColorR = nAttr.create( "colorR", "cr",MFnNumericData::kFloat, 0,
			&status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f ) );

	aColorG = nAttr.create( "colorG", "cg", MFnNumericData::kFloat, 0,
			&status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.58824f ) );

	aColorB = nAttr.create( "colorB", "cb",MFnNumericData::kFloat, 0,
			&status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.644f ) );

	aColor = nAttr.create( "color", "c", aColorR, aColorG, aColorB,
			&status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f, 0.58824f, 0.644f ) );
	CHECK_MSTATUS( nAttr.setUsedAsColor( true ) );

	aIncandescenceR = nAttr.create( "incandescenceR", "ir",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f ) );

	aIncandescenceG = nAttr.create( "incandescenceG", "ig",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f ) );

	aIncandescenceB = nAttr.create( "incandescenceB", "ib",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status);
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f ) );

	aIncandescence = nAttr.create( "incandescence", "ic", aIncandescenceR,
			aIncandescenceG, aIncandescenceB, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f, 0.0f, 0.0f ) );
	CHECK_MSTATUS( nAttr.setUsedAsColor( true ) );

	aInTransR = nAttr.create( "transparencyR", "itr",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );

	aInTransG = nAttr.create( "transparencyG", "itg",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );

	aInTransB = nAttr.create( "transparencyB", "itb",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status);
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );

	aInTransparency = nAttr.create( "transparency", "it", aInTransR,
			aInTransG, aInTransB, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setKeyable( true ) );
	CHECK_MSTATUS( nAttr.setStorable( true ) );
	CHECK_MSTATUS( nAttr.setDefault( 0.0f, 0.0f, 0.0f ) );
	CHECK_MSTATUS( nAttr.setUsedAsColor( true ) );

	
	// Output Attributes
	//

	
	// Transparency Output
	//
	aOutTransR = nAttr.create( "outTransparencyR", "otr",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );

	aOutTransG = nAttr.create( "outTransparencyG", "otg",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );

	aOutTransB = nAttr.create( "outTransparencyB", "otb",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );

	aOutTransparency = nAttr.create( "outTransparency", "ot",
			aOutTransR,aOutTransG,aOutTransB, &status );
	CHECK_MSTATUS( status );

	CHECK_MSTATUS( nAttr.setHidden( false ) );
	CHECK_MSTATUS( nAttr.setReadable( true ) );
	CHECK_MSTATUS( nAttr.setWritable( false ) );


	// Camera Normals
	//
	aNormalCameraX = nAttr.create( "normalCameraX", "nx",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setStorable( false ) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aNormalCameraY = nAttr.create( "normalCameraY", "ny",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setStorable( false ) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aNormalCameraZ = nAttr.create( "normalCameraZ", "nz",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setStorable( false ) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aNormalCamera = nAttr.create( "normalCamera","n", aNormalCameraX,
			aNormalCameraY, aNormalCameraZ, &status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( nAttr.setStorable( false ) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );
	CHECK_MSTATUS( nAttr.setHidden( true ) );


	// Light Direction
	//
	aLightDirectionX = nAttr.create( "lightDirectionX", "ldx",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightDirectionY = nAttr.create( "lightDirectionY", "ldy",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightDirectionZ = nAttr.create( "lightDirectionZ", "ldz",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightDirection = nAttr.create( "lightDirection", "ld",
			aLightDirectionX, aLightDirectionY, aLightDirectionZ,
			&status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );


	// Light Intensity
	//
	aLightIntensityR = nAttr.create( "lightIntensityR", "lir",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightIntensityG = nAttr.create( "lightIntensityG", "lig",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightIntensityB = nAttr.create( "lightIntensityB", "lib",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightIntensity = nAttr.create( "lightIntensity", "li", 
			aLightIntensityR, aLightIntensityG, aLightIntensityB,
			&status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f, 1.0f, 1.0f ) );

   
	// Light
	//
	aLightAmbient = nAttr.create( "lightAmbient", "la",
			MFnNumericData::kBoolean, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( true ) );

	aLightDiffuse = nAttr.create( "lightDiffuse", "ldf",
			MFnNumericData::kBoolean, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( true ) );

	aLightSpecular = nAttr.create( "lightSpecular", "ls",
			MFnNumericData::kBoolean, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( false ) );

	aLightShadowFraction = nAttr.create( "lightShadowFraction", "lsf",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aPreShadowIntensity = nAttr.create( "preShadowIntensity", "psi",
			MFnNumericData::kFloat, 0, &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
	CHECK_MSTATUS( nAttr.setDefault( 1.0f ) );

	aLightBlindData = nAttr.createAddr( "lightBlindData", "lbld",
            &status );
	CHECK_MSTATUS( status );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );

	aLightData = lAttr.create( "lightDataArray", "ltd", aLightDirection,
			aLightIntensity, aLightAmbient, aLightDiffuse, aLightSpecular, 
			aLightShadowFraction, aPreShadowIntensity, aLightBlindData,
			&status );
	CHECK_MSTATUS( status );
	CHECK_MSTATUS( lAttr.setArray( true ) );
	CHECK_MSTATUS( lAttr.setStorable( false ) );
	CHECK_MSTATUS( lAttr.setHidden( true ) );
	CHECK_MSTATUS( lAttr.setDefault( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			true, true, false, 1.0f, 1.0f, NULL ) );


	// Next we will add the attributes we have defined to the node
	//
	CHECK_MSTATUS( addAttribute( aDiffuseReflectivity ) );
	CHECK_MSTATUS( addAttribute( aColor ) );
	CHECK_MSTATUS( addAttribute( aIncandescence ) );
	CHECK_MSTATUS( addAttribute( aInTransparency ) );
	CHECK_MSTATUS( addAttribute( aOutTransparency ) );
	CHECK_MSTATUS( addAttribute( aNormalCamera ) );

	// Only add the parent of the compound
	CHECK_MSTATUS( addAttribute( aLightData ) );

	// The attributeAffects() method is used to indicate when the input
	// attribute affects the output attribute. This knowledge allows Maya
	// to optimize dependencies in the graph in more complex nodes where
	// there may be several inputs and outputs, but not all the inputs
	// affect all the outputs.
	//

	CHECK_MSTATUS( attributeAffects( aDiffuseReflectivity, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aColorR, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aColorG, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aColorB, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aColor, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aInTransR, aOutTransparency ) );
	CHECK_MSTATUS( attributeAffects( aInTransG, aOutTransparency ) );
	CHECK_MSTATUS( attributeAffects( aInTransB, aOutTransparency ) );
	CHECK_MSTATUS( attributeAffects( aInTransparency, aOutTransparency ) );
	CHECK_MSTATUS( attributeAffects( aInTransparency, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aIncandescenceR, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aIncandescenceG, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aIncandescenceB, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aIncandescence, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightIntensityR, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightIntensityB, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightIntensityG, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightIntensity, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aNormalCameraX, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aNormalCameraY, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aNormalCameraZ, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aNormalCamera, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightDirectionX, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightDirectionY, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightDirectionZ, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightDirection, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightAmbient, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightSpecular, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightDiffuse, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightShadowFraction, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aPreShadowIntensity, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightBlindData, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( aLightData, aOutColor ) );

	return( MS::kSuccess );
}


// The compute() method does the actual work of the node using the inputs
// of the node to generate its output. 
//
// Compute takes two parameters: plug and data.
// - Plug is the the data value that needs to be recomputed
// - Data provides handles to all of the nodes attributes, only these
//   handles should be used when performing computations.
//
MStatus mtap_constantShader::compute( const MPlug& plug, MDataBlock& block ) 
{ 
	// The plug parameter will allow us to determine which output attribute
	// needs to be calculated.
	//
	if( plug == aOutColor
			|| plug == aOutColorR
			|| plug == aOutColorG
			|| plug == aOutColorB
			|| plug == aOutTransparency
			|| plug == aOutTransR
			|| plug == aOutTransG
			|| plug == aOutTransB )
	{
		MStatus status;
		MFloatVector resultColor( 0.0, 0.0, 0.0 ); 

		// Get surface shading parameters from input block
		//
		MFloatVector& surfaceNormal = block.inputValue( aNormalCamera,
				&status ).asFloatVector();
		CHECK_MSTATUS( status );

		MFloatVector& surfaceColor = block.inputValue( aColor,
				&status ).asFloatVector();
		CHECK_MSTATUS( status );
		surfaceColor.x = surfaceColor.y = surfaceColor.z = 1.0f;

		MFloatVector& incandescence = block.inputValue( aIncandescence,
				&status ).asFloatVector();
		CHECK_MSTATUS( status );

		float diffuseReflectivity = block.inputValue(
				aDiffuseReflectivity, &status ).asFloat();
		CHECK_MSTATUS( status );
	  

		// Get light list
		//
		MArrayDataHandle lightData = block.inputArrayValue( aLightData,
				&status );
		CHECK_MSTATUS( status );

		int numLights = lightData.elementCount( &status );
		CHECK_MSTATUS( status );


		// Calculate the effect of the lights in the scene on the color
		//

		// Iterate through light list and get ambient/diffuse values
		//
		for( int count=1; count <= numLights; count++ )
		{
			// Get the current light out of the array
			//
			MDataHandle currentLight = lightData.inputValue( &status ); 
			CHECK_MSTATUS( status );


			// Get the intensity of that light
			//
			MFloatVector& lightIntensity = currentLight.child(
					aLightIntensity ).asFloatVector();

	 
			// Find ambient component
			//
			if ( currentLight.child( aLightAmbient ).asBool() )
			{
				resultColor += lightIntensity;
			}


			// Find diffuse component
			//
			if ( currentLight.child( aLightDiffuse ).asBool() )
			{
				MFloatVector& lightDirection = currentLight.child(
						aLightDirection ).asFloatVector();
				float cosln = lightDirection * surfaceNormal;
   
			   if ( cosln > 0.0f ) {
					resultColor += lightIntensity
							* ( cosln * diffuseReflectivity );
			   }
			}


			// Advance to the next light.
			//
			if ( count < numLights ) {
				status = lightData.next();
				CHECK_MSTATUS( status );
			}
		}


		// Factor incident light with surface color and add incandescence
		//
		resultColor[0] = resultColor[0] * surfaceColor[0] + incandescence[0];
		resultColor[1] = resultColor[1] * surfaceColor[1] + incandescence[1];
		resultColor[2] = resultColor[2] * surfaceColor[2] + incandescence[2];


		// Set ouput color attribute
		//
		if ( plug == aOutColor || plug == aOutColorR || plug == aOutColorG
				|| plug == aOutColorB)
		{
			// Get the handle to the attribute
			//
			MDataHandle outColorHandle = block.outputValue( aOutColor,
					&status );
			CHECK_MSTATUS( status );
			MFloatVector& outColor = outColorHandle.asFloatVector();
		
			outColor = resultColor;		// Set the output value
			outColorHandle.setClean(); // Mark the output value as clean
		}


		// Set ouput transparency
		//
		if ( plug == aOutTransparency || plug == aOutTransR
				|| plug == aOutTransG || plug == aOutTransB )
		{
			MFloatVector& transparency = block.inputValue(
					aInTransparency, &status ).asFloatVector();
			CHECK_MSTATUS( status );


			// Get the handle to the attribute
			//
			MDataHandle outTransHandle = block.outputValue(
					aOutTransparency, &status );
			CHECK_MSTATUS( status );
			MFloatVector& outTrans = outTransHandle.asFloatVector();
			
			outTrans = transparency;   // Set the output value
			outTransHandle.setClean(); // Mark the output value as clean
		}
	} 
	else
	{
		return( MS::kUnknownParameter ); // We got an unexpected plug
	}

	return( MS::kSuccess );
}
