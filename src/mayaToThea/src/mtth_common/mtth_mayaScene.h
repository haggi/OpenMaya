#ifndef mtth_MAYA_SCENE
#define mtth_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>
#include <map>

#include "mayaScene.h"
#include "mtth_renderGlobals.h"
#include "mtth_MayaObject.h"
#include "../Thea/Thea.h"
#include "../Thea/TheaDummy.h"
#include "utilities/MiniMap.h"


class mtth_MayaScene : public MayaScene
{
public:
	//TheaDRenderer mtth_renderer;
	TheaRenderer mtth_renderer;
	mtth_RenderGlobals *renderGlobals;
	MayaObject *defaultCamera; // needed for motionblur calculation
	
	//std::map<MayaObject *, MObject> mayaObjMObjMap;
	MiniMap<MObject, MayaObject *> mayaObjMObjMap;

	virtual void transformUpdateCallback(MayaObject *);
	virtual void shapeUpdateCallback(MayaObject *);
	virtual bool translateShaders(int timeStep);
	virtual bool translateShapes(int timeStep);
	virtual bool doPreRenderJobs();
	virtual bool renderImage();
	virtual bool doPreFrameJobs();
	virtual bool doPostFrameJobs();
	virtual bool doPostRenderJobs();
	virtual MayaObject* mayaObjectCreator(MObject&);
	virtual MayaObject* mayaObjectCreator(MDagPath&);
	virtual void mayaObjectDeleter(MayaObject *);
	virtual void getRenderGlobals();
	virtual void stopRendering();
	virtual void updateInteraciveRenderScene(std::vector<MObject> mobjList);
	void mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList);
	virtual bool postParseCallback();
	void makeMayaObjectMObjMap(); // fill with maya objects and dag nodes for later fast access
	mtth_MayaObject *getMayaObjectFromMap(MObject& mobj);
	mtth_MayaScene();
	mtth_MayaScene(RenderType rtype);
	virtual void userThreadProcedure();
	~mtth_MayaScene();
};


#endif
