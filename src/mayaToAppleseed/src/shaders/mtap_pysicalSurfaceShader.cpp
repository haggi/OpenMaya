#include "mtap_physicalSurfaceShader.h"

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


MTypeId mtap_surfaceShader::id( 0x0011CF46 );


// the postConstructor() function is called immediately after the objects
// constructor. It is not safe to call MPxNode member functions from the
// constructor, instead they should be called here.
//
void mtap_surfaceShader::postConstructor( )
{
	// setMPSafe indicates that this shader can be used for multiprocessor
	// rendering. For a shading node to be MP safe, it cannot access any
	// shared global data and should only use attributes in the datablock
	// to get input data and store output data.
	//
	setMPSafe( true );
}


///////////////////////////////////////////////////////
// DESCRIPTION: attribute information
///////////////////////////////////////////////////////
//
MObject  mtap_surfaceShader::aDiffuseReflectivity;
MObject  mtap_surfaceShader::aInTransparency;
MObject  mtap_surfaceShader::aInTransR;
MObject  mtap_surfaceShader::aInTransG;
MObject  mtap_surfaceShader::aInTransB;
MObject  mtap_surfaceShader::aColor;
MObject  mtap_surfaceShader::aColorR;
MObject  mtap_surfaceShader::aColorG;
MObject  mtap_surfaceShader::aColorB;
MObject  mtap_surfaceShader::aIncandescence;
MObject  mtap_surfaceShader::aIncandescenceR;
MObject  mtap_surfaceShader::aIncandescenceG;
MObject  mtap_surfaceShader::aIncandescenceB;
MObject  mtap_surfaceShader::aOutColor;
MObject  mtap_surfaceShader::aOutColorR;
MObject  mtap_surfaceShader::aOutColorG;
MObject  mtap_surfaceShader::aOutColorB;
MObject  mtap_surfaceShader::aOutTransparency;
MObject  mtap_surfaceShader::aOutTransR;
MObject  mtap_surfaceShader::aOutTransG;
MObject  mtap_surfaceShader::aOutTransB;
MObject  mtap_surfaceShader::aNormalCamera;
MObject  mtap_surfaceShader::aNormalCameraX;
MObject  mtap_surfaceShader::aNormalCameraY;
MObject  mtap_surfaceShader::aNormalCameraZ;
MObject  mtap_surfaceShader::aLightData;
MObject  mtap_surfaceShader::aLightDirection;
MObject  mtap_surfaceShader::aLightDirectionX;
MObject  mtap_surfaceShader::aLightDirectionY;
MObject  mtap_surfaceShader::aLightDirectionZ;
MObject  mtap_surfaceShader::aLightIntensity; 
MObject  mtap_surfaceShader::aLightIntensityR;
MObject  mtap_surfaceShader::aLightIntensityG;
MObject  mtap_surfaceShader::aLightIntensityB;
MObject  mtap_surfaceShader::aLightAmbient;
MObject  mtap_surfaceShader::aLightDiffuse;
MObject  mtap_surfaceShader::aLightSpecular;
MObject  mtap_surfaceShader::aLightShadowFraction;
MObject  mtap_surfaceShader::aPreShadowIntensity;
MObject  mtap_surfaceShader::aLightBlindData;

MObject  mtap_surfaceShader::bsdf;
MObject  mtap_surfaceShader::matteReflectance;
MObject  mtap_surfaceShader::matteReflectanceMultiplier;
MObject  mtap_surfaceShader::specularReflectance;
MObject  mtap_surfaceShader::specularReflectanceMultiplier;
MObject  mtap_surfaceShader::roughness;
MObject  mtap_surfaceShader::transmittance;
MObject  mtap_surfaceShader::transmittanceMultiplier;
MObject  mtap_surfaceShader::fromIor;
MObject  mtap_surfaceShader::toIor;
MObject  mtap_surfaceShader::shininess_u;
MObject  mtap_surfaceShader::shininess_v;
MObject  mtap_surfaceShader::fresnelMultiplier;

MObject  mtap_surfaceShader::exitance;
MObject  mtap_surfaceShader::emitLight;
MObject  mtap_surfaceShader::mdf;
MObject  mtap_surfaceShader::mdf_parameter;


mtap_surfaceShader::mtap_surfaceShader() { }
mtap_surfaceShader::~mtap_surfaceShader() { }


void* mtap_surfaceShader::creator()
{
	return new mtap_surfaceShader();
}


// The initialize method is called only once when the node is first
// registered with Maya. In this method you define the attributes of the
// node, what data comes in and goes out of the node that other nodes may
// want to connect to. 
//

MStatus mtap_surfaceShader::initialize()
{
	MFnNumericAttribute nAttr; 
	MFnLightDataAttribute lAttr;
	MFnEnumAttribute eAttr;

	MStatus status; 
	MStatus stat; 

	// Input Attributes
	//

	bsdf = eAttr.create( "bsdf", "bsdf", 1, &stat);
	stat = eAttr.addField( "Lambert", 0 );
	stat = eAttr.addField( "Ashikhmin", 1 );
	stat = eAttr.addField( "Kelemen", 2 );
	stat = eAttr.addField( "Specular", 3 );
	stat = eAttr.addField( "Phong", 4 );
	stat = eAttr.addField( "Microfacet", 5 );
	CHECK_MSTATUS(addAttribute( bsdf ));

	mdf = eAttr.create( "mdf", "mdf", 1, &stat);
	stat = eAttr.addField( "Blinn", 0 );
	stat = eAttr.addField( "Beckmann", 1 );
	stat = eAttr.addField( "Ward", 2 );
	stat = eAttr.addField( "GGX", 3 );
	CHECK_MSTATUS(addAttribute( mdf ));

	mdf_parameter = nAttr.create("mdf_parameter", "mdf_parameter", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( mdf_parameter ));	

	matteReflectance = nAttr.createColor("matte_reflectance", "matte_reflectance");
	nAttr.setDefault(0.8, 0.8, 0.8);
	CHECK_MSTATUS(addAttribute( matteReflectance ));
	
	specularReflectance = nAttr.createColor("specular_reflectance", "specular_reflectance");
	nAttr.setDefault(1.0, 1.0, 1.0);
	CHECK_MSTATUS(addAttribute( specularReflectance ));	
	
	matteReflectanceMultiplier = nAttr.create("matte_reflectance_multiplier", "matte_reflectance_multiplier", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( matteReflectanceMultiplier ));	
	
	specularReflectanceMultiplier = nAttr.create("specular_reflectance_multiplier", "specular_reflectance_multiplier", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( specularReflectanceMultiplier ));	
	
	roughness = nAttr.create("roughness", "roughness", MFnNumericData::kFloat, 0.3f);
	CHECK_MSTATUS(addAttribute( roughness ));	
	
	transmittance = nAttr.createColor("transmittance", "transmittance");
	CHECK_MSTATUS(addAttribute( transmittance ));	
	
	transmittanceMultiplier = nAttr.create("transmittance_multiplier", "transmittance_multiplier", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( transmittanceMultiplier ));	
	
	fromIor = nAttr.create("from_ior", "from_ior", MFnNumericData::kFloat, 1.1f);
	CHECK_MSTATUS(addAttribute( fromIor ));	
	
	toIor = nAttr.create("to_ior", "to_ior", MFnNumericData::kFloat, 1.1f);
	CHECK_MSTATUS(addAttribute( toIor ));	
	
	shininess_u = nAttr.create("shininess_u", "shininess_u", MFnNumericData::kFloat, 1000.0f);
	CHECK_MSTATUS(addAttribute( shininess_u ));	
	
	shininess_v = nAttr.create("shininess_v", "shininess_v", MFnNumericData::kFloat, 1000.0f);
	CHECK_MSTATUS(addAttribute( shininess_v ));	

	fresnelMultiplier = nAttr.create("fresnelMultiplier", "fresnelMultiplier", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( fresnelMultiplier ));	

	emitLight = nAttr.create("emitLight", "emitLight", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( emitLight ));	

	exitance = nAttr.createColor("exitance", "exitance");
	nAttr.setDefault(1.0, 1.0, 1.0);
	CHECK_MSTATUS(addAttribute( exitance ));	

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

	// Color Output
	//
	aOutColorR = nAttr.create( "outColorR", "ocr", MFnNumericData::kFloat,
			0, &status );
	CHECK_MSTATUS( status );
	
	aOutColorG = nAttr.create( "outColorG", "ocg", MFnNumericData::kFloat,
			0, &status );
	CHECK_MSTATUS( status );
	
	aOutColorB = nAttr.create( "outColorB", "ocb", MFnNumericData::kFloat,
			0, &status );
	CHECK_MSTATUS( status );
	
	aOutColor = nAttr.create( "outColor", "oc", aOutColorR, aOutColorG,
			aOutColorB, &status );
	CHECK_MSTATUS( status );
	
	CHECK_MSTATUS( nAttr.setHidden( false ) );
	CHECK_MSTATUS( nAttr.setReadable( true ) );
	CHECK_MSTATUS( nAttr.setWritable( false ) );

	
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
	CHECK_MSTATUS( addAttribute( aOutColor ) );
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

	CHECK_MSTATUS( attributeAffects( matteReflectance, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( specularReflectance, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( matteReflectanceMultiplier, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( specularReflectanceMultiplier, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( roughness, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( transmittance, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( transmittanceMultiplier, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( fromIor, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( toIor, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( shininess_u, aOutColor ) );
	CHECK_MSTATUS( attributeAffects( shininess_v, aOutColor ) );

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
MStatus mtap_surfaceShader::compute( const MPlug& plug, MDataBlock& block ) 
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


		MFloatVector& matte_reflectance = block.inputValue( matteReflectance, &status ).asFloatVector();
		CHECK_MSTATUS( status );
		float& matte_reflectance_multiplier = block.inputValue( matteReflectanceMultiplier, &status ).asFloat();
		CHECK_MSTATUS( status );
		MFloatVector& specular_reflectance = block.inputValue( specularReflectance, &status ).asFloatVector();
		CHECK_MSTATUS( status );
		float& specular_reflectance_multiplier = block.inputValue( specularReflectanceMultiplier, &status ).asFloat();
		CHECK_MSTATUS( status );

		// Get surface shading parameters from input block
		//
		MFloatVector& surfaceNormal = block.inputValue( aNormalCamera,
				&status ).asFloatVector();
		CHECK_MSTATUS( status );

		MFloatVector& surfaceColor = block.inputValue( aColor,
				&status ).asFloatVector();
		CHECK_MSTATUS( status );
		surfaceColor = matte_reflectance;

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


// These methods load and unload the plugin, registerNode registers the
// new node type with maya
//

//MStatus initializePlugin( MObject obj )
//{ 
//	const MString	UserClassify( "shader/surface" );
//
//	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
//
//	MFnPlugin plugin( obj, PLUGIN_COMPANY, "4.5", "Any" );
//
//	CHECK_MSTATUS( plugin.registerNode( "mtap_surfaceShaderShader", mtap_surfaceShader::id, 
//			mtap_surfaceShader::creator, mtap_surfaceShader::initialize, MPxNode::kDependNode,
//			&UserClassify ) );
//
//   command += UserClassify;
//   
//   command += "\");}\n";
//
//   CHECK_MSTATUS( MGlobal::executeCommand( command ) );
//
//   return( MS::kSuccess );
//}

//MStatus uninitializePlugin( MObject obj )
//{
//	const MString UserClassify( "shader/surface" );
//
//	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
//
//	MFnPlugin plugin( obj );
//   
//	CHECK_MSTATUS( plugin.deregisterNode( mtap_surfaceShader::id ) );
//
//	command += UserClassify;
//	command += "\");}\n";
//
//	CHECK_MSTATUS( MGlobal::executeCommand( command ) );
//
//	return MS::kSuccess;
//}
