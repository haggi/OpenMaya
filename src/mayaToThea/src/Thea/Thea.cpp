#include "Thea.h"
#include <SDK/Integration/sdk.kernel.h>
#include <maya/MFileIO.h>
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"
#include "../mtth_common/mtth_mayaScene.h"

static Logging logger;

TheaRenderer::TheaRenderer()
{
	isGood = true;
	
	if(!TheaSDK::Init())
		isGood = false;
}

TheaRenderer::~TheaRenderer()
{
	TheaSDK::Shutdown();
}

void TheaRenderer::renderEndCallback( void *ptr )
{
	TheaRenderer *renderer = (TheaRenderer *)ptr;

}

void TheaRenderer::render()
{
	clearMaterialLists();

	MString fileName = this->mtth_scene->getFileName();
	this->mtth_renderGlobals->exportSceneFile = true; // trigger use of XML commands
	if( this->mtth_renderGlobals->exportSceneFile )
		this->sceneXML = TheaSDK::XML::Scene(fileName.asChar());
	else
		this->scene = TheaSDK::Scene::New(fileName.asChar());

	//std::string cpuDriver = TheaSDK::Kernel::Root().getPrestoCPUDriverFile();
	//std::string gpuDriver = TheaSDK::Kernel::Root().getPrestoGPUDriverFile();
	//logger.debug(MString("CPU Driver: ") + cpuDriver.c_str());
	//logger.debug(MString("GPU Driver: ") + gpuDriver.c_str());

	MString moduleDir = getRendererHome();
	MString gpuDriver = moduleDir + "bin/Plugins/Presto/presto-x64.dll";
	MString cpuDriver = moduleDir + "bin/Plugins/Presto/presto-x86-x64.dll";

	bool gpuSuccess = TheaSDK::Kernel::Root().initPrestoGPUDriver(gpuDriver.asChar());
	bool cpuSuccess = TheaSDK::Kernel::Root().initPrestoCPUDriver(cpuDriver.asChar());
	if( !gpuSuccess )
		logger.error("Unable to load presto gpu driver.");
	if( !cpuSuccess )
		logger.error("Unable to load presto cpu driver.");
	
	if(!isGood )
	{
		EventQueue::Event e;
		e.data = NULL;
		e.type = EventQueue::Event::FRAMEDONE;
		theRenderEventQueue()->push(e);
		return;
	}

	this->defineSettings();
	this->defineCamera();
	this->defineGeometry();
	this->defineLights();
	this->defineEnvironment();
	
	if( this->mtth_renderGlobals->exportSceneFile )
	{
		std::string currentFile = MFileIO::currentFile().asChar();
		std::vector<std::string> parts;
		pystring::split(currentFile, parts, "/");
		currentFile = pystring::replace(parts.back(), ".ma", "");
		currentFile = pystring::replace(currentFile, ".mb", "");
		MString scenePath = MString("C:/daten/3dprojects/mayaToThea/data/Thea/") + currentFile.c_str() + ".xml";
		//MString scenePath = this->mtth_renderGlobals->basePath + "/Thea/" + currentFile.c_str() + ".xml";
		TheaSDK::XML::Importer::Write(this->sceneXML);
		logger.debug(MString("Exporting scene to: ") + scenePath);
		TheaSDK::SaveScene(scenePath.asChar());
	}


	volatile bool isrendering=true;

	// don't start asynchronus because we are in a seperate task anyway
	bool ok = TheaSDK::StartRendering(renderEndCallback, (void *)this, false, false);

	this->mtth_renderGlobals->getImageName();
	MString filename = this->mtth_renderGlobals->imageOutputFile.asChar();
	std::string imgFormatExt = this->mtth_renderGlobals->imageOutputFile.toLowerCase().asChar();
	std::vector<std::string> fileParts;
	pystring::split(imgFormatExt, fileParts, ".");
	std::string ext = fileParts.back();
	
	//filename = filename + ".jpg";
	logger.debug(MString("Saving image as ") +  filename);
	TheaSDK::SaveImage(filename.asChar());

	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void TheaRenderer::initializeRenderer()
{}
void TheaRenderer::updateShape(MayaObject *obj)
{}
void TheaRenderer::updateTransform(MayaObject *obj)
{}
void TheaRenderer::IPRUpdateEntities()
{}
void TheaRenderer::reinitializeIPRRendering()
{}
void TheaRenderer::abortRendering()
{
	TheaSDK::StopRendering(true);
}

