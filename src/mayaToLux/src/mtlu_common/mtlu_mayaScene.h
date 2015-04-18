//#ifndef mtlu_MAYA_SCENE
//#define mtlu_MAYA_SCENE
//
//#include <maya/MDagPathArray.h>
//#include <fstream>
//#include <map>
//
//#include "mayaScene.h"
//#include "mtlu_renderGlobals.h"
//#include "mtlu_MayaObject.h"
//#include "../Lux/Lux.h"
//#include "utilities/MiniMap.h"
//
//
//class mtlu_MayaScene : public MayaScene
//{
//public:
//	LuxRenderer mtlu_renderer;
//	mtlu_RenderGlobals *renderGlobals;
//	MayaObject *defaultCamera; // needed for motionblur calculation
//	
//	//std::map<MayaObject *, MObject> mayaObjMObjMap;
//	MiniMap<MObject, MayaObject *> mayaObjMObjMap;
//
//	virtual void transformUpdateCallback(MayaObject *);
//	virtual void shapeUpdateCallback(MayaObject *);
//	virtual bool translateShaders(int timeStep);
//	virtual bool translateShapes(int timeStep);
//	virtual bool doPreRenderJobs();
//	virtual bool renderImage();
//	virtual bool doPreFrameJobs();
//	virtual bool doPostFrameJobs();
//	virtual bool doPostRenderJobs();
//	virtual MayaObject* mayaObjectCreator(MObject&);
//	virtual MayaObject* mayaObjectCreator(MDagPath&);
//	virtual void mayaObjectDeleter(MayaObject *);
//	virtual void getRenderGlobals();
//	virtual void stopRendering();
//	virtual void updateInteraciveRenderScene(std::vector<MObject> mobjList);
//	void mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList);
//	virtual bool postParseCallback();
//	void makeMayaObjectMObjMap(); // fill with maya objects and dag nodes for later fast access
//	mtlu_MayaObject *getMayaObjectFromMap(MObject& mobj);
//	bool parseScene(ParseType ptype = NORMALPARSE);
//	mtlu_MayaScene();
//	mtlu_MayaScene(RenderType rtype);
//	~mtlu_MayaScene();
//};
//
//
//#endif
