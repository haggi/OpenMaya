#ifndef MTAP_MAYA_SCENE
#define MTAP_MAYA_SCENE

#include <maya/MDagPathArray.h>
#include <fstream>
#include <map>

#include "mayaScene.h"
#include "mtap_renderGlobals.h"
#include "mtap_MayaObject.h"
#include "appleseed.h"

template <class T, class S> class MiniMap
{
public:
	void append(T, S);
	S *find(T);
	void clear();
private:
	std::vector<T> tClass;
	std::vector<S> sClass;
};

template <class T, class S> void MiniMap<T,S>::clear()
{
	this->tClass.clear();
	this->sClass.clear();
}

template <class T, class S> void MiniMap<T,S>::append(T t, S s)
{
	size_t i = 0;
	for( i = 0; i < this->tClass.size(); i++)
	{
		if( tClass[i] == t )
		{
			break;
		}
	}
	if( i >= this->tClass.size())
	{
		tClass.push_back(t);
		sClass.push_back(s);
	}else{
		sClass[i] = s;
	}
}

template <class T, class S> S *MiniMap<T,S>::find(T t)
{
	S *s = NULL;
	for( size_t i = 0; i < this->tClass.size(); i++)
	{
		if( tClass[i] == t )
		{
			s = &sClass[i];
			break;
		}
	}
	return s;
}

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
