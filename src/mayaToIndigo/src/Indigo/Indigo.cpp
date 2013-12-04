#include "Indigo.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "../mtin_common/mtin_renderGlobals.h"
#include "../mtin_common/mtin_mayaScene.h"

#include <maya/MFnCamera.h>

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

	//==================== Create tone-mapping node (required) =========================
	Indigo::SceneNodeTonemappingRef tone_mapping(new Indigo::SceneNodeTonemapping());
	tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
	tone_mapping->pre_scale = 1;
	tone_mapping->post_scale = 1;
	tone_mapping->burn = 6;

	sceneRootRef->addChildNode(tone_mapping);

	this->defineEnvironment();
	this->defineGeometry();
}

// Bitmap writing code


#define BITMAP_ID        0x4D42       // this is the universal id for a bitmap

#pragma pack (push, 1)

typedef struct {
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data, bytes */
} BITMAP_HEADER;

typedef struct {
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
} BITMAP_INFOHEADER;

#pragma pack (pop)


void writeFloatBufferToBMP(const Indigo::String& path, float* buffer, size_t w, size_t h)
{
	// Convert buffer data to 8-bit first:
	std::vector<uint8> uint8_buf(w * h * 4);

	for(size_t i=0; i<w * h * 4; ++i)
		uint8_buf[i] = (uint8)(buffer[i] * 255.9f);

	writeUInt8BufferToBMP(path, &uint8_buf[0], w, h);
}


void writeUInt8BufferToBMP(const Indigo::String& path, uint8* buffer, size_t w, size_t h)
{
	int bytesPerLine = (int)w * 3;
	// round up to a dword boundary
	if(bytesPerLine & 0x0003) 
	{
		bytesPerLine |= 0x0003;
		++bytesPerLine;
	}

	BITMAP_HEADER header;
	memset(&header, 0, sizeof(BITMAP_HEADER));
	header.type = BITMAP_ID;
	header.size = sizeof(BITMAP_HEADER) + sizeof(BITMAP_INFOHEADER) + bytesPerLine * (int)h;
	header.offset = sizeof(BITMAP_HEADER) + sizeof(BITMAP_INFOHEADER);

	BITMAP_INFOHEADER infoheader;
	memset(&infoheader, 0, sizeof(BITMAP_INFOHEADER));
	infoheader.size = sizeof(infoheader);
	infoheader.width = (int)w;
	infoheader.height = (int)h;
	infoheader.planes = 1;
	infoheader.bits = 24;
	infoheader.compression = 0L;

	Indigo::Vector<char> buf = path.getNullTerminatedBuffer();
	std::ofstream file(&buf[0], std::ios::out | std::ios::binary);
	if(!file)
	{
		std::cerr << "Could not open file for writing." << std::endl;
		exit(1);
	}

	// Write headers
	file.write((const char*)&header, sizeof(header));
	file.write((const char*)&infoheader, sizeof(infoheader));

	// Write image data
	char* line = new char[bytesPerLine];
	for(int y=(int)h-1; y >= 0; --y)
	{
		for(unsigned int x=0; x<w; ++x)
		{
			line[x*3] = ((const char*)buffer)[(y*w + x)*4 + 2]; // b
			line[x*3+1] = ((const char*)buffer)[(y*w + x)*4 + 1]; // g
			line[x*3+2] = ((const char*)buffer)[(y*w + x)*4]; // r
		}

		file.write(line, bytesPerLine);
	}
	delete[] line;
}


IndigoRenderer::IndigoRenderer()
{
	rendererStarted = false;
	this->rendererAborted = false;
}
IndigoRenderer::~IndigoRenderer()
{}

void IndigoRenderer::render()
{

	// Path to the directory in which indigo_sdk.dll resides.
	// The IndigoContext object needs to know this path so it can load the DLL successfully.
	Indigo::String indigo_dll_dir;

	// Pick the correct binaries directory to use based on what Visual Studio compiler version we are using.
#if _MSC_VER >= 1600 && _MSC_VER < 1700
		const Indigo::String msvc_version = "2010";
#elif _MSC_VER >= 1700 && _MSC_VER < 1800
		const Indigo::String msvc_version = "2012";
#else
#error Unknown MSVC version."
#endif

	//indigo_dll_dir = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/binaries/vs2010_64_debug";
	MString rendererHome = getRendererHome();
	logger.debug(MString("Homedir: ") + rendererHome);

	indigo_dll_dir = (rendererHome + "/bin").asChar();

	// Path to the appdata directory. This is where the cache directories are written, where the log is written, and where the licence key is read from.
	// An example appdata path would be 'C:\Users\nick\AppData\Roaming\Indigo Renderer'
	const Indigo::String indigo_appdata_path = (rendererHome + "/bin/data").asChar();
	Indigo::IndigoContextRef context(new Indigo::IndigoContext());
	Indigo::String error_string;
	indResult result = context->initialise(indigo_dll_dir, indigo_appdata_path, error_string);

	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Failed to initialise Indigo context, error code: " << result << ", error description: " << toStdString(error_string) << std::endl;
		EventQueue::Event e;
		e.data = NULL;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);

		return;
	}

	
	// Create the renderer
	//Indigo::RendererRef renderer(new Indigo::Renderer(context));
	rendererRef = Indigo::RendererRef(new Indigo::Renderer(context));

	createSceneGraph();

	// Save the scene graph to an IGS on disk so we can look at it in the Indigo GUI.
	try
	{		
		sceneRootRef->writeToXMLFileOnDisk("C:/daten/3dprojects/mayaToIndigo/example.igs", true, NULL);
	}
	catch(Indigo::IndigoException& ex)
	{
		std::cerr << toStdString(ex.what()) << std::endl;
		EventQueue::Event e;
		e.data = NULL;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);
		return;
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
	catch(Indigo::IndigoException& e)
	{
		std::cerr << "finalise() failed: " << toStdString(e.what()) << std::endl;
	}

	result = rendererRef->initialiseWithScene(sceneRootRef, render_buffer, command_line_args); // Non-blocking.
	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Renderer failed to initialise(), error code: " << result << std::endl;
	}

	// Start rendering. This call will create the render threads, and start the rendering process.
	// This will happen after the scene building completes.
	rendererStarted = true;
	rendererRef->startRendering(); // Non-blocking.
	this->parse();
	
	// Start tone-mapping. The actual tone-mapping work will be done in another thread.
	this->toneMapperRef->startToneMapping();

	// Wait until the tone-mapping has been completed.
	while(!this->toneMapperRef->isToneMappingDone())
	{
		std::cout << "Waiting for final tone mapping ..." << std::endl;
		Sleep(50);
	}

	rendererRef->stopRendering();

	// Write the tone-mapped image to disk.
	MString imgName = this->mtin_renderGlobals->getImageOutputFile();
	if( this->mtin_renderGlobals->imageFormatString == "exr")
	{
		rendererRef->saveEXR((imgName).asChar());
		logger.debug(MString("Saved image as: ") + imgName);
	}

	//if( this->mtin_renderGlobals->imageFormatString == "bmp")
	//{
		writeUInt8BufferToBMP((imgName + ".bmp").asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
		//writeFloatBufferToBMP((imgName + "fromFloat.bmp").asChar(), floatBufferRef->dataPtr(), floatBufferRef->width(), floatBufferRef->height());
		logger.debug(MString("Saved image as: ") + imgName);
	//}

	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void IndigoRenderer::initializeRenderer()
{}
void IndigoRenderer::updateShape(MayaObject *obj)
{}
void IndigoRenderer::updateTransform(MayaObject *obj)
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
