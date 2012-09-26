#ifndef MTAP_MAYA_SCENE
#define MTAP_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>
#include <map>

#include "mayaScene.h"
#include "mtap_renderGlobals.h"
#include "mtap_MayaObject.h"
#include "appleseed.h"
#include "utilities/MiniMap.h"


class mtap_MayaScene : public MayaScene
{
public:
	AppleRender::AppleseedRenderer asr;
	mtap_RenderGlobals *renderGlobals;
	MayaObject *defaultCamera; // needed for motionblur calculation
	
	//std::map<MayaObject *, MObject> mayaObjMObjMap;
	MiniMap<MObject, MayaObject *> mayaObjMObjMap;

	void transformUpdateCallback(MayaObject&);
	void deformUpdateCallback(MayaObject&);
	virtual bool translateShaders(int timeStep);
	virtual bool translateShapes(int timeStep);
	virtual bool doPreRenderJobs();
	virtual bool renderImage();
	virtual bool doPreFrameJobs();
	virtual bool doPostFrameJobs();
	virtual bool doPostRenderJobs();
	virtual MayaObject* mayaObjectCreator(MObject&);
	virtual void mayaObjectDeleter(MayaObject *);
	virtual void getRenderGlobals();
	virtual void stopRendering();
	virtual void updateInteraciveRenderScene(std::vector<MObject> mobjList);
	void mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList);

	void makeMayaObjectMObjMap(); // fill with maya objects and dag nodes for later fast access
	mtap_MayaObject *getMayaObjectFromMap(MObject& mobj);

	mtap_MayaScene();
	~mtap_MayaScene();
};


#endif
