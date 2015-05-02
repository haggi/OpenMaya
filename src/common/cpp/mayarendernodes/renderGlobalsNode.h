#ifndef MT_GLOBALS_H
#define MT_GLOBALS_H

#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MPxNode.h>


class MayaRenderGlobalsNode : public MPxNode
{
public:
						MayaRenderGlobalsNode();
	virtual				~MayaRenderGlobalsNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	static  void*		creator();
	virtual void		postConstructor();
	static  MStatus		initialize();

	static	MTypeId		id;
	MStringArray	imageFormatList;
	MStringArray	filterTypeList;

private:

	static    MObject adaptiveSampling;
	// sampling adaptive
	static    MObject minSamples;
	static    MObject maxSamples;

	// sampling raster based
	static    MObject samplesX;
	static    MObject samplesY;

	static    MObject doMotionBlur;
	static    MObject doDof;
	static	  MObject xftimesamples;
	static	  MObject geotimesamples;
	static	  MObject motionBlurRange;
	static	  MObject motionBlurType;
	// 
	static    MObject threads;
	static	  MObject translatorVerbosity;
	static    MObject rendererVerbosity;

	static    MObject detectShapeDeform;

	// pixel filtering
	static    MObject filtertype;
	static    MObject filtersize;
	static    MObject tilesize;

	static	  MObject gamma;


	static    MObject basePath;
	static    MObject imagePath;
	static    MObject imageName;

	static    MObject exrDataTypeHalf;
	static    MObject exrMergeChannels;

	// raytracing
	static    MObject maxTraceDepth;

	// sun light
	// if a renderer support physical sun or simply sun, 
	// you always have only one sun which can be plugged in here.
	static    MObject sunLightConnection;
	static    MObject useSunLightConnection;

	static	  MObject exportSceneFile;
	static	  MObject exportSceneFileName;
	static	  MObject sceneScale;
	static	  MObject imageFormat;
	static	  MObject optimizedTexturePath;
	static	  MObject useOptimizedTextures;

public:
	int		 defaultEnumFilterType;

};

#endif
