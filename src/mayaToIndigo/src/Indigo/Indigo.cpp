#include "Indigo.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "../mtin_common/mtin_renderGlobals.h"

#include <IndigoTypes.h>
#include <IndigoString.h>
#include <IndigoErrorCodes.h>
#include <IndigoHandle.h>
#include <IndigoMessageInterface.h>
#include <IndigoErrorMessageInterface.h>
#include <IndigoLogMessageInterface.h>
#include <IndigoProgressMessageInterface.h>
#include <IndigoUpdateDoneMessageInterface.h>
#include <SceneNodeRoot.h>
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

inline const std::string toStdString(const Indigo::String& s)
{
	return std::string(s.dataPtr(), s.length());
}


const size_t render_width = 800;
const size_t render_height = 600;


// This function creates an Indigo scene graph and returns it.
Indigo::SceneNodeRootRef IndigoRenderer::createSceneGraph()
{
	//==================== Create root node =========================
	Indigo::SceneNodeRootRef root(new Indigo::SceneNodeRoot());

	//==================== Create renderer settings node =========================
	Indigo::SceneNodeRenderSettingsRef settings(Indigo::SceneNodeRenderSettings::getDefaults()); // Initialise it with the default values.
	// Override a few values for our example:
	settings->setBoolSetting("metropolis", false);
	settings->setBoolSetting("bidirectional", false);
	settings->setDoubleSetting("image_save_period", 10000000.0);
	settings->setIntSetting("width", this->mtin_renderGlobals->imgWidth);
	settings->setIntSetting("height", this->mtin_renderGlobals->imgHeight);
	settings->setBoolSetting("info_overlay", true);
	settings->setDoubleSetting("halt_time", 15.0f);

	root->addChildNode(settings); // Add the settings node to the scene graph.

	//==================== Create camera node =========================
	Indigo::SceneNodeCameraRef cam(new Indigo::SceneNodeCamera());
	cam->lens_radius = 0.0001;
	cam->autofocus = false;
	cam->exposure_duration = 1.0 / 30.0;
	cam->focus_distance = 1.0;
	cam->lens_sensor_dist = 0.03;
	cam->lens_shift_right_distance = 0;
	cam->lens_shift_up_distance = 0;
	cam->sensor_width = 0.035;
	cam->forwards = Indigo::Vec3d(0, 1, 0);
	cam->up = Indigo::Vec3d(0, 0, 1);
	cam->setPos(Indigo::Vec3d(0, -2, 0.3));

	root->addChildNode(cam);

	//==================== Create tone-mapping node (required) =========================
	Indigo::SceneNodeTonemappingRef tone_mapping(new Indigo::SceneNodeTonemapping());
	tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
	tone_mapping->pre_scale = 1;
	tone_mapping->post_scale = 1;
	tone_mapping->burn = 6;

	root->addChildNode(tone_mapping);


	////==================== Create sun/sky environment light ====================
	// This is an optional node.
	if(false)
	{
		Indigo::SceneNodeBackgroundSettingsRef background_settings_node(new Indigo::SceneNodeBackgroundSettings());

		Reference<Indigo::SunSkyMaterial> sun_sky_mat(new Indigo::SunSkyMaterial());
		sun_sky_mat->model = "captured-simulation";
		sun_sky_mat->sundir = Indigo::Vec3d(1,1,1); // Direction to sun.

		background_settings_node->background_material = sun_sky_mat;

		root->addChildNode(background_settings_node); // Add to scene graph
	}

	////==================== Create an environment map light ====================
	// This is an optional node.
	if(true)
	{
		Indigo::SceneNodeBackgroundSettingsRef background_settings(new Indigo::SceneNodeBackgroundSettings());

		Reference<Indigo::DiffuseMaterial> mat(new Indigo::DiffuseMaterial());

		// Albedo should be zero.
		mat->albedo = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(0))));

		// Emission is a texture parameter that references our texture that we will create below.
		mat->emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::TextureWavelengthDependentParam(0));

		// Base emission is the emitted spectral radiance.
		mat->base_emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(1.0e10))));

		Indigo::Texture texture;
		texture.path = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/indigo_dll_example_vs2010/ColorChecker_sRGB_from_Ref.jpg"; // You will usually want to use a lat-long EXR environment map here
		texture.exponent = 1; // Since we will usually use a HDR image, the exponent (gamma) should be set to one.
		texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::SphericalTexCoordGenerator(Reference<Indigo::Rotation>(new Indigo::MatrixRotation())));

		mat->textures.push_back(texture);

		background_settings->background_material = mat;

		root->addChildNode(background_settings);
	}


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

		root->addChildNode(mesh_node);


		//==================== Create an emitting material =========================
		Indigo::SceneNodeMaterialRef mat(new Indigo::SceneNodeMaterial());

		Reference<Indigo::DiffuseMaterial> diffuse = new Indigo::DiffuseMaterial();
		diffuse->random_triangle_colours = false;
		diffuse->layer = 0;
		diffuse->base_emission = new Indigo::ConstantWavelengthDependentParam(new Indigo::UniformSpectrum(1.0e10));
		diffuse->emission = new Indigo::ConstantWavelengthDependentParam(new Indigo::UniformSpectrum(1.0));
		mat->material = diffuse;

		root->addChildNode(mat);


		//==================== Create the light object =========================
		Indigo::SceneNodeModelRef model(new Indigo::SceneNodeModel());
		model->setName("Light Object");
		model->setGeometry(mesh_node);
		model->keyframes.push_back(Indigo::KeyFrame());
		model->rotation = new Indigo::MatrixRotation();
		model->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, mat));

		root->addChildNode(model);
	}

	{
		//==================== Create ground geometry =========================
		Indigo::SceneNodeMeshRef mesh_node(new Indigo::SceneNodeMesh());
		mesh_node->setName("Ground Mesh");
		mesh_node->mesh = Indigo::MeshRef(new Indigo::Mesh());

		// Make a single quad
		Indigo::Quad q;
		q.mat_index = 0;
		for(int i = 0; i < 4; ++i)
			q.vertex_indices[i] = q.uv_indices[i] = i;
		mesh_node->mesh->quads.push_back(q);

		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(-2, -2, 0));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(-2, 2, 0));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(2, 2, 0));
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(2, -2, 0));

		// Make a UV mapping for the quad
		mesh_node->mesh->num_uv_mappings = 1;

		mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(0, 0));
		mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(0, 1));
		mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(1, 1));
		mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(1, 0));

		mesh_node->mesh->endOfModel(); // Build the mesh

		root->addChildNode(mesh_node);


		//==================== Create the ground material =========================
		Indigo::SceneNodeMaterialRef mat(new Indigo::SceneNodeMaterial());

		Indigo::DiffuseMaterial* diffuse = new Indigo::DiffuseMaterial();
		diffuse->random_triangle_colours = false;
		diffuse->layer = 0;
		diffuse->albedo = new Indigo::TextureWavelengthDependentParam(
			0 // texture index
		);

		diffuse->textures.push_back(Indigo::Texture());
		diffuse->textures.back().path = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/indigo_dll_example_vs2010/ColorChecker_sRGB_from_Ref.jpg";
		diffuse->textures.back().tex_coord_generation = new Indigo::UVTexCoordGenerator();

		mat->material = diffuse;
		mat->setName("Ground diffuse material");

		root->addChildNode(mat);

		if(false)
		{
			// Example code to create a blend material instead:
			Indigo::SceneNodeMaterialRef mat_a(new Indigo::SceneNodeMaterial());
			Indigo::DiffuseMaterial* diffuse_a = new Indigo::DiffuseMaterial();
			mat_a->material = diffuse_a;
			mat_a->setName("Child mat A");
			diffuse_a->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(1,0,0), 2.2));
			root->addChildNode(mat_a);

			Indigo::SceneNodeMaterialRef mat_b(new Indigo::SceneNodeMaterial());
			Indigo::DiffuseMaterial* diffuse_b = new Indigo::DiffuseMaterial();
			mat_b->material = diffuse_b;
			mat_b->setName("Child mat B");
			diffuse_b->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(0,1,0), 2.2));
			root->addChildNode(mat_b);

			Indigo::SceneNodeMaterialRef blend_node(new Indigo::SceneNodeMaterial());
			Indigo::BlendMaterial* blend = new Indigo::BlendMaterial();
			blend_node->setName("Blend mat");
			blend_node->material = blend;
			blend->blend = new Indigo::ConstantDisplacementParam(0.5);
			blend->a_mat = mat_a;
			blend->b_mat = mat_b;

			root->addChildNode(blend_node);
		}


		//==================== Create the ground object =========================
		Indigo::SceneNodeModelRef model(new Indigo::SceneNodeModel());
		model->setName("Ground Object");
		model->setGeometry(mesh_node);
		model->keyframes.push_back(Indigo::KeyFrame());
		model->rotation = new Indigo::MatrixRotation();
		model->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, mat));

		root->addChildNode(model); // Add node to scene graph.
	}

	return root;
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
{}
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

	//// Get GPU Info and Hardware ID.  This is just here for demonstration purposes and is not needed to start the render.
	//{
	//	Indigo::HardwareInfoRef hw_info(new Indigo::HardwareInfo(context));

	//	//
	//	// Get GPU Info.
	//	Indigo::Handle<Indigo::GPUInfoList> GPUinfos = hw_info->queryGPUInfo();

	//	// Do something with it.
	//	Indigo::Vector<Indigo::GPUInfo> gpu_info_copy = GPUinfos->getGPUInfos();

	//	//
	//	// Get hardware ID
	//	Indigo::Handle<Indigo::HardwareIdentifier> hardwareID = hw_info->getHardwareIdentifier();
	//	
	//	// Hardware ID is empty. There was an error.
	//	if(hardwareID->getHardwareID().empty())
	//	{
	//		const Indigo::String error_string = hardwareID->getError();
	//	}
	//	else
	//	{
	//		// Great success.
	//		const Indigo::String hw_id_string = hardwareID->getHardwareID();
	//	}
	//}
	
	// Create the renderer
	//Indigo::RendererRef renderer(new Indigo::Renderer(context));
	rendererRef = Indigo::RendererRef(new Indigo::Renderer(context));

	// Create and build the scene graph
	Indigo::SceneNodeRootRef root = createSceneGraph();

	// Save the scene graph to an IGS on disk so we can look at it in the Indigo GUI.
	try
	{
		root->writeToXMLFileOnDisk("example.igs", true, NULL);
	}
	catch(Indigo::IndigoException& e)
	{
		std::cerr << toStdString(e.what()) << std::endl;
		return;
	}

	// Create the main render buffer
	Indigo::RenderBufferRef render_buffer(new Indigo::RenderBuffer(root));

	// Create the low dynamic range (LDR) buffer
	Indigo::UInt8BufferRef uint8_buffer(new Indigo::UInt8Buffer());

	// Create the floating point buffer.  This buffer is optional, and only needs to be non-null if you want to 
	// have the resulting tone-mapped image stored in a floating-point format.
	this->floatBufferRef = Indigo::FloatBufferRef(new Indigo::FloatBuffer());

	// Create a tone mapper object.
	this->toneMapperRef = Indigo::ToneMapperRef(new Indigo::ToneMapper(context, render_buffer, uint8_buffer, floatBufferRef, 2.2f));
	this->toneMapperRef->update(root->getToneMapping(), root->getRendererSettings(), root->getCamera());


	// Build the scene from the scene graph.
	Indigo::Vector<Indigo::String> command_line_args;
	command_line_args.push_back("Dummy_scene_path");

	try
	{
		root->finalise(command_line_args[0]);
	}
	catch(Indigo::IndigoException& e)
	{
		std::cerr << "finalise() failed: " << toStdString(e.what()) << std::endl;
		return;
	}

	result = rendererRef->initialiseWithScene(root, render_buffer, command_line_args); // Non-blocking.
	if(result != Indigo::INDIGO_SUCCESS)
	{
		std::cerr << "Renderer failed to initialise(), error code: " << result << std::endl;
		return;
	}


	// Start rendering. This call will create the render threads, and start the rendering process.
	// This will happen after the scene building completes.
	rendererRef->startRendering(); // Non-blocking.

	// Let the Indigo DLL render threads do their thing for a while, while this thread sleeps.
	for(int i = 0; i < 5; ++i)
	{
		Sleep(1000);

		std::cout << "render time elapsed: " << rendererRef->getRenderTimeElapsed() << " s  (samples/pixel: " << rendererRef->getSamplesPerPixel() << ")" << std::endl;

		// Poll for messages coming back from the Indigo DLL
		Indigo::Vector<Indigo::Handle<Indigo::MessageInterface> > messages;
		rendererRef->getMessages(messages);

		for(size_t m = 0; m < messages.size(); ++m)
		{
			Indigo::Handle<Indigo::MessageInterface> message = messages[m];
			Indigo::MessageInterface::Type msg_type = message->getType();

			// Switch on the message type
			switch(msg_type)
			{
				case Indigo::MessageInterface::ERROR_MESSAGE: // The Indigo DLL ran into an error
				{
					// NOTE: not using dynamic cast here, because RTTI doesn't work well across DLL boundaries. So just use static_cast instead.
					std::cerr << "\tERROR: " << toStdString(static_cast<Indigo::ErrorMessageInterface*>(message.getPointer())->getMessage()) << std::flush;

					// Plugin authors could pop up a message box instead of exiting.
					return;
					break;
				}
				case Indigo::MessageInterface::LOG_MESSAGE:
				{
					std::cout << "LOG: " << toStdString(static_cast<Indigo::LogMessageInterface*>(message.getPointer())->getMessage()) << std::flush;
					break;
				}
				case Indigo::MessageInterface::PROGRESS_MESSAGE:
				{
					Indigo::ProgressMessageInterface* progress_message = static_cast<Indigo::ProgressMessageInterface*>(message.getPointer());

					if(progress_message->getProgressFraction() == 1.0)
						std::cout << "Renderer successfully initialised, starting render..." << std::endl;

					break;
				}
			}
		}
	}
	

	// Start tone-mapping. The actual tone-mapping work will be done in another thread.
	this->toneMapperRef->startToneMapping();

	// Wait until the tone-mapping has been completed.
	while(!this->toneMapperRef->isToneMappingDone())
	{
		std::cout << "Waiting for tone mapping ..." << std::endl;
		Sleep(50);
	}


	// Write the tone-mapped image to disk.
	MString imgName = this->mtin_renderGlobals->getImageOutputFile();
	writeUInt8BufferToBMP((imgName + ".bmp").asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
	writeFloatBufferToBMP((imgName + "fromFloat.bmp").asChar(), floatBufferRef->dataPtr(), floatBufferRef->width(), floatBufferRef->height());

	std::cout << "Render written to render.bmp." << std::endl;


	//
	// Realtime scene change example.
	// We will make a realtime change to the textured quad on the ground, changing it to a constant grey diffuse colour.
	//
	{
		// The scene graph must be locked when being changed, so the core can't read from it while it is being written to.
		// The lock will be unlocked once it goes out of scope.
		Indigo::Lock lock(root->getMutex());

		// Get a vector of all the materials in the scene
		Indigo::Vector<Indigo::SceneNodeMaterialRef> materials;
		root->getAllMaterials(materials);

		// Get the textured material.
		Indigo::SceneNodeMaterialRef mat = materials[1]; // We know it's the second material.

		// Check the type of the material
		if(mat->material->getType() == Indigo::Material::Diffuse)
		{
			Reference<Indigo::DiffuseMaterial> diffuse_mat = mat->material.downcast<Indigo::DiffuseMaterial>();

			// Change to a constant colour albedo.
			diffuse_mat->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(0.8), 2.2));
		}

		// Set the material as dirty and update the sequence number for the change.  This tells the render core that the material needs to be reloaded and rebuilt.
		mat->setDirtyFlags(Indigo::SceneNode::IsDirty);
		mat->updateSequenceNumber();
	}


	// Tell the core to update the scene.
	rendererRef->updateScene(root);

	// Wait until we get a 'UPDATE_DONE' message back from the core.
	bool update_done = false;

	while(!update_done)
	{
		Sleep(50);

		// Poll for messages coming back from the Indigo DLL
		Indigo::Vector<Indigo::Handle<Indigo::MessageInterface> > messages;
		rendererRef->getMessages(messages);

		for(size_t m = 0; m < messages.size(); ++m)
		{
			Indigo::Handle<Indigo::MessageInterface> message = messages[m];
			Indigo::MessageInterface::Type msg_type = message->getType();

			// Switch on the message type
			switch(msg_type)
			{
				case Indigo::MessageInterface::UPDATE_DONE: // The core is done updating the scene
				{
					Indigo::UpdateDoneMessageInterface* update_done_message = static_cast<Indigo::UpdateDoneMessageInterface*>(message.getPointer());

					// Reset all the dirty flags in the scene graph with the last handled sequence number that came back from the core.
					{
						// Lock the scene
						Indigo::Lock lock(root->getMutex());
						
						root->resetAllDirtyFlags(update_done_message->getSequenceNumber());
					}

					update_done = true;

					break;
				}
			}
		}
	}

	// Sleep for a while to give the updated scene a chance to render.
	Sleep(4000);


	// Start tone-mapping. The actual tone-mapping work will be done in another thread.
	this->toneMapperRef->startToneMapping();

	// Wait until the tone-mapping has been completed.
	while(!this->toneMapperRef->isToneMappingDone())
	{
		std::cout << "Waiting for tone mapping ..." << std::endl;
		Sleep(50);
	}

	// Write the tone-mapped image to disk.
	writeUInt8BufferToBMP((imgName + "V2.bmp").asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
	writeFloatBufferToBMP((imgName + "V2_float.bmp").asChar(), floatBufferRef->dataPtr(), floatBufferRef->width(), floatBufferRef->height());

	std::cout << "Render written to render2.bmp." << std::endl;




	//
	// Do another realtime scene change example.
	// We will make a realtime change to the textured quad on the ground, changing it to a red and green blend material read from the material file 'blend.igm'.
	//
	{
		// Load the material nodes from 'blend.igm' into the scene graph.
		Indigo::Vector<Indigo::SceneNodeRef> loaded_igm_nodes;
		Indigo::MaterialSerialisation mat_serialisation(context);
		try
		{
			mat_serialisation.loadMaterialsIntoSceneGraph("blend.igm", root, loaded_igm_nodes);
			// The scene graph must be locked when being changed, so the core can't read from it while it is being written.
			// The lock will be unlocked once it goes out of scope.
			Indigo::Lock lock(root->getMutex());

			// Find our ground object.  We know it's the last SceneNodeModel added to the scene.
			Indigo::SceneNodeModelRef ground_object;
			for(size_t z=0; z<root->getChildren().size(); ++z)
				if(root->getChildren()[z]->getNodeType() == Indigo::SceneNode::SceneNodeModel)
					ground_object = root->getChildren()[z].downcast<Indigo::SceneNodeModel>();

			if(ground_object.isNull())
			{
				std::cerr << "Failed to find ground object." << std::endl;
				return;
			}

			// The blend.igm material file will have a few nodes in it - 2 child materials and the parent/root material.
			// The parent material will be last in the file, so we want to assign that one to the ground plane.
			// So we will use loaded_igm_nodes.back() to get it.
			if(loaded_igm_nodes.size() > 0 && loaded_igm_nodes.back()->getNodeType() == Indigo::SceneNode::SceneNodeMaterial) // Check it has type SceneNodeMaterial.
			{
				Indigo::SceneNodeMaterialRef new_mat = loaded_igm_nodes.back().downcast<Indigo::SceneNodeMaterial>();
		
				// Update the material on the ground object to be our newly loaded material.
				// In general objects/models have a vector of materials, so we have to pass in a vector here.
				ground_object->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, new_mat));
			}

			// Set the ground_object as dirty and update the sequence number for the change.
			ground_object->setDirtyFlags(Indigo::SceneNode::IsDirty);
			ground_object->updateSequenceNumber();

		}
		catch(Indigo::IndigoException& e)
		{
			std::cerr << toStdString(e.what()) << std::endl;
		}		

	}


	// Tell the core to update the scene (again!)
	rendererRef->updateScene(root);

	// Wait until we get a 'UPDATE_DONE' message back from the core.
	update_done = false;

	while(!update_done)
	{
		Sleep(50);

		// Poll for messages coming back from the Indigo DLL
		Indigo::Vector<Indigo::Handle<Indigo::MessageInterface> > messages;
		rendererRef->getMessages(messages);

		for(size_t m = 0; m < messages.size(); ++m)
		{
			Indigo::Handle<Indigo::MessageInterface> message = messages[m];
			Indigo::MessageInterface::Type msg_type = message->getType();

			// Switch on the message type
			switch(msg_type)
			{
				case Indigo::MessageInterface::ERROR_MESSAGE: // The Indigo DLL ran into an error
				{
					// NOTE: not using dynamic cast here, because RTTI doesn't work well across DLL boundaries. So just use static_cast instead.
					std::cerr << "\tERROR: " << toStdString(static_cast<Indigo::ErrorMessageInterface*>(message.getPointer())->getMessage()) << std::flush;
					break;
				}
				case Indigo::MessageInterface::LOG_MESSAGE:
				{
					std::cout << "LOG: " << toStdString(static_cast<Indigo::LogMessageInterface*>(message.getPointer())->getMessage()) << std::flush;
					break;
				}
				case Indigo::MessageInterface::UPDATE_DONE: // The core is done updating the scene
				{
					Indigo::UpdateDoneMessageInterface* update_done_message = static_cast<Indigo::UpdateDoneMessageInterface*>(message.getPointer());

					// Reset all the dirty flags in the scene graph with the last handled sequence number that came back from the core.
					{
						// Lock the scene
						Indigo::Lock lock(root->getMutex());
						
						root->resetAllDirtyFlags(update_done_message->getSequenceNumber());
					}

					update_done = true;

					break;
				}
			}
		}
	}

	
	// Sleep for a while to give the updated scene a chance to render.
	Sleep(4000);


	// Stop the render. This call is optional, render will be stopped before indigo object is destroyed anyway.
	rendererRef->stopRendering();

	// Start tone-mapping. The actual tone-mapping work will be done in another thread.
	this->toneMapperRef->startToneMapping();

	// Wait until the tone-mapping has been completed.
	while(!this->toneMapperRef->isToneMappingDone())
	{
		std::cout << "Waiting for tone mapping ..." << std::endl;
		Sleep(50);
	}


	// Write the tone-mapped image to disk.
	writeUInt8BufferToBMP((imgName + "V3.bmp").asChar(), uint8_buffer->dataPtr(), uint8_buffer->width(), uint8_buffer->height());
	writeFloatBufferToBMP((imgName + "V3_float.bmp").asChar(), floatBufferRef->dataPtr(), floatBufferRef->width(), floatBufferRef->height());

	std::cout << "Render written to render3.bmp." << std::endl;


	std::cout << "Finished." << std::endl;

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

