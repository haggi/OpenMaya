#ifndef MTAP_GLOBALS_H
#define MTAP_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToAppleseedGlobalsName = "appleseedGlobals";

class MayaToAppleseedGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToAppleseedGlobals();
	virtual				~MayaToAppleseedGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
	// 8, 16, 32 etc.
	static    MObject bitdepth;
	static    MObject colorSpace;
	static	  MObject clamping;
	static	  MObject maxError;
	static	  MObject caustics;
	static	  MObject diffuseDepth;
	static	  MObject glossyDepth;
	static	  MObject directLightSamples;

	static    MObject filtertype;
	static	  MObject lightingEngine;
	static	  MObject exportXMLFile;
	static	  MObject exportXMLFileName;
	static	  MObject optimizedTexturePath;

	static	  MObject assemblyExportType;
	static	  MObject assemblyPolyTheshold;

	static	  MObject environmentColor;
	static	  MObject gradientHorizon;
	static	  MObject gradientZenit;
	static	  MObject environmentMap;
	static	  MObject environmentType;
	static	  MObject environmentIntensity;
	static	  MObject latlongHoShift;
	static	  MObject latlongVeShift;

	static	  MObject imageFormat;
	static	  MObject sceneScale;

};

#endif