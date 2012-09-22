#ifndef MTL_GLOBALS_H
#define MTL_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToLuxGlobalsName = "luxGlobals";

class MayaToLuxGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToLuxGlobals();
	virtual				~MayaToLuxGlobals(); 

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

	static	  MObject assemblyExportType;
	static	  MObject assemblyPolyTheshold;

	static	  MObject environmentColor;
	static	  MObject gradientHorizon;
	static	  MObject gradientZenit;
	static	  MObject environmentMap;
	static	  MObject environmentType;
	static	  MObject environmentIntensity;

	static	  MObject imageFormat;

};

#endif