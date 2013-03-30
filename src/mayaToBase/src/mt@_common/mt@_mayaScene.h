#ifndef mt@_MAYA_SCENE
#define mt@_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>
#include <map>

#include "mayaScene.h"
#include "mt@_renderGlobals.h"
#include "mt@_MayaObject.h"
#include "../@/@.h"
#include "utilities/MiniMap.h"


class mt@_MayaScene : public MayaScene
{
public:
	@Renderer mt@_renderer;
	mt@_RenderGlobals *renderGlobals;
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
	mt@_MayaObject *getMayaObjectFromMap(MObject& mobj);
	bool parseScene(ParseType ptype = NORMALPARSE);
	mt@_MayaScene();
	mt@_MayaScene(RenderType rtype);
	~mt@_MayaScene();
};


#endif
