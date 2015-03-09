#include "Corona.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "renderGlobals.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"
#include "utilities/attrTools.h"
#include "CoronaMap.h"
#include "world.h"

static Logging logger;

CoronaRenderer::CoronaRenderer()
{
	Logging::debug("CoronaRenderer::CoronaRenderer()");
	this->context.core = NULL;
	this->context.fb = NULL;
	this->context.scene = NULL;
	this->context.logger = NULL;
	this->context.settings = NULL;
	this->context.isCancelled = false;
	this->context.colorMappingData = NULL;
}

CoronaRenderer::~CoronaRenderer()
{
	Logging::debug("CoronaRenderer::~CoronaRenderer()");
	//Corona::ICore::shutdownLib();
}

#ifdef CORONA_RELEASE_ASSERTS
#pragma comment(lib, "PrecompiledLibs_Assert.lib")
#pragma comment(lib, "Corona_Assert.lib")
#else
#pragma comment(lib, "PrecompiledLibs_Release.lib")
#pragma comment(lib, "Corona_Release.lib")
#endif

using namespace Corona;


void CoronaRenderer::createScene()
{
}

void CoronaRenderer::render()
{
	Logging::debug("CoronaRenderer::render");
	//Logging::debug("Rendering for 10 secs.");
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	//Logging::debug("Sleeping done.");

	//this->clearMaterialLists();

	this->defineCamera();
	//this->defineGeometry();
	//this->defineEnvironment();
	//this->defineLights();

	context.core->sanityCheck(context.scene);

	Logging::debug(MString("registering framebuffer callback."));
	size_t framebufferCallbackId = RenderQueueWorker::registerCallback(&framebufferCallback);

	Corona::String basePath = (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->basePath + "/corona/").asChar();
	Logging::debug(MString("beginSession..."));
	ICore::AdditionalInfo info;
	info.defaultFilePath = basePath;
	context.core->beginSession(context.scene, context.settings, context.fb, context.logger, info);
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING);
	context.core->renderFrame(); // blocking render call
    context.core->endSession();
	RenderQueueWorker::unregisterCallback(framebufferCallbackId);
	framebufferCallback();
	this->saveImage();

	//for( size_t objId = 0; objId < this->mtco_scene->objectList.size(); objId++)
	//{
	//	std::shared_ptr<MayaObject> obj = (std::shared_ptr<MayaObject> )this->mtco_scene->objectList[objId];
	//	obj->geom = NULL;
	//	obj->instance = NULL;
	//}
}

// init all data which will be used during a rendering.
// These data will be reused in rendering until the rendering is done.
void CoronaRenderer::initializeRenderer()
{
	Logging::debug("CoronaRenderer::initializeRenderer()");
	context.settings = new Settings();
	context.core = ICore::createInstance();
	context.scene = context.core->createScene();
	context.logger = new mtco_Logger(context.core);
	context.colorMappingData = new Corona::ColorMappingData;

	ConfParser parser;
	Corona::String resPath = (getRendererHome() + "ressources/").asChar();
	Logging::debug(MString("parser: ") + (resPath + CORONA_DEFAULT_CONF_FILENAME).cStr());
	parser.parseFile(resPath + CORONA_DEFAULT_CONF_FILENAME, context.settings, ConfParser::CREATE_IF_NONEXISTENT);

	this->defineSettings();
	this->defineColorMapping();
	this->definePasses();
	context.fb = context.core->createFb();
	context.fb->initFb(context.settings, context.renderPasses);
	context.core->sanityCheck(context.settings);

	//renderGlobals->getImageName();
	//Corona::String dumpFilename = (renderGlobals->imageOutputFile + ".dmp").asChar();
	//if (getBoolAttr("dumpAndResume", rGlNode, false))
	//{
	//	context.settings->set(Corona::PARAM_RANDOM_SEED, 0);
	//	if (!context.fb->accumulateFromExr(dumpFilename))
	//	{
	//		Logging::debug(MString("Accumulating from a dumpfile failed: ") + dumpFilename.cStr());
	//	}
	//	else{
	//		// random seed has to be 0 for resuming a render
	//		context.settings->set(Corona::PARAM_RESUME_RENDERING, true);
	//		context.settings->set(Corona::PARAM_RANDOM_SEED, 0);
	//	}
	//}

	OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getWorldPtr()->getObjPtr("oslRenderer");
	if (r == NULL)
	{
		Logging::debug("error CoronaRenderer::render: OSL renderer == NULL");
		return;
	}
	this->oslRenderer = r;
	r->setup(); // delete existing shadingsys and reinit all

}

void CoronaRenderer::unInitializeRenderer()
{
	Logging::debug("CoronaRenderer::unInitializeRenderer()");
	if (context.logger != nullptr)
	{
		delete context.logger;
		context.logger = nullptr;
	}
	if (context.settings != nullptr)
	{
		delete context.settings;
		context.settings = nullptr;
	}
	for (auto it : context.renderPasses)
	{
		context.core->destroyRenderPass(it);
	}
	context.renderPasses.clear();
	context.core->destroyFb(context.fb);
	if (context.scene != nullptr)
	{
		context.core->destroyScene(context.scene);
		context.scene = nullptr;
	}
	if (context.core != nullptr)
	{
		ICore::destroyInstance(context.core);
		context.core = nullptr;
	}	
}

void CoronaRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{
	if( obj->instanceNumber > 0)
		return;

	if( obj->mobject.hasFn(MFn::kMesh))
	{
		if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->isMbStartStep() || obj->isShapeConnected())
		{
			obj->addMeshData();
		}
	}
}

void CoronaRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{}

void CoronaRenderer::abortRendering()
{
	if( this->context.core != NULL)
		this->context.core->cancelRender();
}

