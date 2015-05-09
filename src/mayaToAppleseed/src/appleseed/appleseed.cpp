#include "appleseed.h"
#include "mtap_tileCallback.h"

#include "renderer/global/globallogger.h"
#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

#include "renderer/modeling/environmentedf/sphericalcoordinates.h"

#include "maya/MFnDependencyNode.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "threads/threads.h"
#include "threads/queue.h"
#include "utilities/pystring.h"
#include "../mtap_common/mtap_mayaScene.h"
#include "threads/renderQueueWorker.h"

#include <stdio.h>
#include <iostream>

#include "utilities/logging.h"

static Logging logger;

static int tileCount = 0;
static int tileCountTotal = 0;

using namespace AppleRender;

std::vector<asr::Entity *> definedEntities;

AppleseedRenderer::AppleseedRenderer()
{
	//log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());
}

AppleseedRenderer::~AppleseedRenderer()
{
	//asr::global_logger().remove_target(log_target.get());
}

void AppleseedRenderer::initializeRenderer()
{
	//RENDERER_LOG_INFO("%s", asf::Appleseed::get_synthetic_version_string());
	//definedEntities.clear();
	//this->project = asr::ProjectFactory::create("mtap_project");
	//project->add_default_configurations();
	//asf::auto_release_ptr<asr::Scene> sceneEntity = asr::SceneFactory::create();
	//project->set_scene(sceneEntity);
	//scene = project->get_scene();
	//defineConfig();
}

void AppleseedRenderer::unInitializeRenderer()
{
	//Logging::debug("Releasing scene");
	//this->scene.reset();
	//this->project->get_scene()->release();
	//Logging::debug("Releasing project");
	//this->project.release();
	//Logging::debug("Releasing done.");
}

void AppleseedRenderer::defineProject()
{
	//// Create an empty project.
	//project = asr::ProjectFactory::create("test project");
	//project->search_paths().push_back("data");

	//// Add default configurations to the project.
	//project->add_default_configurations();

	//// Set the number of samples. This is the main quality parameter: the higher the
	//// number of samples, the smoother the image but the longer the rendering time.
	//project->configurations()
	//	.get_by_name("final")->get_parameters()
	//	.insert_path("uniform_pixel_renderer.samples", "25");

	//// Create a scene.
	//asf::auto_release_ptr<asr::Scene> scene(asr::SceneFactory::create());

	//// Create an assembly.
	//asf::auto_release_ptr<asr::Assembly> assembly(
	//	asr::AssemblyFactory().create(
	//	"assembly",
	//	asr::ParamArray()));

	////------------------------------------------------------------------------
	//// Materials
	////------------------------------------------------------------------------

	//// Create a color called "gray" and insert it into the assembly.
	//static const float GrayReflectance[] = { 0.5f, 0.5f, 0.5f };
	//assembly->colors().insert(
	//	asr::ColorEntityFactory::create(
	//	"gray",
	//	asr::ParamArray()
	//	.insert("color_space", "srgb"),
	//	asr::ColorValueArray(3, GrayReflectance)));

	//// Create a BRDF called "diffuse_gray_brdf" and insert it into the assembly.
	//assembly->bsdfs().insert(
	//	asr::LambertianBRDFFactory().create(
	//	"diffuse_gray_brdf",
	//	asr::ParamArray()
	//	.insert("reflectance", "gray")));

	//// Create a physical surface shader and insert it into the assembly.
	//assembly->surface_shaders().insert(
	//	asr::PhysicalSurfaceShaderFactory().create(
	//	"physical_surface_shader",
	//	asr::ParamArray()));

	//// Create a material called "gray_material" and insert it into the assembly.
	//assembly->materials().insert(
	//	asr::GenericMaterialFactory().create(
	//	"gray_material",
	//	asr::ParamArray()
	//	.insert("surface_shader", "physical_surface_shader")
	//	.insert("bsdf", "diffuse_gray_brdf")));

	////------------------------------------------------------------------------
	//// Geometry
	////------------------------------------------------------------------------

	//// Load the scene geometry from disk.
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

}

//void AppleseedRenderer::defineProject()
//{
//	//TODO: the logging procedures of applesse causes a crash, no idea why.
//	//asf::auto_release_ptr<asf::LogTargetBase> log_target(asf::create_console_log_target(stdout));
//	//asr::global_logger().add_target(log_target.get());
//	//asr::global_logger().add_target(log_target.release());
//
//	//RENDERER_LOG_INFO("Testlog");
//
//	//fprintf(stdout, "-------------DassnTest---------: %d:",123);
//	//fflush(stdout);
//
//	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";
//	this->project->set_path(outputPath.asChar());
//	this->project->set_scene(asr::SceneFactory::create());
//	this->scenePtr = this->project->get_scene();
//
//	definedEntities.clear();
//}
//
//void AppleseedRenderer::definePreRender()
//{
//	this->defineProject();
//	this->defineConfig();
//	clearShaderAssemblyAssignments();
//}
//
//void AppleseedRenderer::writeXML()
//{
//	Logging::debug("AppleseedRenderer::writeXML");
//	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + "." + (int)this->renderGlobals->currentFrame + ".appleseed";
//	asr::ProjectFileWriter::write(this->project.ref(), outputPath.asChar());
//}
//
//
//void AppleseedRenderer::defineOutput()
//{
//	Logging::debug("AppleseedRenderer::defineOutput");
//    // Create a frame and bind it to the project.
//	MString res = MString("") + renderGlobals->imgWidth + " " + renderGlobals->imgHeight;
//	MString colorSpaceString = renderGlobals->colorSpaceString;
//	MString tileSize =  MString("") + renderGlobals->tilesize + " " + renderGlobals->tilesize;
//	
//    this->project->set_frame( 
//        asr::FrameFactory::create(
//            "beauty",
//            asr::ParamArray()
//                .insert("camera",this->scenePtr->get_camera()->get_name())
//				.insert("resolution", res.asChar())
//				.insert("tile_size", tileSize.asChar())
//				.insert("color_space", colorSpaceString.asChar())));
//
//}
//
//
//void AppleseedRenderer::updateEntitiesCaller()
//{
//	Logging::debug("AppleseedRenderer::updateEntities");
//	if(appleRenderer != nullptr)
//		appleRenderer->updateEntities();
//}
//
//
//
//bool AppleseedRenderer::initializeRenderer(mtap_RenderGlobals *renderGlobals,  std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList)
//{
//	return true;
//}
//
//void AppleseedRenderer::defineMasterAssembly()
//{
//	if( this->scenePtr->assemblies().get_by_name("world") == nullptr)
//	{
//		asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create( "world", asr::ParamArray());
//		this->scenePtr->assemblies().insert(assembly);			
//
//		asf::Matrix4d appMatrix;
//		MMatrix transformMatrix;
//		transformMatrix.setToIdentity();
//		transformMatrix *= this->renderGlobals->globalConversionMatrix;
//		asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create(
//			"world_assInst",
//			asr::ParamArray(),
//			"world");
//
//		this->MMatrixToAMatrix(transformMatrix, appMatrix);
//		assemblyInstance->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
//		this->scenePtr->assembly_instances().insert(assemblyInstance);
//	}
//
//	this->masterAssembly = this->scenePtr->assemblies().get_by_name("world");
//}
//
//void AppleseedRenderer::defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList, std::vector<MayaObject *>& instancerList)
//{
//	Logging::debug("AppleseedRenderer::defineScene");
//	// camera has to be defined here because the next command defineOutput will need a camera
//	this->updateCamera(true);
//	this->defineOutput();
//	this->defineEnvironment(renderGlobals);
//}
//
//
//void AppleseedRenderer::addDeformStep(std::shared_ptr<MayaObject> obj, asr::Assembly *assembly)
//{
//	MString geoName = obj->fullNiceName;
//	asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());
//
//	MStatus stat = MStatus::kSuccess;
//	MFnMesh meshFn(obj->mobject, &stat);
//	CHECK_MSTATUS(stat);
//	if( !stat )
//		return;
//	MItMeshPolygon faceIt(obj->mobject, &stat);
//	CHECK_MSTATUS(stat);
//	if( !stat )
//		return;
//
//	MPointArray points;
//	stat = meshFn.getPoints(points);
//	CHECK_MSTATUS(stat);
//    MFloatVectorArray normals;
//    meshFn.getNormals( normals, MSpace::kWorld );
//
//	Logging::debug(MString("Defining deform step for mesh object ") + obj->fullNiceName);
//	
//	asr::MeshObject *meshShape = (asr::MeshObject *)geoObject;
//	MString meshName = meshShape->get_name();
//	int numMbSegments = (int)meshShape->get_motion_segment_count();
//	Logging::debug(MString("Mesh name ") + meshName + " has " + numMbSegments + " motion segments");
//
//	// now adding one motion segment
//	size_t mbSegmentIndex = meshShape->get_motion_segment_count() + 1;
//	meshShape->set_motion_segment_count(mbSegmentIndex);
//
//	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
//	{
//		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
//		//Logging::debug(MString("update vtx0: ") + points[vtxId].x + " " + points[vtxId].y + " " + points[vtxId].z);
//
//		meshShape->set_vertex_pose(vtxId, mbSegmentIndex - 1, vtx);
//	}
//}
//
////
////	Puts Object (only mesh at the moment) into this assembly.
////
//
//void AppleseedRenderer::putObjectIntoAssembly(asr::Assembly *assembly, std::shared_ptr<MayaObject> obj)
//{
//	Logging::debug(MString("putObjectIntoAssembly obj: ") + obj->shortName);
//	MMatrix matrix;
//	matrix.setToIdentity();
//	putObjectIntoAssembly(assembly, obj, matrix);
//}
//
//void AppleseedRenderer::putObjectIntoAssembly(asr::Assembly *assembly, std::shared_ptr<MayaObject> obj, MMatrix matrix)
//{
//
//	asf::StringArray material_names;
//	// here the mesh per face assignments are read and placed into the obj->perFaceAssignments
//	this->defineObjectMaterial(renderGlobals, obj, material_names);
//
//	asr::MeshObjectArray meshArray;
//	bool isProxy = false;
//	this->createMesh(obj, meshArray, isProxy);
//
//	Logging::debug(MString("Found ") + meshArray.size() + " meshes.");
//	
//	for( size_t meshId = 0; meshId < meshArray.size(); meshId++)
//	{
//		MString meshName = meshArray[meshId]->get_name();
//		Logging::debug(MString("checking mesh ") + meshName);
//		assembly->objects().insert(asf::auto_release_ptr<asr::Object>(meshArray[meshId]));
//		asr::Object *meshObject = assembly->objects().get_by_name(meshName.asChar());
//	
//		asf::Matrix4d tmatrix = asf::Matrix4d::identity();
//		this->MMatrixToAMatrix(matrix, tmatrix);
//
//		asf::StringDictionary matDict = asf::StringDictionary();
//		asf::StringDictionary matBackDict = asf::StringDictionary();
//
//		int counterFront = 0;
//		int counterBack = 0;
//		if( isProxy )
//		{
//			Logging::debug(MString("mesh is proxy, getting material names from mesh."));
//			size_t numMat = meshObject->get_material_slot_count();
//			for( size_t i = 0; i < numMat; i++)
//			{						
//				matBackDict.insert(MString(MString("slot_") + counterBack++).asChar(), meshObject->get_material_slot(i));
//				matDict.insert(MString(MString("slot_") + counterFront++).asChar(), meshObject->get_material_slot(i));			
//			}
//		}else{
//			for( size_t i = 0; i < material_names.size(); i++)
//			{	
//				if( pystring::endswith(material_names[i], "_back") )
//					matBackDict.insert(MString(MString("slot_") + counterBack++).asChar(), material_names[i]);
//				else
//					matDict.insert(MString(MString("slot_") + counterFront++).asChar(), material_names[i]);			
//			}
//		}
//
//		bool doubleSided = true;
//		MFnDependencyNode depFn(obj->mobject);
//		getBool(MString("doubleSided"), depFn, doubleSided);
//
//		asr::ParamArray meshInstanceParams;
//
//		MString ray_bias_method = "none";
//		int id;
//		getEnum("mtap_ray_bias_method", depFn, id, ray_bias_method);
//		if( ray_bias_method != "none")
//		{
//			float ray_bias_distance = 0.0f;
//			getFloat("mtap_ray_bias_distance", depFn, ray_bias_distance);
//			meshInstanceParams.insert("ray_bias_method", ray_bias_method.asChar());
//			meshInstanceParams.insert("mtap_ray_bias_distance", ray_bias_distance);
//		}
//
//		if(doubleSided)
//		{
//			if( matBackDict.size() == 0)
//				matBackDict = matDict;
//
//			assembly->object_instances().insert(
//					asr::ObjectInstanceFactory::create(
//					(meshName + "_inst").asChar(),
//					meshInstanceParams,
//					meshObject->get_name(),
//					asf::Transformd::from_local_to_parent(tmatrix),
//					matDict,
//					matBackDict
//					));
//		}else
//			assembly->object_instances().insert(
//					asr::ObjectInstanceFactory::create(
//					(meshName + "_inst").asChar(),
//					asr::ParamArray(),
//					meshObject->get_name(),
//					asf::Transformd::from_local_to_parent(tmatrix),
//					matDict
//					));
//	}
//}
//	
//

void AppleseedRenderer::render()
{
	Logging::debug("AppleseedRenderer::render");

	// Create a log target that outputs to stderr, and binds it to the renderer's global logger.
	// Eventually you will probably want to redirect log messages to your own target. For this
	// you will need to implement foundation::ILogTarget (foundation/utility/log/ilogtarget.h).

	//std::auto_ptr<asf::ILogTarget> log_target(asf::create_console_log_target(stderr));
	//asr::global_logger().add_target(log_target.get());

	//// Print appleseed's version string.
	//RENDERER_LOG_INFO("%s", asf::Appleseed::get_synthetic_version_string());

	std::auto_ptr<asf::ILogTarget> log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	asr::global_logger().add_target(log_target.get());

	// Create an empty project.
	asf::auto_release_ptr<asr::Project> project = asr::ProjectFactory::create("test project");
	project->search_paths().push_back("data");

	// Add default configurations to the project.
	project->add_default_configurations();

	// Set the number of samples. This is the main quality parameter: the higher the
	// number of samples, the smoother the image but the longer the rendering time.
	project->configurations()
		.get_by_name("final")->get_parameters()
		.insert_path("uniform_pixel_renderer.samples", "25");

	// Create a scene.
	asf::auto_release_ptr<asr::Scene> scene(asr::SceneFactory::create());

	// Create an assembly.
	asf::auto_release_ptr<asr::Assembly> assembly(
		asr::AssemblyFactory().create(
		"assembly",
		asr::ParamArray()));

	//------------------------------------------------------------------------
	// Materials
	//------------------------------------------------------------------------

	// Create a color called "gray" and insert it into the assembly.
	static const float GrayReflectance[] = { 0.5f, 0.5f, 0.5f };
	assembly->colors().insert(
		asr::ColorEntityFactory::create(
		"gray",
		asr::ParamArray()
		.insert("color_space", "srgb"),
		asr::ColorValueArray(3, GrayReflectance)));

	// Create a BRDF called "diffuse_gray_brdf" and insert it into the assembly.
	assembly->bsdfs().insert(
		asr::LambertianBRDFFactory().create(
		"diffuse_gray_brdf",
		asr::ParamArray()
		.insert("reflectance", "gray")));

	// Create a physical surface shader and insert it into the assembly.
	assembly->surface_shaders().insert(
		asr::PhysicalSurfaceShaderFactory().create(
		"physical_surface_shader",
		asr::ParamArray()));

	// Create a material called "gray_material" and insert it into the assembly.
	assembly->materials().insert(
		asr::GenericMaterialFactory().create(
		"gray_material",
		asr::ParamArray()
		.insert("surface_shader", "physical_surface_shader")
		.insert("bsdf", "diffuse_gray_brdf")));

	//------------------------------------------------------------------------
	// Geometry
	//------------------------------------------------------------------------

	// Load the scene geometry from disk.
	asr::MeshObjectArray objects;
	asr::MeshObjectReader::read(
		project->search_paths(),
		"cube",
		asr::ParamArray()
		.insert("filename", "h:/temp/as/scene.obj"),
		objects);

	// Insert all the objects into the assembly.
	for (size_t i = 0; i < objects.size(); ++i)
	{
		// Insert this object into the scene.
		asr::MeshObject* object = objects[i];
		assembly->objects().insert(asf::auto_release_ptr<asr::Object>(object));

		// Create an instance of this object and insert it into the assembly.
		const std::string instance_name = std::string(object->get_name()) + "_inst";
		assembly->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			instance_name.c_str(),
			asr::ParamArray(),
			object->get_name(),
			asf::Transformd::identity(),
			asf::StringDictionary()
			.insert("default", "gray_material")
			.insert("default2", "gray_material")));
	}

	//------------------------------------------------------------------------
	// Light
	//------------------------------------------------------------------------

	// Create a color called "light_intensity" and insert it into the assembly.
	static const float LightRadiance[] = { 1.0f, 1.0f, 1.0f };
	assembly->colors().insert(
		asr::ColorEntityFactory::create(
		"light_intensity",
		asr::ParamArray()
		.insert("color_space", "srgb")
		.insert("multiplier", "30.0"),
		asr::ColorValueArray(3, LightRadiance)));

	// Create a point light called "light" and insert it into the assembly.
	asf::auto_release_ptr<asr::Light> light(
		asr::PointLightFactory().create(
		"light",
		asr::ParamArray()
		.insert("intensity", "light_intensity")));
	light->set_transform(
		asf::Transformd::from_local_to_parent(
		asf::Matrix4d::translation(asf::Vector3d(0.6, 2.0, 1.0))));
	assembly->lights().insert(light);

	//------------------------------------------------------------------------
	// Assembly instance
	//------------------------------------------------------------------------

	// Create an instance of the assembly and insert it into the scene.
	asf::auto_release_ptr<asr::AssemblyInstance> assembly_instance(
		asr::AssemblyInstanceFactory::create(
		"assembly_inst",
		asr::ParamArray(),
		"assembly"));
	assembly_instance
		->transform_sequence()
		.set_transform(
		0.0,
		asf::Transformd::identity());
	scene->assembly_instances().insert(assembly_instance);

	// Insert the assembly into the scene.
	scene->assemblies().insert(assembly);

	//------------------------------------------------------------------------
	// Environment
	//------------------------------------------------------------------------

	// Create a color called "sky_radiance" and insert it into the scene.
	static const float SkyRadiance[] = { 0.75f, 0.80f, 1.0f };
	scene->colors().insert(
		asr::ColorEntityFactory::create(
		"sky_radiance",
		asr::ParamArray()
		.insert("color_space", "srgb")
		.insert("multiplier", "0.5"),
		asr::ColorValueArray(3, SkyRadiance)));

	// Create an environment EDF called "sky_edf" and insert it into the scene.
	scene->environment_edfs().insert(
		asr::ConstantEnvironmentEDFFactory().create(
		"sky_edf",
		asr::ParamArray()
		.insert("radiance", "sky_radiance")));

	// Create an environment shader called "sky_shader" and insert it into the scene.
	scene->environment_shaders().insert(
		asr::EDFEnvironmentShaderFactory().create(
		"sky_shader",
		asr::ParamArray()
		.insert("environment_edf", "sky_edf")));

	// Create an environment called "sky" and bind it to the scene.
	scene->set_environment(
		asr::EnvironmentFactory::create(
		"sky",
		asr::ParamArray()
		.insert("environment_edf", "sky_edf")
		.insert("environment_shader", "sky_shader")));

	//------------------------------------------------------------------------
	// Camera
	//------------------------------------------------------------------------

	// Create a pinhole camera with film dimensions 0.980 x 0.735 in (24.892 x 18.669 mm).
	asf::auto_release_ptr<asr::Camera> camera(
		asr::PinholeCameraFactory().create(
		"camera",
		asr::ParamArray()
		.insert("film_dimensions", "0.024892 0.018669")
		.insert("focal_length", "0.035")));

	// Place and orient the camera. By default cameras are located in (0.0, 0.0, 0.0)
	// and are looking toward Z- (0.0, 0.0, -1.0).
	camera->transform_sequence().set_transform(
		0.0,
		asf::Transformd::from_local_to_parent(
		asf::Matrix4d::rotation(asf::Vector3d(1.0, 0.0, 0.0), asf::deg_to_rad(-20.0)) *
		asf::Matrix4d::translation(asf::Vector3d(0.0, 0.8, 11.0))));

	// Bind the camera to the scene.
	scene->set_camera(camera);

	//------------------------------------------------------------------------
	// Frame
	//------------------------------------------------------------------------

	// Create a frame and bind it to the project.
	project->set_frame(
		asr::FrameFactory::create(
		"beauty",
		asr::ParamArray()
		.insert("camera", scene->get_camera()->get_name())
		.insert("resolution", "640 480")
		.insert("color_space", "srgb")));

	// Bind the scene to the project.
	project->set_scene(scene);



	//defineProject();


	// Create the master renderer.
	asr::DefaultRendererController renderer_controller;
	asr::MasterRenderer mrenderer(
		project.ref(),
		project->configurations().get_by_name("final")->get_inherited_parameters(),
		&renderer_controller);

	// Render the frame.
	mrenderer.render();

	// Save the frame to disk.
	project->get_frame()->write_main_image("h:/temp/as/test.png");

	// Save the project to disk.
	asr::ProjectFileWriter::write(project.ref(), "h:/temp/as/test.appleseed");

	project.release();

	asr::global_logger().remove_target(log_target.get());

}


	//// Print appleseed's version string.
	//RENDERER_LOG_INFO("%s", asf::Appleseed::get_synthetic_version_string());

	//// Create the master renderer.
	//asr::DefaultRendererController renderer_controller;
	//asr::MasterRenderer renderer(
	//	project.ref(),
	//	project->configurations().get_by_name("final")->get_inherited_parameters(),
	//	&renderer_controller);

	//// Render the frame.
	//renderer.render();

	//// Save the frame to disk.
	//project->get_frame()->write_main_image("C:/daten/3dprojects/mayaToAppleseed/images/test.png");

	//// Save the project to disk.
	//asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/test.appleseed");

//
//	bool isIpr = this->mtap_scene->renderType == MayaScene::IPR;
//
//	this->tileCallbackFac.reset(new mtap_ITileCallbackFactory());
//
//	masterRenderer = nullptr;
//	
//	mtap_controller.entityUpdateProc = updateEntitiesCaller;
//
//	if( this->mtap_scene->renderType == MayaScene::NORMAL)
//	{
//		masterRenderer = new asr::MasterRenderer(
//			this->project.ref(),
//			this->project->configurations().get_by_name("final")->get_inherited_parameters(),
//			&mtap_controller,
//			this->tileCallbackFac.get());
//	}
//
//	if( this->mtap_scene->renderType == MayaScene::IPR)
//	{
//		masterRenderer = new asr::MasterRenderer(
//			this->project.ref(),
//			this->project->configurations().get_by_name("interactive")->get_inherited_parameters(),
//			&mtap_controller,
//			this->tileCallbackFac.get());
//	}
//	
//	
//	//log_target.reset(asf::create_console_log_target(stderr));
//    //asr::global_logger().add_target(log_target.get());
//
//	m_log_target.reset(asf::create_file_log_target());
//	MString logPath = this->renderGlobals->basePath + "/applelog" + (int)this->mtap_scene->renderGlobals->currentFrame + ".log";
//	m_log_target->open(logPath.asChar() );
//	asr::global_logger().add_target(m_log_target.get());
//
//	Logging::debug("------------- start rendering ----------------------");
//
//	masterRenderer->render();
//
//	//asr::global_logger().remove_target(log_target.get());
//	asr::global_logger().remove_target(m_log_target.get());
//	//m_log_target->close();
//
//	this->renderGlobals->getImageName();
//	Logging::debug(MString("Writing image: ") + renderGlobals->imageOutputFile);
//	MString imageOutputFile =  renderGlobals->imageOutputFile;
//	project->get_frame()->write_main_image(imageOutputFile.asChar());
//	project->get_frame()->write_aov_images(imageOutputFile.asChar());
//
//	// if we render sequences, we remove all geometry and other elemenet because
//	// otherwise we could have the case that we have a deforming geometry and it will receive more and more deform steps.
//	if( this->mtap_scene->renderType != MayaScene::IPR )
//	{
//		this->project->set_scene(asf::auto_release_ptr<asr::Scene>(0));
//		this->project->set_scene(asr::SceneFactory::create());
//		this->scenePtr = this->project->get_scene();
//
//		if( MGlobal::mayaState() != MGlobal::kBatch )
//		{
//			MString cmd = MString("import mtap_initialize as minit; minit.theRenderer().showLogFile(\"") + logPath + "\")";
//			Logging::debug(cmd);
//			MGlobal::executePythonCommand(cmd, true, false);	
//		}
//	}
//
