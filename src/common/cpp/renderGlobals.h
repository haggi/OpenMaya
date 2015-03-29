#ifndef MAYA_RENDERGLOBALS_H
#define MAYA_RENDERGLOBALS_H

#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <maya/MDistance.h>
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
		Both,
		None
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

	enum RendererUpAxis{
		XUp,
		YUp,
		ZUp
	};

	MDistance::Unit internalUnit;
	MDistance::Unit rendererUnit;
	RendererUpAxis internalAxis;
	RendererUpAxis rendererAxis;
	
	float internalScaleFactor;
	float rendererScaleFactor;
	float toMillimeters(float mm);

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
	RenderType::RType renderType;

private:
	int imgWidth;
	int imgHeight;
	int currentFrameIndex;
public:
	void setWidth(int w) { this->imgWidth = w; };
	void setHeight(int h) { this->imgHeight = h; };
	void setWidthHeight(int w, int h) { this->imgWidth = w; this->imgHeight = h; };
	void getWidthHeight(int& w, int& h){ w = this->imgWidth; h = this->imgHeight; };
	int getWidth(){ return this->imgWidth; };
	int getHeight(){ return this->imgHeight; };
	float updateFrameNumber(); // returns the current frame number and incements the currentFrameIndex
	float getFrameNumber();
	bool frameListDone();
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
	float motionBlurRange; // float range default = 0.4 ~ 144°
	int motionBlurType; // Center, FrameStart, FrameEnd
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
	bool isMbStartStep(); 

	bool createDefaultLight;

	MString basePath;
	MString imagePath;
	MString imageName;
	MString imageOutputFile; // complete path to current image file
	int		imageFormatId;
	MString imageFormatString;

	MString preFrameScript;
	MString postFrameScript;
	MString preRenderLayerScript;
	MString postRenderLayerScript;

private:
	bool	useRenderRegion;
	int		regionLeft;
	int		regionRight;
	int		regionBottom;
	int		regionTop;

public:
	void checkRenderRegion();
	void setUseRenderRegion(bool useRegion){ useRenderRegion = useRegion; checkRenderRegion(); };
	bool getUseRenderRegion() { return useRenderRegion; };
	void getRenderRegion(int& left, int& bottom, int& right, int& top) { left = regionLeft; bottom = regionBottom; right = regionRight; top = regionTop; };
	bool detectShapeDeform;
	bool exportSceneFile;
	MString exportSceneFileName;

	bool useSunLightConnection;

	MMatrix globalConversionMatrix; // for default unit conversion e.g. centimeter to meter
	MMatrix sceneScaleMatrix; // user defined scene scale
	float scaleFactor;

	float sceneScale;
	MString optimizedTexturePath;
	bool useOptimizedTextures;

	std::vector<RenderPass *> renderPasses;
	RenderPass *currentRenderPass;
	int currentRenderPassElementId;

	RenderGlobals();
	~RenderGlobals();
	bool getDefaultGlobals();
	bool getMbSteps();
	bool isTransformStep();
	bool isDeformStep();
	void getImageName();
	MString getImageOutputFile();
	MString getImageExt();

	// these methods are not defined in the base implementation, they should be implemented in every renderer
	void setRendererUnit();
	void setRendererAxis();
	void defineGlobalConversionMatrix();
	//virtual MObject getRenderGlobalsNode();
};

#endif
