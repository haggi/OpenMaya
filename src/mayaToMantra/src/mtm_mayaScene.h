#ifndef MTM_MAYA_SCENE
#define MTM_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>

#include "mayaScene.h"
class mtm_RenderGlobals;

class mtm_MayaScene : public MayaScene
{
public:
	mtm_RenderGlobals *renderGlobals;
	std::ofstream *outFile;
	MString ifdFileName;
	MayaObject *defaultCamera; // needed for motionblur calculation
	bool needsLightCategories; // if on, lights receive light category strings for shadow linking
	bool parseScene();
	bool updateScene();
	void runupDynamics();
	bool isMotionStep();
	bool sceneNeedsUpdate();
	bool exportSceneForFrame();
	bool exportShadingGroups();
	bool finalizeExportScene();
	bool executeGeoConverter();
	bool executeRenderCmd();
	virtual bool translateShaders(int timeStep);
	virtual bool translateShapes(int timeStep);
	bool renderScene();
	bool getPasses();
	virtual bool doPreRenderJobs();
	virtual bool doFrameJobs();
	virtual bool doPreFrameJobs();
	virtual bool doPostFrameJobs();
	virtual bool doPostRenderJobs();
	bool initIfdFile(); // ifd header like mantra version etc. 
	bool writeObjDefinitions();
	bool writeImagePlanes();
	bool writeOutputData();
	bool writeRendererData();

	virtual void transformUpdateCallback(MayaObject&);
	virtual void deformUpdateCallback(MayaObject&);
	virtual bool renderImage(); // the actual render job, overwrite
	virtual MayaObject* mayaObjectCreator(MObject&);
	virtual void mayaObjectDeleter(MayaObject *);
	virtual void getRenderGlobals();


	mtm_MayaScene();
	~mtm_MayaScene();
};

#endif
