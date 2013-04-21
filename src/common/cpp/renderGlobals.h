#ifndef MAYA_RENDERGLOBALS_H
#define MAYA_RENDERGLOBALS_H

#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <vector>

// Render pass definition goes into the global framework because most of the known passses are
// the same in all renderers, like shadowmap, photonmap, caustics, bake...

// A render pass contains a complete render procedure.
// e.g. a ShadowMap Render pass will start a new rendering. Here all shadow casting lights will produce
// shadow maps. They can be seen as a pendant to frames in a sequence. 

// During the render procedure the passes are collected in the render globals.
// There are preSequence passes. e.g. for shadow maps that are created only once before the final rendering.
// Then for every frame we have perFrame passes e.g. moving shadow maps or photon maps.

class RenderPass
{
public:
	enum RenderPassType{
		PassNone,
		PhotonGI,
		PhotonCaustic,
		ShadowMap,
		Bake,
		Beauty
	};

	enum EvalFrequency{
		FrequencyNone,
		OncePerFrame,
		OncePerJob
	};

	RenderPassType passType;
	EvalFrequency evalFrequency;
	
	std::vector<void *> objectList; // void element pointer to camera, light or other things

	RenderPass()
	{
		passType = PassNone;
		evalFrequency = FrequencyNone;
	};
};

// Some renderers offer different transform mb steps and geometry deformation steps (mantra)
// So I create this as as global framework option
class MbElement
{
public:

	enum Type{
		XForm,
		Geo,
		Both
	};

	MbElement::Type elementType;
	double time;

	// these are for sorting the mb steps
	bool operator>(const MbElement& other) const 
	{
		return ( time > other.time );
	};
	bool operator<(const MbElement& other) const 
	{
		return( time < other.time );
	};
	MbElement(void){};
};

struct RenderType{
	enum RType{
		FINAL,
		INTERACTIVE
	};
};

class RenderGlobals
{
public:
	
	MObject renderGlobalsMobject;
	bool good;
	
	bool doAnimation;
	float currentFrameNumber; // current real frame (with mb steps)
	float currentFrame; // current frame
	float startFrame;
	float endFrame;
	float byFrame;
	std::vector<float> frameList;

	bool inBatch;
	bool useShortNames;
	RenderType::RType renderType;

	int imgWidth;
	int imgHeight;
	float pixelAspect;

	// sampling
	int minSamples;
	int maxSamples;
	// regular sampling
	int samplesX;
	int samplesY;
	bool adaptiveSampling;

	// filtering
	float filterSize;
	int filterType;
	MString filterTypeString;

	float gamma;

	bool doMb;
	int mbtype; // leading = 0, centered = 1, trailing = 2
	float mbStartTime; // frame relative start time e.g. -0.2 frames
	float mbEndTime; // frame relative end time e.g. 0.2 frames
	float mbLength; // absolute length of motion blur, e.g. 0.4 frames
	bool doDof;
	int bitdepth; // 8, 16, 16halfFloat, 32float
	
	int threads;
	int tilesize;
	int translatorVerbosity;
	int rendererVerbosity;

	// raytracing
	int maxTraceDepth;
	//static    MObject detectShapeDeform;

	int xftimesamples;
	int geotimesamples;
	std::vector<MbElement> mbElementList;
	MbElement currentMbElement; // contains type and relative time
	int currentMbStep; // currend mb step id 0 - x
	bool isMbStartStep; 

	bool createDefaultLight;

	MString basePath;
	MString imagePath;
	MString imageName;
	MString imageOutputFile; // complete path to current image file

	MString preFrameScript;
	MString postFrameScript;
	MString preRenderLayerScript;
	MString postRenderLayerScript;

	bool	useRenderRegion;
	int		regionLeft;
	int		regionRight;
	int		regionBottom;
	int		regionTop;

	bool detectShapeDeform;

	bool exportSceneFile;
	MString exportSceneFileName;
	MMatrix sceneScaleMatrix;
	float sceneScale;
	MString optimizedTexturePath;

	std::vector<RenderPass *> renderPasses;
	RenderPass *currentRenderPass;
	int currentRenderPassElementId;

	RenderGlobals();
	~RenderGlobals();
	bool getDefaultGlobals();
	bool getMbSteps( MObject& camera);
	bool isTransformStep();
	bool isDeformStep();
	void getImageName();
	virtual MString getImageExt() = 0;
};

#endif
