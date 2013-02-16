#ifndef MT_GLOBALS_H
#define MT_GLOBALS_H

#include <maya/MTypeId.h>
#include <maya/MString.h>
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

	// 
	static    MObject threads;
	static	  MObject translatorVerbosity;
	static    MObject rendererVerbosity;

	static    MObject detectShapeDeform;

	// pixel filtering
	// filtertype is renderer specific. It will be defined in subclass
	static    MObject filtersize;
	static    MObject tilesize;

	static	  MObject gamma;


	static    MObject basePath;
	static    MObject imagePath;
	static    MObject imageName;

	// raytracing
	static    MObject maxTraceDepth;

	static	  MObject exportSceneFile;
	static	  MObject exportSceneFileName;
	static	  MObject sceneScale;

public:
	bool adaptiveSamplingDV;
	// sampling adaptive
	int minSamplesDV;
	int maxSamplesDV;

	// sampling raster based
	int samplesXDV;
	int samplesYDV;

	bool doMotionBlurDV;
	bool doDofDV;

	// 
	int threadsDV;
	int translatorVerbosityDV;
	int rendererVerbosityDV;

	bool detectShapeDeformDV;

	// pixel filtering
	// filtertype is renderer specific. It will be defined in subclass
	float filtersizeDV;
	int tilesizeDV;

	float gammaDV;

	//MString basePathDV;
	//MString imagePathDV;
	//MString imageNameDV;

	// raytracing
	int maxTraceDepthDV;

	bool exportSceneFileDV;
	MString exportSceneFileNameDV;
	float sceneScaleDV;


};

#endif
