#include "Thea.h"
#include <SDK/Integration/sdk.kernel.h>
#include <maya/MFileIO.h>
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"
#include "mayaScene.h"
#include "renderGlobals.h"
#include "world.h"

TheaRenderer::TheaRenderer()
{
	isGood = true;
	
	//if(!TheaSDK::Init())
	//	isGood = false;
}

TheaRenderer::~TheaRenderer()
{
	TheaSDK::Shutdown();
}

void TheaRenderer::renderEndCallback( void *ptr )
{
	TheaRenderer *renderer = (TheaRenderer *)ptr;

}

void TheaRenderer::interactiveFbCallback(){}

void TheaRenderer::render()
{
	clearMaterialLists();

	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<MayaScene> scene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	MString fileName = scene->getFileName();
	renderGlobals->exportSceneFile = true; // trigger use of XML commands
	if (renderGlobals->exportSceneFile)
		this->sceneXML = TheaSDK::XML::Scene(fileName.asChar());
	else
		this->scene = TheaSDK::Scene::New(fileName.asChar());

	MString moduleDir = getRendererHome();
	MString gpuDriver = moduleDir + "bin/Plugins/Presto/presto-x64.cuda.dll";
	MString cpuDriver = moduleDir + "bin/Plugins/Presto/presto-x64.cpu.dll";

	bool gpuSuccess = TheaSDK::Kernel::Root().initPrestoCUDADriver(gpuDriver.asChar());
	bool cpuSuccess = TheaSDK::Kernel::Root().initPrestoCPUDriver(cpuDriver.asChar());
	if( !gpuSuccess )
		Logging::error("Unable to load presto gpu driver.");
	if( !cpuSuccess )
		Logging::error("Unable to load presto cpu driver.");
	
	if(!isGood )
	{
		EventQueue::Event e;
		e.type = EventQueue::Event::FRAMEDONE;
		theRenderEventQueue()->push(e);
		return;
	}

	this->defineSettings();
	this->defineCamera();
	this->defineGeometry();
	this->defineLights();
	this->defineEnvironment();
	
	if (renderGlobals->exportSceneFile)
	{
		std::string currentFile = MFileIO::currentFile().asChar();
		std::vector<std::string> parts;
		pystring::split(currentFile, parts, "/");
		currentFile = pystring::replace(parts.back(), ".ma", "");
		currentFile = pystring::replace(currentFile, ".mb", "");
		MString scenePath = MString("C:/daten/3dprojects/mayaToThea/data/Thea/") + currentFile.c_str() + ".xml";
		//MString scenePath = this->mtth_renderGlobals->basePath + "/Thea/" + currentFile.c_str() + ".xml";
		TheaSDK::XML::Importer::Write(this->sceneXML);
		Logging::debug(MString("Exporting scene to: ") + scenePath);
		TheaSDK::SaveScene(scenePath.asChar());
	}


	volatile bool isrendering=true;

	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING);
	size_t framebufferCallbackId = RenderQueueWorker::registerCallback(&framebufferCallback);

	// don't start asynchronus because we are in a seperate task anyway
	bool ok = TheaSDK::StartRendering(renderEndCallback, (void *)this, false, false);
	framebufferCallback();

	RenderQueueWorker::unregisterCallback(framebufferCallbackId);

	renderGlobals->getImageName();
	MString filename = renderGlobals->imageOutputFile.asChar();
	std::string imgFormatExt = renderGlobals->imageOutputFile.toLowerCase().asChar();
	std::vector<std::string> fileParts;
	pystring::split(imgFormatExt, fileParts, ".");
	std::string ext = fileParts.back();
	
	Logging::debug(MString("Saving image as ") +  filename);
	TheaSDK::SaveImage(filename.asChar());

	EventQueue::Event e;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void TheaRenderer::initializeRenderer()
{}
void TheaRenderer::unInitializeRenderer()
{}
void TheaRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{}
void TheaRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{}
void TheaRenderer::IPRUpdateEntities()
{}
void TheaRenderer::reinitializeIPRRendering()
{}
void TheaRenderer::abortRendering()
{
	TheaSDK::StopRendering(true);
}

