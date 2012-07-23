#include <maya/MFnStringData.h>
#include <maya/MFnDependencyNode.h>
#include "mtm_envlight.h"

MTypeId mtmEnvLight::id( 0x106EF1 );

// RTB
	//ray_transform 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1
	//ray_property object name "/obj/envlight1"
	//ray_property object samplingquality 1.37999999523
	//ray_property object surface opdef:/Shop/v_arealight lightcolor 0.85598 0.883581 1.27 envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//ray_property light areashape "env"
	//ray_property light areamap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//ray_property light areafullsphere 1
	//ray_property light envintensity 0.855980018635 0.883581288896 1.26999998093
	//ray_property light raybackground 1
	//ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1
	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter shadowI 1
	//ray_property light projection "perspective"
	//ray_property light zoom 1.20710550585 1.20710550585
 //   ray_end	# }

// DIRECT
	//ray_property object name "/obj/envlight1"
	//ray_property object surface opdef:/Shop/v_arealight lightcolor 1.2345 0 0 envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//ray_property light areashape "env"
	//ray_property light areamap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//ray_property light areafullsphere 1
	//ray_property light envintensity 1.23450005054 0 0
	//ray_property light raybackground 0
	//ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1
	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter shadowI 1
	//ray_property light projection "perspective"
	//ray_property light zoom 1.20710550585 1.20710550585
 //   ray_end	# }



// AO
	//ray_transform 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1
	//ray_property object name "/obj/envlight1"
	//ray_property object surface opdef:/Shop/v_arealight lightcolor 1 1 1
	//ray_property light envintensity 1 1 1
	//ray_property light raybackground 0
	//ray_property light shader opdef:/Shop/v_gilight light_color 1 1 1 istyle "opacity" doraysamples 1 doadaptive 0 domaxdist 0 maxdist 10 cone_angle 90 objmask "*" background 1 1 1
	//ray_property light projection "perspective"
	//ray_property light zoom 1.20710550585 1.20710550585


MObject  mtmEnvLight::samplingquality;
MObject  mtmEnvLight::envmap;
MObject  mtmEnvLight::areamap;
MObject  mtmEnvLight::areafullsphere;
MObject  mtmEnvLight::envintensity;
MObject  mtmEnvLight::raybackground;
MObject  mtmEnvLight::castshadow;	
MObject  mtmEnvLight::envtype;	
MObject  mtmEnvLight::doraysamples;	
MObject  mtmEnvLight::doadaptive;	
MObject  mtmEnvLight::domaxdist;	
MObject  mtmEnvLight::maxdist;	
MObject  mtmEnvLight::coneangle;	
MObject  mtmEnvLight::envtint;	
MObject  mtmEnvLight::shadowI;		
MObject  mtmEnvLight::samples;		
MObject  mtmEnvLight::objectmask;		
MObject  mtmEnvLight::usePortalGeometry;		
MObject  mtmEnvLight::portalGeometry;		

//MObject  mtmEnvLight::aColor;
MObject  mtmEnvLight::aLightColor;
MObject  mtmEnvLight::aShadowColor;
MObject  mtmEnvLight::aPosition;
MObject  mtmEnvLight::aInputDirection;
MObject  mtmEnvLight::aInputAmbient;
MObject  mtmEnvLight::aInputDiffuse;
MObject  mtmEnvLight::aInputSpecular;
MObject  mtmEnvLight::aIntensity; 

MObject  mtmEnvLight::aLightData;
MObject  mtmEnvLight::aLightDirection;
MObject  mtmEnvLight::aLightIntensity;
MObject  mtmEnvLight::aLightAmbient;
MObject  mtmEnvLight::aLightDiffuse;
MObject  mtmEnvLight::aLightSpecular;
MObject  mtmEnvLight::aLightShadowFraction;
MObject  mtmEnvLight::aPreShadowIntensity;
MObject  mtmEnvLight::aLightBlindData;

static float mtm[][3] = {{0.0f, 0.0f, 0.0f},
						 {0.1f, 0.0f, 0.0f},
						 {0.1f, 0.0f, 0.2f},
						 {0.2f, 0.0f, 0.1f},
						 {0.3f, 0.0f, 0.2f},
						 {0.3f, 0.0f, 0.0f},
						 {0.4f, 0.0f, 0.0f},
						 {0.4f, 0.0f, 0.2f},
						 {0.5f, 0.0f, 0.1f},
						 {0.6f, 0.0f, 0.2f},
						 {0.6f, 0.0f, 0.0f},
						 {0.7f, 0.0f, 0.0f},
						 {0.7f, 0.0f, 0.3f},
						 {0.6f, 0.0f, 0.3f},
						 {0.5f, 0.0f, 0.2f},
						 {0.4f, 0.0f, 0.3f},
						 {0.3f, 0.0f, 0.3f},
						 {0.2f, 0.0f, 0.2f},
						 {0.1f, 0.0f, 0.3f},
						 {0.0f, 0.0f, 0.3f},
						 {0.0f, 0.0f, 0.0f}
						};
static int mtmCount = 21;

void mtmEnvLight::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 
	// Get the size
	//
	//MObject thisNode = thisMObject();
	//MPlug plug( thisNode, size );
	//MDistance sizeVal;
	//plug.getValue( sizeVal );

	//float multiplier = (float) sizeVal.asCentimeters();

	view.beginGL(); 
	float size2 = 0.7f * .5f;
	float scale = 2.0f;

	if ( ( style == M3dView::kFlatShaded ) || 
		 ( style == M3dView::kGouraudShaded ) ) 
	{  
		// Push the color settings
		// 
		glPushAttrib( GL_CURRENT_BIT );

		if ( status == M3dView::kActive ) {
			view.setDrawColor( 13, M3dView::kActiveColors );
		} else {
			view.setDrawColor( 13, M3dView::kDormantColors );
		}  

		glBegin( GL_TRIANGLE_FAN );
	        int i;
			int last = mtmCount - 1;
			for ( i = 0; i < last; ++i ) {
				glVertex3f( (mtm[i][0] - size2) * scale,
							(mtm[i][1]) * scale,
							(mtm[i][2] - .15f) * scale );
			} 
		glEnd();
		glPopAttrib();
	}

	// Draw the outline of the foot
	//
	glBegin( GL_LINES );
	    int i;
	    int last = mtmCount - 1;
	    for ( i = 0; i < last; ++i ) { 
				glVertex3f( (mtm[i][0] - size2) * scale,
							(mtm[i][1]),
							(mtm[i][2] - .15f) * scale );
				glVertex3f( (mtm[i+1][0] - size2) * scale,
							(mtm[i+1][1]),
							(mtm[i+1][2] - .15f) * scale );
		} 
	glEnd();
		

	view.endGL();
}

bool mtmEnvLight::isBounded() const
{ 
	return true;
}

MBoundingBox mtmEnvLight::boundingBox() const
{   
	// Get the size
	//
	//MObject thisNode = thisMObject();
	//MPlug plug( thisNode, size );
	//MDistance sizeVal;
	//plug.getValue( sizeVal );

	//double multiplier = sizeVal.asCentimeters();
 
	MPoint corner1( -0.7, 0.0, -0.3 );
	MPoint corner2( 0.7, 0.0, 0.3 );

	//corner1 = corner1 * multiplier;
	//corner2 = corner2 * multiplier;

	return MBoundingBox( corner1, corner2 );
}


void mtmEnvLight::postConstructor( )
{
	setMPSafe(true);
	MFnDependencyNode nodefn(thisMObject());
	nodefn.setName("mtm_envlightShape#");
}


mtmEnvLight::mtmEnvLight()
{
}


mtmEnvLight::~mtmEnvLight()
{
}

void * mtmEnvLight::creator()
{
    return new mtmEnvLight();
}

MStatus mtmEnvLight::initialize()
{
    MFnTypedAttribute tAttr; 
    MFnNumericAttribute nAttr; 
    MFnLightDataAttribute lAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;
	MStatus stat;

    //aColor = nAttr.createColor( "color", "c" );
    //CHECK_MSTATUS ( nAttr.setKeyable(true) );
    //CHECK_MSTATUS ( nAttr.setStorable(true) );
    //CHECK_MSTATUS ( nAttr.setDefault(0.0f, 0.58824f, 0.644f) );

    aLightColor = nAttr.createColor( "lightColor", "lightColor" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    nAttr.setDefault(0.7f, 0.58824f, 0.344f);

	aShadowColor = nAttr.createColor( "shadowColor", "sc" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setDefault(0.0f, 0.0f, 0.0f) );

    aPosition = nAttr.createPoint( "position", "pos" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );

    aInputDirection = nAttr.createPoint( "inputDirection", "id" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setDefault(-1.0f, 0.0f, 0.0f) );

    aInputAmbient = nAttr.create( "ambientOn", "an", MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

    aInputDiffuse = nAttr.create( "emitDiffuse", "dn", MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

    aInputSpecular = nAttr.create( "emitSpecular", "sn", MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

    aIntensity = nAttr.create( "intensity", "i", MFnNumericData::kFloat);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(1.0f) );

	samplingquality = nAttr.create( "samplingquality", "samplingquality", MFnNumericData::kFloat, 1.0);
	envmap = tAttr.create( "envmap", "envmap", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	areamap = tAttr.create( "areamap", "areamap", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	areafullsphere = nAttr.create( "areafullsphere", "areafullsphere", MFnNumericData::kBoolean, true);
	envintensity = nAttr.createColor( "envintensity", "envintensity" );
	nAttr.setDefault(1.0f, 1.0f, 1.0f);
	raybackground = nAttr.create( "raybackground", "raybackground", MFnNumericData::kBoolean, false);
	castshadow = nAttr.create( "castshadow", "castshadow", MFnNumericData::kBoolean, true);
	envtype = eAttr.create( "envtype", "envtype", 0, &stat);
	stat = eAttr.addField( "Direct Lighting", 0 );
	stat = eAttr.addField( "Ambient Occlusion", 1 );
	stat = eAttr.addField( "Full Irradiance", 2 );
	stat = eAttr.addField( "Raytrace Background", 3 );
	eAttr.setDefault(0);		
	doraysamples = nAttr.create( "doraysamples", "doraysamples", MFnNumericData::kBoolean, false);
	doadaptive = nAttr.create( "doadaptive", "doadaptive", MFnNumericData::kBoolean, false);
	domaxdist = nAttr.create( "domaxdist", "domaxdist", MFnNumericData::kBoolean, false);
	maxdist = nAttr.create( "maxdist", "maxdist", MFnNumericData::kFloat, 10.0);
	coneangle = nAttr.create( "coneangle", "coneangle", MFnNumericData::kFloat, 45.0);
	envtint = nAttr.createColor( "envtint", "envtint" );
	nAttr.setDefault(1.0f, 1.0f, 1.0f);
	shadowI = nAttr.create( "shadowI", "shadowI", MFnNumericData::kFloat, 1.0);
	samples = nAttr.create( "samples", "samples", MFnNumericData::kInt, 32);
	MFnStringData fnStringData;
	MObject defaultObjectMask;
	defaultObjectMask = fnStringData.create( "*" );
	objectmask = tAttr.create( "objectmask", "objectmask", MFnNumericData::kString, defaultObjectMask);
	usePortalGeometry = nAttr.create( "usePortalGeometry", "usePortalGeometry", MFnNumericData::kBoolean, false);
	portalGeometry = mAttr.create( "portalGeometry", "portalGeometry");
	mAttr.setConnectable(true);
	mAttr.accepts(MFnData::kAny);
	
// Outputs

    aLightDirection = nAttr.createPoint( "lightDirection", "ld" );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(-1.0f, 0.0f, 0.0f) );

    aLightIntensity = nAttr.createColor( "lightIntensity", "li" );
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(1.0f, 0.5f, 0.2f) );

    aLightAmbient = nAttr.create( "lightAmbient", "la", 
								  MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    nAttr.setDefault(true);

    aLightDiffuse = nAttr.create( "lightDiffuse", "ldf",
								  MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

    aLightSpecular = nAttr.create( "lightSpecular", "ls", 
								   MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

    aLightShadowFraction = nAttr.create("lightShadowFraction","lsf",
										MFnNumericData::kFloat);
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(0.0f) );

    aPreShadowIntensity = nAttr.create("preShadowIntensity","psi",
									   MFnNumericData::kFloat);
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( nAttr.setDefault(0.0f) );

    aLightBlindData = nAttr.createAddr("lightBlindData","lbld");
    CHECK_MSTATUS ( nAttr.setStorable(false) );
    CHECK_MSTATUS ( nAttr.setHidden(true) );
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );

    aLightData = lAttr.create( "lightData", "ltd", 
                               aLightDirection, aLightIntensity, 
							   aLightAmbient, 
                               aLightDiffuse, aLightSpecular, 
							   aLightShadowFraction,
                               aPreShadowIntensity, aLightBlindData);
    CHECK_MSTATUS ( nAttr.setReadable(true) );
    CHECK_MSTATUS ( nAttr.setWritable(false) );
    CHECK_MSTATUS ( lAttr.setStorable(false) );
    CHECK_MSTATUS ( lAttr.setHidden(true) );
    lAttr.setDefault(-1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.2f, true, true,
					 true, 0.0f, 1.0f, NULL);

    CHECK_MSTATUS ( addAttribute(coneangle) );
    CHECK_MSTATUS ( addAttribute(samples) );
    CHECK_MSTATUS ( addAttribute(objectmask) );
    CHECK_MSTATUS ( addAttribute(shadowI) );
    CHECK_MSTATUS ( addAttribute(envtint) );
    CHECK_MSTATUS ( addAttribute(maxdist) );
    CHECK_MSTATUS ( addAttribute(domaxdist) );
    CHECK_MSTATUS ( addAttribute(doadaptive) );
    CHECK_MSTATUS ( addAttribute(doraysamples) );
    CHECK_MSTATUS ( addAttribute(envtype) );
    CHECK_MSTATUS ( addAttribute(castshadow) );
    CHECK_MSTATUS ( addAttribute(raybackground) );
    CHECK_MSTATUS ( addAttribute(envintensity) );
    CHECK_MSTATUS ( addAttribute(areafullsphere) );
    CHECK_MSTATUS ( addAttribute(areamap) );
    CHECK_MSTATUS ( addAttribute(envmap) );
    CHECK_MSTATUS ( addAttribute(samplingquality) );
    CHECK_MSTATUS ( addAttribute(usePortalGeometry) );
    CHECK_MSTATUS ( addAttribute(portalGeometry) );
	

    //CHECK_MSTATUS ( addAttribute(aColor) );
    CHECK_MSTATUS ( addAttribute(aLightColor) );
    CHECK_MSTATUS ( addAttribute(aShadowColor) );
    CHECK_MSTATUS ( addAttribute(aPosition) );
    CHECK_MSTATUS ( addAttribute(aInputDirection) );
    CHECK_MSTATUS ( addAttribute(aInputAmbient) );
    CHECK_MSTATUS ( addAttribute(aInputDiffuse) );
    CHECK_MSTATUS ( addAttribute(aInputSpecular) );
    CHECK_MSTATUS ( addAttribute(aIntensity) );
	
    CHECK_MSTATUS ( addAttribute(aLightData) );

    CHECK_MSTATUS ( attributeAffects (aLightIntensity, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightDirection, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightAmbient, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightDiffuse, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightSpecular, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightShadowFraction, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aPreShadowIntensity, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightBlindData, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aLightData, aLightData) );

    //CHECK_MSTATUS ( attributeAffects (aColor, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aPosition, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aInputDirection, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aInputAmbient, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aInputDiffuse, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aInputSpecular, aLightData) );
    CHECK_MSTATUS ( attributeAffects (aIntensity, aLightData) );

    return MS::kSuccess;
}


//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mtmEnvLight::compute(
const MPlug&      plug,
      MDataBlock& block ) 
{ 
    if ((plug != aLightData) && (plug.parent() != aLightData))
		return MS::kUnknownParameter;

    MFloatVector resultColor;

	// Real user input
    MFloatVector LColor(1,0,0);
    //MFloatVector& LColor  = block.inputValue( aColor ).asFloatVector();
    // MFloatVector& Position  = block.inputValue( aPosition ).asFloatVector();
    float LIntensity  = block.inputValue( aIntensity ).asFloat();

	// Components to build LightData
    MFloatVector& LDirection  = block.inputValue( aInputDirection ).asFloatVector();
    bool  LAmbient  = block.inputValue( aInputAmbient ).asBool();
    bool  LDiffuse  = block.inputValue( aInputDiffuse ).asBool();
    bool  LSpecular = block.inputValue( aInputSpecular ).asBool();


    resultColor = LColor * LIntensity;
  
    // set ouput color attribute
    MDataHandle outLightDataHandle = block.outputValue( aLightData );

    MFloatVector& outIntensity = outLightDataHandle.child(aLightIntensity).asFloatVector();
    outIntensity = resultColor;

    MFloatVector& outDirection = outLightDataHandle.child(aLightDirection).asFloatVector();

    outDirection = LDirection;

    bool& outAmbient = outLightDataHandle.child(aLightAmbient).asBool();
    outAmbient = LAmbient;
    bool& outDiffuse = outLightDataHandle.child(aLightDiffuse).asBool();
    outDiffuse = LDiffuse;
    bool& outSpecular = outLightDataHandle.child(aLightSpecular).asBool();
    outSpecular = LSpecular;

    float& outSFraction = outLightDataHandle.child(aLightShadowFraction).asFloat();
    outSFraction = 1.0f;

    float& outPSIntensity = outLightDataHandle.child(aPreShadowIntensity).asFloat();
    outPSIntensity = (resultColor[0] + resultColor[1] + resultColor[2]) / 3.0f;

	void*& outBlindData = outLightDataHandle.child(aLightBlindData).asAddr();
	outBlindData = NULL;

    outLightDataHandle.setClean();


    return MS::kSuccess;
}
