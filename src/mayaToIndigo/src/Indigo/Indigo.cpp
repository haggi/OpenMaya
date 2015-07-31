#include "Indigo.h"
#include "IndigoImgExport.h"
#include "renderGlobals.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "world.h"

#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>

#include <IndigoTypes.h>
#include <IndigoString.h>
#include <IndigoErrorCodes.h>
#include <IndigoHandle.h>
#include <IndigoMessageInterface.h>
#include <IndigoEventMessageInterface.h>
#include <IndigoErrorMessageInterface.h>
#include <IndigoLogMessageInterface.h>
#include <IndigoProgressMessageInterface.h>
#include <IndigoUpdateDoneMessageInterface.h>
#include <SceneNodeMesh.h>
#include <SceneNodeMaterial.h>
#include <SceneNodeModel.h>
#include <IndigoSceneParser.h>
#include <UInt8Buffer.h>
#include <RenderBuffer.h>
#include <IndigoContext.h>
#include <IndigoLock.h>
#include <IndigoHardwareInfo.h>
#include <IndigoMaterialSerialisation.h>
#include <IndigoDataManager.h>

#include <iostream>
#include <fstream>
#include <windows.h>
#include <assert.h>
#include <string>
#include <vector>

static Logging logger;

void writeUInt8BufferToBMP(const Indigo::String& path, uint8* buffer, size_t w, size_t h);
void writeFloatBufferToBMP(const Indigo::String& path, float* buffer, size_t w, size_t h);

const size_t render_width = 800;
const size_t render_height = 600;


// This function creates an Indigo scene graph and returns it.
void IndigoRenderer::createSceneGraph()
{
	//==================== Create root node =========================
	sceneRootRef = Indigo::SceneNodeRootRef(new Indigo::SceneNodeRoot());

	this->createRenderSettings();
	this->defineCamera();
	this->createToneMapper();
	this->defineEnvironment();
	this->defineGeometry();
}


IndigoRenderer::IndigoRenderer()
{
	rendererStarted = false;
	this->rendererAborted = false;
}
IndigoRenderer::~IndigoRenderer()
{
	logger.debug("~IndigoRenderer");
}

void IndigoRenderer::render()
{
	MFnDependencyNode glFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	// Path to the directory in which indigo_sdk.dll resides.
	// The IndigoContext object needs to know this path so it can load the DLL successfully.
	Indigo::String indigo_dll_dir;

	// Pick the correct binaries directory to use based on what Visual Studio compiler version we are using.
	const Indigo::String msvc_version = "2010";

	MString rendererHome = getRendererHome();
	logger.debug(MString("Renderer homedir: ") + rendererHome);
	
#ifdef _DEBUG
	indigo_dll_dir = "H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK/binaries/vs2010_64_debug";
#else
	indigo_dll_dir = (rendererHome + "/bin").asChar();
#endif

	// Path to the appdata directory. This is where the cache directories are written, where the log is written, and where the licence key is read from.
	// An example appdata path would be 'C:\Users\nick\AppData\Roaming\Indigo Renderer'
	const Indigo::String indigo_appdata_path = (rendererHome + "/bin/data").asChar();
	Indigo::IndigoContextRef context(new Indigo::IndigoContext());
	Indigo::String error_string;
	logger.debug(MString("Initislize renderer with dllDir:") + (rendererHome + "/bin appdata path: ") + (rendererHome + "/bin/data"));
	indResult result = context->initialise(indigo_dll_dir, indigo_appdata_path, error_string);

	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Failed to initialise Indigo context, error code: " << result << ", error description: " << toStdString(error_string) << std::endl;
		EventQueue::Event e;
		e.pixelData = nullptr;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);
		return;
	}

	// Get GPU Info and Hardware ID.  This is just here for demonstration purposes and is not needed to start the render.
	{
		Indigo::HardwareInfoRef hw_info(new Indigo::HardwareInfo(context));

		//
		// Get GPU Info.
		Indigo::Handle<Indigo::GPUInfoList> GPUinfos = hw_info->queryGPUInfo();

		// Do something with it.
		Indigo::Vector<Indigo::GPUInfo> gpu_info_copy = GPUinfos->getGPUInfos();

		//
		// Get hardware ID
		Indigo::Handle<Indigo::HardwareIdentifier> hardwareID = hw_info->getHardwareIdentifier();

		// Hardware ID is empty. There was an error.
		if (hardwareID->getHardwareID().empty())
		{
			const Indigo::String error_string = hardwareID->getError();
		}
		else
		{
			// Great success.
			const Indigo::String hw_id_string = hardwareID->getHardwareID();
		}
	}


	// Create the renderer
	//Indigo::RendererRef renderer(new Indigo::Renderer(context));
	rendererRef = Indigo::RendererRef(new Indigo::Renderer(context));
	createSceneGraph();

	// Save the scene graph to an IGS on disk so we can look at it in the Indigo GUI.
	if (getBoolAttr("exportSceneFile", glFn, false))
	{
		try
		{	
			std::string currentFile = MFileIO::currentFile().asChar();
			std::vector<std::string> parts;
			pystring::split(currentFile, parts, "/");
			currentFile = pystring::replace(parts.back(), ".ma", "");
			currentFile = pystring::replace(currentFile, ".mb", "");
			MString scenePath = getStringAttr("basePath", glFn, "") + "/indigo/" + currentFile.c_str() + ".igs";
			logger.debug(MString("Writing igs file to ") + scenePath);
			logger.debug(MString("Renderglobals export file ") + getStringAttr("exportSceneFileName", glFn, "") );
			sceneRootRef->writeToXMLFileOnDisk(scenePath.asChar(), true, nullptr);
		}
		catch(Indigo::IndigoException& ex)
		{
			std::cerr << toStdString(ex.what()) << std::endl;
			EventQueue::Event e;
			e.pixelData = nullptr;
			e.type = EventQueue::Event::RENDERERROR;
			theRenderEventQueue()->push(e);
			return;
		}
	}
	// Create the main render buffer
	Indigo::RenderBufferRef render_buffer(new Indigo::RenderBuffer(sceneRootRef));

	// Create the low dynamic range (LDR) buffer
	Indigo::UInt8BufferRef uint8_buffer(new Indigo::UInt8Buffer());

	// Create the floating point buffer.  This buffer is optional, and only needs to be non-null if you want to 
	// have the resulting tone-mapped image stored in a floating-point format.
	this->floatBufferRef = Indigo::FloatBufferRef(new Indigo::FloatBuffer());

	// Create a tone mapper object.
	this->toneMapperRef = Indigo::ToneMapperRef(new Indigo::ToneMapper(context, render_buffer, uint8_buffer, floatBufferRef, 2.2f));
	this->toneMapperRef->update(sceneRootRef->getToneMapping(), sceneRootRef->getRendererSettings(), sceneRootRef->getCamera());


	// Build the scene from the scene graph.
	Indigo::Vector<Indigo::String> command_line_args;
	command_line_args.push_back("Dummy_scene_path");

	try
	{
		sceneRootRef->finalise(command_line_args[0]);
	}
	catch(Indigo::IndigoException& ex)
	{
		std::cerr << "finalise() failed: " << toStdString(ex.what()) << std::endl;
		EventQueue::Event e;
		e.pixelData = nullptr;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);
		return;
	}
	Indigo::DataManagerRef data_manager = new Indigo::DataManager(context);

	result = rendererRef->initialiseWithScene(sceneRootRef, render_buffer, command_line_args, data_manager); // Non-blocking.
	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Renderer failed to initialise(), error code: " << result << std::endl;
		EventQueue::Event e;
		e.pixelData = nullptr;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);
		return;
	}

	// Start rendering. This call will create the render threads, and start the rendering process.
	// This will happen after the scene building completes.
	rendererStarted = true;
	rendererRef->startRendering(); // Non-blocking.

	size_t framebufferCallbackId = RenderQueueWorker::registerCallback(&framebufferCallback);
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING);

	this->parse(); // this one is blocking
	
	// Start tone-mapping. The actual tone-mapping work will be done in another thread.
	this->toneMapperRef->startToneMapping();

	// Wait until the tone-mapping has been completed.
	while(!this->toneMapperRef->isToneMappingDone())
	{
		std::cout << "Waiting for final tone mapping ..." << std::endl;
		Sleep(50);
	}
	rendererRef->stopRendering();

	RenderQueueWorker::unregisterCallback(framebufferCallbackId);

	// Write the tone-mapped image to disk.
	MString imgFormatExt = pystring::lower(getStringAttr("imageFormatString", glFn, "exr").asChar()).c_str();
	if(( imgFormatExt != "exr") && (imgFormatExt != "bmp"))
	{
		logger.debug(MString("Unsupported image output format: ") + imgFormatExt + " -using exr: ");
		imgFormatExt = "exr";
	}
	MString imgName = renderGlobals->getImageOutputFile();
	if( imgFormatExt == "exr")
	{
		rendererRef->saveEXR((imgName).asChar());
		logger.debug(MString("Saved image as: ") + imgName);
	}

	if( imgFormatExt == "bmp")
	{
		BitMap bm;
		bm.writeUInt8BufferToBMP((imgName).asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
		logger.debug(MString("Saved image as: ") + imgName);
	}
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATENONE);
}

void IndigoRenderer::initializeRenderer()
{}
void IndigoRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{}
void IndigoRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{}
void IndigoRenderer::IPRUpdateEntities()
{}
void IndigoRenderer::reinitializeIPRRendering()
{}
void IndigoRenderer::abortRendering()
{
	logger.debug("User requested to abort rendering.");
	rendererRef->stopRendering();
	this->rendererAborted = true;
}
