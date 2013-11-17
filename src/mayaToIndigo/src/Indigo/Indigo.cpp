#include "Indigo.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
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
	//==================== Create renderer settings node =========================
	//Indigo::SceneNodeRenderSettingsRef settings(Indigo::SceneNodeRenderSettings::getDefaults()); // Initialise it with the default values.
	//// Override a few values for our example:
	//settings->setBoolSetting("metropolis", false);
	//settings->setBoolSetting("bidirectional", false);
	//settings->setDoubleSetting("image_save_period", 10000000.0);
	//settings->setIntSetting("width", this->mtin_renderGlobals->imgWidth);
	//settings->setIntSetting("height", this->mtin_renderGlobals->imgHeight);
	//settings->setBoolSetting("info_overlay", true);
	//settings->setDoubleSetting("halt_time", this->mtin_renderGlobals->halt_time);

	//sceneRootRef->addChildNode(settings); // Add the settings node to the scene graph.

	this->defineCamera();

	//mtin_MayaObject *icam = (mtin_MayaObject *)this->mtin_scene->camList[0];
	//MFnCamera camFn(icam->dagPath);
	//MVector camUp = camFn.upDirection(MSpace::kWorld);
	//MVector camView = camFn.viewDirection(MSpace::kWorld);
	//MPoint camPos = camFn.eyePoint(MSpace::kWorld);

	////==================== Create camera node =========================
	//Indigo::SceneNodeCameraRef cam(new Indigo::SceneNodeCamera());
	//cam->lens_radius = 0.0001;
	//cam->autofocus = false;
	//cam->exposure_duration = 1.0 / 30.0;
	//cam->focus_distance = 1.0;
	//cam->lens_sensor_dist = 0.03;
	//cam->lens_shift_right_distance = 0;
	//cam->lens_shift_up_distance = 0;
	//cam->sensor_width = 0.035;

	//cam->forwards = Indigo::Vec3d(camView.x, camView.y, camView.z);
	//cam->up = Indigo::Vec3d(camUp.x, camUp.y, camUp.z);
	//cam->setPos(Indigo::Vec3d(camPos.x, camPos.y, camPos.z));

	//cam->forwards = Indigo::Vec3d(0, 1, 0);
	//cam->up = Indigo::Vec3d(0, 0, 1);
	//cam->setPos(Indigo::Vec3d(0, -6, 0.3));


	//==================== Create tone-mapping node (required) =========================
	Indigo::SceneNodeTonemappingRef tone_mapping(new Indigo::SceneNodeTonemapping());
	tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
	tone_mapping->pre_scale = 1;
	tone_mapping->post_scale = 1;
	tone_mapping->burn = 6;

	sceneRootRef->addChildNode(tone_mapping);

	this->defineEnvironment();

	//////==================== Create sun/sky environment light ====================
	//// This is an optional node.
	//if(true)
	//{
	//	Indigo::SceneNodeBackgroundSettingsRef background_settings_node(new Indigo::SceneNodeBackgroundSettings());

	//	Reference<Indigo::SunSkyMaterial> sun_sky_mat(new Indigo::SunSkyMaterial());
	//	sun_sky_mat->model = "captured-simulation";
	//	sun_sky_mat->sundir = Indigo::Vec3d(1,1,1); // Direction to sun.

	//	background_settings_node->background_material = sun_sky_mat;

	//	sceneRootRef->addChildNode(background_settings_node); // Add to scene graph
	//}

	//////==================== Create an environment map light ====================
	//// This is an optional node.
	//if(false)
	//{
	//	Indigo::SceneNodeBackgroundSettingsRef background_settings(new Indigo::SceneNodeBackgroundSettings());

	//	Reference<Indigo::DiffuseMaterial> mat(new Indigo::DiffuseMaterial());

	//	// Albedo should be zero.
	//	mat->albedo = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(0))));

	//	// Emission is a texture parameter that references our texture that we will create below.
	//	mat->emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::TextureWavelengthDependentParam(0));

	//	// Base emission is the emitted spectral radiance.
	//	mat->base_emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(35.0))));

	//	Indigo::Texture texture;
	//	texture.path = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/indigo_dll_example_vs2010/ColorChecker_sRGB_from_Ref.jpg"; // You will usually want to use a lat-long EXR environment map here
	//	texture.exponent = 1; // Since we will usually use a HDR image, the exponent (gamma) should be set to one.
	//	texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::SphericalTexCoordGenerator(Reference<Indigo::Rotation>(new Indigo::MatrixRotation())));

	//	mat->textures.push_back(texture);

	//	background_settings->background_material = mat;

	//	sceneRootRef->addChildNode(background_settings);
	//}


	{
		//==================== Create light geometry =========================
		Indigo::SceneNodeMeshRef mesh_node(new Indigo::SceneNodeMesh());
		mesh_node->mesh = Indigo::MeshRef(new Indigo::Mesh());
		mesh_node->mesh->num_uv_mappings = 0;
		
		// Make a single quad
		Indigo::Quad q;
		
		// Set the material index to the first material of the object.
		q.mat_index = 0;

		for(int i = 0; i < 4; ++i)
		{
			q.vertex_indices[i] = i;
			q.uv_indices[i] = 0;
		}

		// Add it to mesh's quads
		mesh_node->mesh->quads.push_back(q);

		// Define the vertices
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(0, 0, 2));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(0, 1, 2));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(1, 1, 2));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(1, 0, 2));

		mesh_node->mesh->endOfModel(); // Build the mesh
		
		sceneRootRef->addChildNode(mesh_node);


		//==================== Create an emitting material =========================
		Indigo::SceneNodeMaterialRef mat(new Indigo::SceneNodeMaterial());

		Reference<Indigo::DiffuseMaterial> diffuse = new Indigo::DiffuseMaterial();
		diffuse->random_triangle_colours = false;
		diffuse->layer = 0;
		diffuse->base_emission = new Indigo::ConstantWavelengthDependentParam(new Indigo::UniformSpectrum(1.0e10));
		diffuse->emission = new Indigo::ConstantWavelengthDependentParam(new Indigo::UniformSpectrum(1.0));
		mat->material = diffuse;

		sceneRootRef->addChildNode(mat);


		//==================== Create the light object =========================
		Indigo::SceneNodeModelRef model(new Indigo::SceneNodeModel());
		model->setName("Light Object");
		model->setGeometry(mesh_node);
		model->keyframes.push_back(Indigo::KeyFrame());
		model->rotation = new Indigo::MatrixRotation();
		model->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, mat));

		sceneRootRef->addChildNode(model);
	}

	//{
	//	//==================== Create ground geometry =========================
	//	Indigo::SceneNodeMeshRef mesh_node(new Indigo::SceneNodeMesh());
	//	mesh_node->setName("Ground Mesh");
	//	mesh_node->mesh = Indigo::MeshRef(new Indigo::Mesh());

	//	// Make a single quad
	//	Indigo::Quad q;
	//	q.mat_index = 0;
	//	for(int i = 0; i < 4; ++i)
	//		q.vertex_indices[i] = q.uv_indices[i] = i;
	//	mesh_node->mesh->quads.push_back(q);

	//	mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(-2, -2, 0));
	//	mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(-2, 2, 0));
	//	mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(2, 2, 0));
	//	mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(2, -2, 0));

	//	// Make a UV mapping for the quad
	//	mesh_node->mesh->num_uv_mappings = 1;

	//	mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(0, 0));
	//	mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(0, 1));
	//	mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(1, 1));
	//	mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(1, 0));

	//	mesh_node->mesh->endOfModel(); // Build the mesh

	//	sceneRootRef->addChildNode(mesh_node);


	//	//==================== Create the ground material =========================
	//	Indigo::SceneNodeMaterialRef mat(new Indigo::SceneNodeMaterial());

	//	Indigo::DiffuseMaterial* diffuse = new Indigo::DiffuseMaterial();
	//	diffuse->random_triangle_colours = false;
	//	diffuse->layer = 0;
	//	diffuse->albedo = new Indigo::TextureWavelengthDependentParam(
	//		0 // texture index
	//	);

	//	diffuse->textures.push_back(Indigo::Texture());
	//	diffuse->textures.back().path = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/indigo_dll_example_vs2010/ColorChecker_sRGB_from_Ref.jpg";
	//	diffuse->textures.back().tex_coord_generation = new Indigo::UVTexCoordGenerator();

	//	mat->material = diffuse;
	//	mat->setName("Ground diffuse material");

	//	sceneRootRef->addChildNode(mat);

	//	if(false)
	//	{
	//		// Example code to create a blend material instead:
	//		Indigo::SceneNodeMaterialRef mat_a(new Indigo::SceneNodeMaterial());
	//		Indigo::DiffuseMaterial* diffuse_a = new Indigo::DiffuseMaterial();
	//		mat_a->material = diffuse_a;
	//		mat_a->setName("Child mat A");
	//		diffuse_a->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(1,0,0), 2.2));
	//		sceneRootRef->addChildNode(mat_a);

	//		Indigo::SceneNodeMaterialRef mat_b(new Indigo::SceneNodeMaterial());
	//		Indigo::DiffuseMaterial* diffuse_b = new Indigo::DiffuseMaterial();
	//		mat_b->material = diffuse_b;
	//		mat_b->setName("Child mat B");
	//		diffuse_b->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(0,1,0), 2.2));
	//		sceneRootRef->addChildNode(mat_b);

	//		Indigo::SceneNodeMaterialRef blend_node(new Indigo::SceneNodeMaterial());
	//		Indigo::BlendMaterial* blend = new Indigo::BlendMaterial();
	//		blend_node->setName("Blend mat");
	//		blend_node->material = blend;
	//		blend->blend = new Indigo::ConstantDisplacementParam(0.5);
	//		blend->a_mat = mat_a;
	//		blend->b_mat = mat_b;

	//		sceneRootRef->addChildNode(blend_node);
	//	}


	//	//==================== Create the ground object =========================
	//	Indigo::SceneNodeModelRef model(new Indigo::SceneNodeModel());
	//	model->setName("Ground Object");
	//	model->setGeometry(mesh_node);
	//	model->keyframes.push_back(Indigo::KeyFrame());
	//	model->rotation = new Indigo::MatrixRotation();
	//	model->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, mat));
	//	
	//	sceneRootRef->addChildNode(model); // Add node to scene graph.

	//}

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

	if(sizeof(void*) == 8) // If this is a 64-bit executable:
		indigo_dll_dir = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/binaries/vs2010_64_debug";
	else 
		indigo_dll_dir = "../binaries/vs" + msvc_version + "_32";

	// Path to the appdata directory. This is where the cache directories are written, where the log is written, and where the licence key is read from.
	// An example appdata path would be 'C:\Users\nick\AppData\Roaming\Indigo Renderer'
	const Indigo::String indigo_appdata_path = "C:/daten/3dprojects/Indigo/data";

	Indigo::IndigoContextRef context(new Indigo::IndigoContext());
	// Call the initialise function for the renderer object. This must be called before any other methods on the renderer object are called.
	Indigo::String error_string;
	indResult result = context->initialise(indigo_dll_dir, indigo_appdata_path, error_string);
	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Failed to initialise Indigo context, error code: " << result << ", error description: " << toStdString(error_string) << std::endl;
		return;
	}

	
	// Create the renderer
	//Indigo::RendererRef renderer(new Indigo::Renderer(context));
	rendererRef = Indigo::RendererRef(new Indigo::Renderer(context));

	createSceneGraph();

	// Save the scene graph to an IGS on disk so we can look at it in the Indigo GUI.
	try
	{
		sceneRootRef->writeToXMLFileOnDisk("C:/daten/3dprojects/Indigo/example.igs", true, NULL);
	}
	catch(Indigo::IndigoException& e)
	{
		std::cerr << toStdString(e.what()) << std::endl;
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
	writeUInt8BufferToBMP((imgName + ".bmp").asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
	writeFloatBufferToBMP((imgName + "fromFloat.bmp").asChar(), floatBufferRef->dataPtr(), floatBufferRef->width(), floatBufferRef->height());

	std::cout << "Render written to render.bmp." << std::endl;

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
}
