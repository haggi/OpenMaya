#ifndef MTM_MAYA_ROPTIONS
#define MTM_MAYA_ROPTIONS

#include <maya/MString.h>
#include <vector>
#include <maya/MObject.h>

#include "mtm_mayaObject.h"

enum RenderPass{
	mtmNone,
	mtmPhotonGI,
	mtmPhotonCaustic,
	mtmShadowMap,
	mtmBake,
	mtmBeauty
};

class PassElement
{
public:
	enum Type{
		mtmNone,
		mtmCamera,
		mtmLight,
		mtmPhoton
	};

	RenderPass renderPass;
	Type passType;
	MayaObject *mo; // pointer because we have to modify some of them

	PassElement(){
		passType = mtmNone;
		mo = NULL;
	};
};

// because we can have different mb type exports I create a struct
// which saves the type and the frame to save.
class MbExportElement
{
public:

	enum Type{
		XForm,
		Geo,
		Both
	};

	MbExportElement::Type elementType;
	double time;

	bool operator>(const MbExportElement& other) const 
	{
		return ( time > other.time );
	};
	bool operator<(const MbExportElement& other) const 
	{
		return( time < other.time );
	};
	MbExportElement(void){};
};

class RenderGlobals
{
public:

	MString basePath;
	MString imagePath;
	MString imageName;
	MString imageOutputFile;

	RenderPass currentPass;
	PassElement currentPassElement;

	MStringArray renderEngines;
	bool isGood;
	bool inBatch;
	bool detectShapeDeform;
	bool doAnimation;
	int outputType; // 0 = outputToMPlay 1 = outputToFile
	double currentFrameNumber;
	double startFrame;
	double endFrame;
	double byFrame;
	int height, width;
	double pixelAspect;
	std::vector<double> frameList;
	int2 samples;
	int translatorVerbosity;
	MString verbosity;
	bool doMb;
	bool doDof;
	int xftimesamples;
	int geotimesamples;
	int mbtype;
	int renderengine;
	float scaleFactor;
	float jitter;
	int bitdepth; // 8, 16, 16halfFloat, 32float
	int reflectlimit;
	int refractlimit;
	int minRaySamples;
	int maxRaySamples;
	float noiselevel;
	int randomseed;
	bool rayvariance;
	bool binaryGeoExport;

	std::vector<MbExportElement> mbElements;
	MbExportElement currentMbElement;
	int currentMbStep;
	//std::vector<ShaderNode> supportedShaders;
	//MObject currentCamera;

	bool getDefaultGlobals();
	bool getMantraGlobals();
	//bool getShaderInfoData();
	//ShaderNode getSupportedShaderNode( MString name);
	bool getMbSteps(MObject& camera);

	RenderGlobals();
	~RenderGlobals();

};


//
//class RenderOptions
//{
//public:
//	MString basePath;
//	MString imagePath;
//	MString imageName;
//	MString imageOutputFile;
//
//	MStringArray renderEngines;
//	bool isGood;
//	int output;
//	double currentFrameNumber;
//	double startFrame;
//	double endFrame;
//	double byFrame;
//	int height, width;
//	double pixelAspect;
//	int2 samples;
//	int translatorVerbosity;
//	MString verbosity;
//	bool doMb;
//	int xftimesamples;
//	int geotimesamples;
//	int mbtype;
//	std::vector<MbExportElement> mbElements;
//	MbExportElement currentMbElement;
//	int currentMbStep;
//	std::vector<ShaderNode> supportedShaders;
//	std::vector<double> frameList;
//	bool inBatch;
//	MObject currentCamera;
//	int renderengine;
//	float scaleFactor;
//	bool doDof;
//
//	bool getDefaultGlobals();
//	bool getMantraGlobals();
//	bool getShaderInfoData();
//	ShaderNode getSupportedShaderNode( MString name);
//	bool getMbSteps(MObject& camera);
//
//	RenderOptions();
//	~RenderOptions();
//
//};

#endif
