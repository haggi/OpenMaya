#include "Corona.h"
#include "mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "renderGlobals.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"
#include "utilities/attrTools.h"
#include "CoronaMap.h"
#include "world.h"
#include "CoronaShaders.h"
#include <maya/MNodeMessage.h>

//#include "CoronaTestScene.h"

static Logging logger;

CoronaRenderer::CoronaRenderer()
{
	MStatus stat;
	Logging::debug("CoronaRenderer::CoronaRenderer()");
	this->context.core = nullptr;
	this->context.fb = nullptr;
	this->context.scene = nullptr;
	this->context.logger = nullptr;
	this->context.settings = nullptr;
	this->context.isCancelled = false;
	this->context.colorMappingData = nullptr;
	MObject renderGlobalsNode = getRenderGlobalsNode();
	this->renderFbGlobalsNodeCallbackId = 0;
	this->renderFbGlobalsNodeCallbackId = MNodeMessage::addNodeDirtyCallback(renderGlobalsNode, CoronaRenderer::frameBufferInteractiveCallback, nullptr, &stat);
}

CoronaRenderer::~CoronaRenderer()
{
	Logging::debug("CoronaRenderer::~CoronaRenderer()");

	if (this->renderFbGlobalsNodeCallbackId != 0)
		MNodeMessage::removeCallback(this->renderFbGlobalsNodeCallbackId);
	if (this->renderFbCamNodeCallbackId != 0)
		MNodeMessage::removeCallback(this->renderFbCamNodeCallbackId);

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
	context.renderPasses.clear();
	context.core->destroyFb(context.fb);
	if (context.scene != nullptr)
	{
		context.core->destroyScene(context.scene);
		context.scene = nullptr;
	}
	if (context.core != nullptr)
	{
		Corona::ICore::destroyInstance(context.core);
		context.core = nullptr;
	}
	if (context.colorMappingData != nullptr)
	{
		delete context.colorMappingData;
		context.colorMappingData = nullptr;
	}
}

void CoronaRenderer::frameBufferInteractiveCallback(MObject& node, void *clientData)
{
	Logging::debug("CoronaRenderer::frameBufferInteractiveCallback");
	MFnDependencyNode depFn(node);
	Logging::debug(MString("NodeType: ") + depFn.typeName());
	//MGlobal::executeCommand("refresh;");
	EventQueue::Event e;
	e.type = EventQueue::Event::INTERACTIVEFBCALLBACK;
	theRenderEventQueue()->push(e);
}

void CoronaRenderer::interactiveFbCallback()
{
	CoronaRenderer::framebufferCallback();
}

// if photographic exposure is used, we need access to the camera which was used for rendering
void CoronaRenderer::updateCameraFbCallback(MObject& camera)
{
	MStatus stat;
	if (this->renderFbCamNodeCallbackId != 0)
		MNodeMessage::removeCallback(this->renderFbCamNodeCallbackId);
	this->renderFbCamNodeCallbackId = MNodeMessage::addNodeDirtyCallback(camera, CoronaRenderer::frameBufferInteractiveCallback, nullptr, &stat);
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

	if (MGlobal::mayaState() != MGlobal::kBatch)
	{
		Corona::ICore::doLicensePopup();
	}
	Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
	if (!li.isUsable())
	{
		MString reason = li.error.cStr();
		MGlobal::displayError(MString("Sorry! Could not get a valid license.") + reason);
		return;
	}
	//createTestScene();
	//OSL::OSLShadingNetworkRenderer *r = new OSL::OSLShadingNetworkRenderer();

	//this->clearMaterialLists();
	this->defineCamera();
	this->defineGeometry();
	this->defineEnvironment();
	this->defineLights();

	context.core->sanityCheck(context.scene);
	Logging::debug(MString("registering framebuffer callback."));
	size_t framebufferCallbackId = RenderQueueWorker::registerCallback(&framebufferCallback);
	Corona::String basePath = (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->basePath + "/corona/").asChar();
	Logging::debug(MString("beginSession..."));
	ICore::AdditionalInfo info;
	info.defaultFilePath = basePath;
	context.core->beginSession(context.scene, context.settings, context.fb, context.logger, info);
	int maxCount=100, count=0;
	while (MayaTo::getWorldPtr()->getRenderState() != MayaTo::MayaToWorld::WorldRenderState::RSTATENONE)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		count++;
		if (count >= maxCount)
		{
			Logging::warning("RState is not RSTATENONE, but wait is over...");
			break;
		}
	}
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING);
	context.core->renderFrame(); // blocking render call
	context.core->endSession();
	RenderQueueWorker::unregisterCallback(framebufferCallbackId);
	framebufferCallback();
	this->saveImage();
	//std::string statsString = this->oslRenderer->shadingsys->getstats();
	//Logging::debug(statsString.c_str());
	OIIO::TextureSystem *tsystem = this->oslRenderer->renderer.texturesys();
	std::string statsString = tsystem->getstats(3);
	Logging::debug(statsString.c_str());
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATENONE);
}

// init all data which will be used during a rendering.
// These data will be reused in rendering until the rendering is done.
void CoronaRenderer::initializeRenderer()
{
	Logging::debug("CoronaRenderer::initializeRenderer()");

	clearDataList(); // clear nativeMtlData

	if (MGlobal::mayaState() != MGlobal::kBatch)
		MayaTo::getWorldPtr()->setRenderType(MayaTo::MayaToWorld::WorldRenderType::UIRENDER);
	else
		MayaTo::getWorldPtr()->setRenderType(MayaTo::MayaToWorld::WorldRenderType::BATCHRENDER);

	// first we delete any still existing elements.
	// after a render, the framebuffers, core and passes still exist until a new scene is loaded
	// or a new rendering is started.
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
	context.renderPasses.clear();
	if (context.fb != nullptr)
	{
		context.core->destroyFb(context.fb);
		context.fb = nullptr;
	}
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
	if (context.colorMappingData != nullptr)
	{
		delete context.colorMappingData;
		context.colorMappingData = nullptr;
	}

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

	// this can be extracted and placed in a button function in the render globals
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode renderGlobalsNode(getRenderGlobalsNode());
	Corona::String dumpFilename = (renderGlobals->getImageOutputFile() + ".dmp").asChar();
	if (getBoolAttr("dumpAndResume", renderGlobalsNode, false))
	{
		context.settings->set(Corona::PARAM_RANDOM_SEED, 0);
		if (!context.fb->accumulateFromExr(dumpFilename))
		{
			Logging::debug(MString("Accumulating from a dumpfile failed: ") + dumpFilename.cStr());
		}
		else{
			// random seed has to be 0 for resuming a render
			context.settings->set(Corona::PARAM_RESUME_RENDERING, true);
			context.settings->set(Corona::PARAM_RANDOM_SEED, 0);
		}
	}

	OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getWorldPtr()->getObjPtr("oslRenderer");
	if (r == nullptr)
	{
		Logging::debug("error CoronaRenderer::render: OSL renderer == nullptr");
		return;
	}
	this->oslRenderer = r;
	r->setup(); // delete existing shadingsys and reinit all

}

void CoronaRenderer::unInitializeRenderer()
{
	Logging::debug("CoronaRenderer::unInitializeRenderer()");
	if (context.scene != nullptr)
	{
		context.core->destroyScene(context.scene);
		context.scene = nullptr;
	}
	MayaTo::getWorldPtr()->setRenderType(MayaTo::MayaToWorld::WorldRenderType::RTYPENONE);
}

void CoronaRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{
	if( obj->instanceNumber > 0)
		return;

	if( obj->mobject.hasFn(MFn::kMesh))
	{
		if ((obj->meshDataList.size() == 0) || obj->isShapeConnected())
		{
			obj->addMeshData();
		}
	}
}

void CoronaRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{}

void CoronaRenderer::abortRendering()
{
	if( this->context.core != nullptr)
		this->context.core->cancelRender();
}

