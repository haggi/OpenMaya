#include <maya/MGlobal.h>

#include "mtap_mayaScene.h"
#include "utilities/logging.h"
#include "appleseed.h"

static Logging logger;


mtap_MayaScene::mtap_MayaScene()
{
	getRenderGlobals();
	this->asr.mtap_scene = this;
	this->asr.renderGlobals = this->renderGlobals;
}

mtap_MayaScene::~mtap_MayaScene()
{
}

void mtap_MayaScene::transformUpdateCallback(MayaObject& obj)
{
	logger.debug(MString("mtap_MayaScene::transformUpdateCallback"));	
}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtap_MayaScene::deformUpdateCallback(MayaObject& obj)
{
	if( !obj.geometryShapeSupported() )
		return;
	if( !obj.mobject.hasFn(MFn::kMesh))
		return
	logger.debug(MString("mtap_MayaScene::deformUpdateCallback"));
	asr.defineMeshDeformStep( (mtap_MayaObject *)&obj);
}

MayaObject *mtap_MayaScene::mayaObjectCreator(MObject& mobject)
{
	return new mtap_MayaObject(mobject);
}

void mtap_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtap_MayaObject *mtap_obj = (mtap_MayaObject *)obj;
	delete mtap_obj;
	obj = NULL;
}

void mtap_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtap_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
}


bool mtap_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mtap_MayaScene::translateShaders");
	return true;
}

bool mtap_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mtap_MayaScene::translateShapes");
	return true;
}

bool mtap_MayaScene::doPreRenderJobs()
{
	logger.debug("mtap_MayaScene::doPreRenderJobs");
	return true;
}

bool mtap_MayaScene::doPreFrameJobs()
{
	logger.debug("mtap_MayaScene::doPreFrameJobs");

	// creating scene, env
	this->asr.definePreRender();
	this->asr.defineLights();
	return true;
}

bool mtap_MayaScene::doPostFrameJobs()
{
	logger.debug("mtap_MayaScene::doPostFrameJobs");
	return true;
}

bool mtap_MayaScene::doPostRenderJobs()
{
	logger.debug("mtap_MayaScene::doPostRenderJobs");
	return true;
}

bool mtap_MayaScene::renderImage()
{
	logger.debug("mtap_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->asr.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);

	if( this->renderGlobals->exportXMLFile)
		this->asr.writeXML();
	this->asr.render(this->renderGlobals);
	return true;
}


