#include "appleseedSwatchRenderer.h"

#include "foundation/core/appleseed.h"
#include "renderer/api/bsdf.h"
#include "renderer/api/camera.h"
#include "renderer/api/color.h"
#include "renderer/api/environment.h"
#include "renderer/api/frame.h"
#include "renderer/api/light.h"
#include "renderer/api/log.h"
#include "renderer/api/material.h"
#include "renderer/api/object.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"
#include "renderer/api/scene.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/image/image.h"
#include "foundation/image/tile.h"
#include "foundation/math/matrix.h"
#include "foundation/math/scalar.h"
#include "foundation/math/transform.h"
#include "foundation/utility/containers/specializedarrays.h"
#include "foundation/utility/log/consolelogtarget.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/searchpaths.h"

#include "renderer/global/globallogger.h"
#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

#include "renderer/modeling/shadergroup/shadergroup.h"

#include "SwatchesEvent.h"
#include "swatchesRenderer/swatchesQueue.h"
#include "swatchesRenderer/newSwatchRenderer.h"
#include "swatchesRenderer/swatchGeometry.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include <thread>
#include "OSL/oslUtils.h"
#include "shadingTools/material.h"
#include "shadingTools/shadingUtils.h"

namespace asf = foundation;
namespace asr = renderer;

AppleseedSwatchRenderer::AppleseedSwatchRenderer()
{
	Logging::debug(MString("Initialze appleseed swatch renderer."));
#if _DEBUG
	//log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());
#endif

	MString swatchRenderFile = "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swatchRender.xml";
	MString schemaPath = getRendererHome() + "/schemas/project.xsd";
	project = asr::ProjectFileReader().read(swatchRenderFile.asChar(), schemaPath.asChar());
	if (project.get() == nullptr)
	{}
	else{
		Logging::info(MString("Successfully loaded swatch render file."));
	}
	
	mrenderer = std::auto_ptr<asr::MasterRenderer>(new asr::MasterRenderer(
		project.ref(),
		project->configurations().get_by_name("final")->get_inherited_parameters(),
		&renderer_controller));
}

void AppleseedSwatchRenderer::renderSwatch()
{
	mrenderer->render();
	//MString tstFile = "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swatchRender.jpg";
	//project->get_frame()->write_main_image(tstFile.asChar());
	//asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swOutputTest.appleseed");
}

void AppleseedSwatchRenderer::renderSwatch(NewSwatchRenderer *sr)
{

	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	int res(sr->resolution());
	this->setSize(res);
	this->setShader(sr->dNode);
	mrenderer->render();
	MString tstFile = "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swatchRender.exr";
	//project->get_frame()->write_main_image(tstFile.asChar());
	//asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swOutputTest.appleseed");

	//project->get_frame()->transform_to_output_color_space(project->get_frame()->image());

	sr->image().create(res, res, 4, MImage::kFloat);
	float *floatPixels = sr->image().floatPixels();
	this->fillSwatch(floatPixels);

	//float rndR = rnd();
	//float rndG = rnd();
	//float rndB = rnd();

	//float *pixels = sr->image().floatPixels();
	//int index = 0;
	//for (int y = 0; y < res; y++)
	//{
	//	for (int x = 0; x < res; x++)
	//	{
	//		float fac = float(y) / res;
	//		pixels[index++] = fac * rndR;
	//		pixels[index++] = fac * rndG;
	//		pixels[index++] = fac * rndB;
	//		pixels[index++] = 1.0f;
	//	}
	//}

}

void AppleseedSwatchRenderer::fillSwatch(float *pixels)
{
	asf::Image& image = project->get_frame()->image();
	Logging::info(MString("m_canvas_height:") + image.properties().m_canvas_height);
	Logging::info(MString("m_rcp_canvas_height:") + image.properties().m_rcp_canvas_height);
	Logging::info(MString("m_tile_height:") + image.properties().m_tile_height);
	int res = image.properties().m_canvas_height;
	int chCount = image.properties().m_channel_count;

	//int res = image.properties().m_rcp_canvas_height;
	//Logging::info(MString("Numtiles: expected 1 real:") + image.properties().m_tile_count);

	//float rndR = rnd();
	//float rndG = rnd();
	//float rndB = rnd();

	//int index = 0;
	//for (int y = 0; y < res; y++)
	//{
	//	for (int x = 0; x < res; x++)
	//	{
	//		float fac = float(y) / res;
	//		pixels[index++] = fac * rndR;
	//		pixels[index++] = fac * rndG;
	//		pixels[index++] = fac * rndB;
	//		pixels[index++] = 1.0f;
	//	}
	//}

	uint index = 0;
	for (int y = 0; y < res; y++)
	{
		for (int x = 0; x < res; x++)
		{
			asf::Color4f p;
			image.get_pixel(x, y, p);

			pixels[index++] = p.r;
			pixels[index++] = p.g;
			pixels[index++] = p.g;
			pixels[index++] = 1.0f;
		}
	}
}

void AppleseedSwatchRenderer::setSize(int size)
{
	MString res = MString("") + size + " " + size;
	asr::ParamArray frameParams = project->get_frame()->get_parameters();
	frameParams.insert("resolution", res.asChar());
	frameParams.insert("tile_size", size);
	project->set_frame(
		asr::FrameFactory::create(
		"beauty",
		frameParams));
}

void AppleseedSwatchRenderer::setShader(MObject shader)
{
	this->defineMaterial(shader);
}

AppleseedSwatchRenderer::~AppleseedSwatchRenderer()
{
	terminateAppleseedSwatchRender(this);
	project.release();
#if _DEBUG
	asr::global_logger().remove_target(log_target.get());
#endif
	Logging::debug("Removing AppleseedSwatchRenderer.");
}

void AppleseedSwatchRenderer::mainLoop()
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif

	SQueue::SEvent swatchEvent;
	Logging::debug("Starting AppleseedSwatchRenderer main Loop.");
	while (!terminateLoop)
	{
		SQueue::getQueue()->wait_and_pop(swatchEvent);
		Logging::debug(MString("AppleseedSwatchRenderer main Loop: renderDoneAddr: ") + (int)swatchEvent.renderDone);

		if (swatchEvent.renderDone == nullptr)
		{
			Logging::debug(MString("AppleseedSwatchRenderer main Loop: received a null ptr. Terminating loop"));
			terminateLoop = true;
		}
		else{
			//this->setShader(swatchEvent.shadingNode);
			//this->setSize(swatchEvent.height);
			//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			//this->renderSwatch();
			//this->fillSwatch(swatchEvent.pixels);
			swatchEvent.swatchRenderer->finishParallelRender();
			*swatchEvent.renderDone = true;
		}
	}
	loopDone = true;
}


void AppleseedSwatchRenderer::defineMaterial(MObject shadingNode)
{
	MStatus status;
	asf::StringArray materialNames;
	MAYATO_OSLUTIL::OSLUtilClass oslClass;
	//MAYATO_OSL::initOSLUtil();
	MString shaderGroupName = "previewSG";
	MString surfaceShader;
	MObject surfaceShaderNode = shadingNode;
	MString surfaceShaderName = getObjectName(surfaceShaderNode);
	ShadingNetwork network(surfaceShaderNode);
	size_t numNodes = network.shaderList.size();

	asr::Assembly *assembly = project->get_scene()->assemblies().get_by_name("sceneAssembly")->assemblies().get_by_name("world");
	assert(assembly != nullptr);

	asr::ShaderGroup *existingShaderGroup = assembly->shader_groups().get_by_name(shaderGroupName.asChar());
	existingShaderGroup->clear();	
	oslClass.group = (OSL::ShaderGroup *)existingShaderGroup;

	//MPlug shaderPlug = shadingGroupNode.findPlug("surfaceShader");
	//MAYATO_OSL::createOSLProjectionNodes(shaderPlug);

	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = network.shaderList[shadingNodeId];
		Logging::debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		if (shadingNodeId == (numNodes - 1))
			Logging::debug(MString("LastNode Surface Shader: ") + snode.fullName);
		oslClass.createOSLHelperNodes(network.shaderList[shadingNodeId]);
		oslClass.createOSLShadingNode(network.shaderList[shadingNodeId]);
		oslClass.connectProjectionNodes(network.shaderList[shadingNodeId].mobject);
	}
	if (numNodes > 0)
	{
		ShadingNode snode = network.shaderList[numNodes - 1];
		MString layer = (snode.fullName + "_interface");
		Logging::debug(MString("Adding interface shader: ") + layer);
		existingShaderGroup->add_shader("surface", "surfaceShaderInterface", layer.asChar(), asr::ParamArray());
		const char *srcLayer = snode.fullName.asChar();
		const char *srcAttr = "outColor";
		const char *dstLayer = layer.asChar();
		const char *dstAttr = "inColor";
		Logging::debug(MString("Connecting interface shader: ") + srcLayer + "." + srcAttr + " -> " + dstLayer + "." + dstAttr);
		existingShaderGroup->add_connection(srcLayer, srcAttr, dstLayer, dstAttr);
	}
}

void AppleseedSwatchRenderer::startAppleseedSwatchRender(AppleseedSwatchRenderer *swRend)
{
	if (swRend != nullptr)
	{
		Logging::debug(MString("startAppleseedSwatchRender"));
		swRend->mainLoop();
		delete swRend;
		Logging::debug(MString("appleseedSwatchRender done and deleted."));
	}
}

void AppleseedSwatchRenderer::terminateAppleseedSwatchRender(AppleseedSwatchRenderer *swRend)
{
	Logging::debug(MString("terminateAppleseedSwatchRender"));
	SQueue::SEvent swatchEvent;
	SQueue::SwatchesQueue.push(swatchEvent);
	swRend->terminateLoop = true;
	while (!swRend->loopDone)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
}




//// Create an empty project.
//project = asr::ProjectFactory::create("SwatchRenderer project");
//project->add_default_configurations();
//project->configurations().get_by_name("final");
//project->configurations()
//	.get_by_name("final")->get_parameters()
//	.insert_path("uniform_pixel_renderer.samples", "10");

//// Create a scene.
//scene = asr::SceneFactory::create();

//// Create an assembly.
//asf::auto_release_ptr<asr::Assembly> assembly(
//	asr::AssemblyFactory().create(
//	"assembly",
//	asr::ParamArray()));

//static const float GrayReflectance[] = { 0.5f, 0.5f, 0.5f };
//assembly->colors().insert(
//	asr::ColorEntityFactory::create(
//	"gray",
//	asr::ParamArray()
//	.insert("color_space", "srgb"),
//	asr::ColorValueArray(3, GrayReflectance)));

//assembly->bsdfs().insert(
//	asr::LambertianBRDFFactory().create(
//	"diffuse_gray_brdf",
//	asr::ParamArray()
//	.insert("reflectance", "gray")));

//assembly->surface_shaders().insert(
//	asr::PhysicalSurfaceShaderFactory().create(
//	"physical_surface_shader",
//	asr::ParamArray()));

//assembly->materials().insert(
//	asr::GenericMaterialFactory().create(
//	"gray_material",
//	asr::ParamArray()
//	.insert("surface_shader", "physical_surface_shader")
//	.insert("bsdf", "diffuse_gray_brdf")));

//asr::MeshObjectArray objects;
//asr::MeshObjectReader::read(
//	project->search_paths(),
//	"cube",
//	asr::ParamArray()
//	.insert("filename", "h:/temp/as/scene.obj"),
//	objects);

//// Insert all the objects into the assembly.
//for (size_t i = 0; i < objects.size(); ++i)
//{
//	// Insert this object into the scene.
//	asr::MeshObject* object = objects[i];
//	assembly->objects().insert(asf::auto_release_ptr<asr::Object>(object));

//	// Create an instance of this object and insert it into the assembly.
//	const std::string instance_name = std::string(object->get_name()) + "_inst";
//	assembly->object_instances().insert(
//		asr::ObjectInstanceFactory::create(
//		instance_name.c_str(),
//		asr::ParamArray(),
//		object->get_name(),
//		asf::Transformd::identity(),
//		asf::StringDictionary()
//		.insert("default", "gray_material")
//		.insert("default2", "gray_material")));
//}

//asf::auto_release_ptr<asr::MeshObject> bg(asr::MeshObjectFactory::create("BG", asr::ParamArray()));
//asf::auto_release_ptr<asr::MeshObject> meshLight(asr::MeshObjectFactory::create("meshLight", asr::ParamArray()));
//asf::auto_release_ptr<asr::MeshObject> sphere(asr::MeshObjectFactory::create("sphere", asr::ParamArray()));

//object->push_vertex(asr::GVector3(-1.0f, 0.0f, -1.0f));
//object->push_vertex(asr::GVector3(-1.0f, 0.0f, 1.0f));
//object->push_vertex(asr::GVector3(1.0f, 0.0f, 1.0f));
//object->push_vertex(asr::GVector3(1.0f, 0.0f, -1.0f));

//// Vertex normals.
//object->push_vertex_normal(asr::GVector3(0.0f, 1.0f, 0.0f));

//object->push_tex_coords(asr::GVector2(0.0, 0.0));
//object->push_tex_coords(asr::GVector2(1.0, 0.0));
//object->push_tex_coords(asr::GVector2(1.0, 1.0));
//object->push_tex_coords(asr::GVector2(0.0, 1.0));

//// Triangles.
//object->push_triangle(asr::Triangle(0, 1, 2, 0, 0, 0, 0, 1, 2, 0));
//object->push_triangle(asr::Triangle(0, 2, 3, 0, 0, 0, 0, 2, 3, 0));


////------------------------------------------------------------------------
//// Light
////------------------------------------------------------------------------

//// Create a color called "light_intensity" and insert it into the assembly.
//static const float LightRadiance[] = { 1.0f, 1.0f, 1.0f };
//assembly->colors().insert(
//	asr::ColorEntityFactory::create(
//	"light_intensity",
//	asr::ParamArray()
//	.insert("color_space", "srgb")
//	.insert("multiplier", "30.0"),
//	asr::ColorValueArray(3, LightRadiance)));

//// Create a point light called "light" and insert it into the assembly.
//asf::auto_release_ptr<asr::Light> light(
//	asr::PointLightFactory().create(
//	"light",
//	asr::ParamArray()
//	.insert("intensity", "light_intensity")));
//light->set_transform(
//	asf::Transformd::from_local_to_parent(
//	asf::Matrix4d::translation(asf::Vector3d(0.6, 2.0, 1.0))));
//assembly->lights().insert(light);

////------------------------------------------------------------------------
//// Assembly instance
////------------------------------------------------------------------------

//// Create an instance of the assembly and insert it into the scene.
//asf::auto_release_ptr<asr::AssemblyInstance> assembly_instance(
//	asr::AssemblyInstanceFactory::create(
//	"assembly_inst",
//	asr::ParamArray(),
//	"assembly"));
//assembly_instance
//	->transform_sequence()
//	.set_transform(
//	0.0,
//	asf::Transformd::identity());
//scene->assembly_instances().insert(assembly_instance);

//// Insert the assembly into the scene.
//scene->assemblies().insert(assembly);

////------------------------------------------------------------------------
//// Environment
////------------------------------------------------------------------------

//// Create a color called "sky_radiance" and insert it into the scene.
//static const float SkyRadiance[] = { 0.75f, 0.80f, 1.0f };
//scene->colors().insert(
//	asr::ColorEntityFactory::create(
//	"sky_radiance",
//	asr::ParamArray()
//	.insert("color_space", "srgb")
//	.insert("multiplier", "0.5"),
//	asr::ColorValueArray(3, SkyRadiance)));

//// Create an environment EDF called "sky_edf" and insert it into the scene.
//scene->environment_edfs().insert(
//	asr::ConstantEnvironmentEDFFactory().create(
//	"sky_edf",
//	asr::ParamArray()
//	.insert("radiance", "sky_radiance")));

//// Create an environment shader called "sky_shader" and insert it into the scene.
//scene->environment_shaders().insert(
//	asr::EDFEnvironmentShaderFactory().create(
//	"sky_shader",
//	asr::ParamArray()
//	.insert("environment_edf", "sky_edf")));

//// Create an environment called "sky" and bind it to the scene.
//scene->set_environment(
//	asr::EnvironmentFactory::create(
//	"sky",
//	asr::ParamArray()
//	.insert("environment_edf", "sky_edf")
//	.insert("environment_shader", "sky_shader")));

////------------------------------------------------------------------------
//// Camera
////------------------------------------------------------------------------

//// Create a pinhole camera with film dimensions 0.980 x 0.735 in (24.892 x 18.669 mm).
//asf::auto_release_ptr<asr::Camera> camera(
//	asr::PinholeCameraFactory().create(
//	"camera",
//	asr::ParamArray()
//	.insert("film_dimensions", "0.024892 0.018669")
//	.insert("focal_length", "0.035")));

//// Place and orient the camera. By default cameras are located in (0.0, 0.0, 0.0)
//// and are looking toward Z- (0.0, 0.0, -1.0).
//camera->transform_sequence().set_transform(
//	0.0,
//	asf::Transformd::from_local_to_parent(
//	asf::Matrix4d::rotation(asf::Vector3d(1.0, 0.0, 0.0), asf::deg_to_rad(-20.0)) *
//	asf::Matrix4d::translation(asf::Vector3d(0.0, 0.8, 11.0))));

//// Bind the camera to the scene.
//scene->set_camera(camera);

////------------------------------------------------------------------------
//// Frame
////------------------------------------------------------------------------

//// Create a frame and bind it to the project.
//project->set_frame(
//	asr::FrameFactory::create(
//	"beauty",
//	asr::ParamArray()
//	.insert("camera", scene->get_camera()->get_name())
//	.insert("resolution", "640 480")
//	.insert("color_space", "srgb")));

//// Bind the scene to the project.
//project->set_scene(scene);

