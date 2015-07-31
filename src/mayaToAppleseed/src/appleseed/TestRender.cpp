#include "TestRender.h"

namespace asf = foundation;
namespace asr = renderer;

void doTestRender()
{
	std::auto_ptr<asf::ILogTarget> log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	asr::global_logger().add_target(log_target.get());
	RENDERER_LOG_INFO("%s", asf::Appleseed::get_synthetic_version_string());

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