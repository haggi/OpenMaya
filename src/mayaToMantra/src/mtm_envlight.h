#ifndef MTM_ENVLIGHT
#define MTM_ENVLIGHT

#include <math.h>

#include <maya/MPxLocatorNode.h> 
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MVector.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>

const MString mtmEnvLightName = "mtm_envlight";

class mtmEnvLight : public MPxLocatorNode
{
	public:
                      mtmEnvLight();
    virtual           ~mtmEnvLight();

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 

    virtual MStatus   compute( const MPlug&, MDataBlock& );
	virtual void      postConstructor();

    static void *     creator();
    static MStatus    initialize();

    static MTypeId    id;

	private:
	// Inputs
	//static MObject  aColor;
	static MObject  aLightColor;
	static MObject  aShadowColor;
	static MObject  aPosition;
	static MObject  aInputDirection;
    static MObject  aInputAmbient;
    static MObject  aInputDiffuse;
    static MObject  aInputSpecular;
    static MObject  aIntensity;

    static MObject  samplingquality;
    static MObject  envmap;
    static MObject  areamap;
    static MObject  areafullsphere;
    static MObject  envintensity;
    static MObject  raybackground;
    static MObject  castshadow;	
    static MObject  envtype;	
    static MObject  doraysamples;	
    static MObject  doadaptive;	
    static MObject  domaxdist;	
    static MObject  maxdist;	
    static MObject  coneangle;	
    static MObject  envtint;	
    static MObject  shadowI;	
    static MObject  envnull;	
    static MObject  samples;	
    static MObject  objectmask;	
    static MObject  usePortalGeometry;	
    static MObject  portalGeometry;	

// Outputs
	static MObject  aLightDirection;
	static MObject  aLightIntensity;
    static MObject  aLightAmbient;
    static MObject  aLightDiffuse;
    static MObject  aLightSpecular;
    static MObject  aLightShadowFraction;
    static MObject  aPreShadowIntensity;
    static MObject  aLightBlindData;
    static MObject  aLightData;
};

#endif