#ifndef mtin_MAYA_SCENE
#define mtin_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>
#include <map>

#include "mayaScene.h"
#include "mtin_renderGlobals.h"
#include "mtin_MayaObject.h"
#include "../Indigo/Indigo.h"
#include "utilities/MiniMap.h"


class mtin_MayaScene : public MayaScene
{
public:
	IndigoRenderer mtin_renderer;
	mtin_RenderGlobals *renderGlobals;
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
	virtual void userThreadProcedure();
	void makeMayaObjectMObjMap(); // fill with maya objects and dag nodes for later fast access
	mtin_MayaObject *getMayaObjectFromMap(MObject& mobj);
	bool parseScene(ParseType ptype = NORMALPARSE);
	mtin_MayaScene();
	mtin_MayaScene(RenderType rtype);
	~mtin_MayaScene();
};


#endif
