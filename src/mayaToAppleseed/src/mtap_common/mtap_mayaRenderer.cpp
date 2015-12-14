#include "mtap_mayaRenderer.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "../appleseed/appleseedGeometry.h"
#include "OSL/oslUtils.h"
#include "shadingTools/material.h"
#include "shadingTools/shadingUtils.h"
#include "world.h"

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
#include "../appleseed/appleseedUtils.h"

#include <thread>
#include <vector>
#include <maya/MGlobal.h>
#include <maya/MStringArray.h>
#include <maya/MFnDependencyNode.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>


#if MAYA_API_VERSION >= 201600

#define kNumChannels 4

#define GETASM() project->get_scene()->assemblies().get_by_name("world")

mtap_MayaRenderer::mtap_MayaRenderer()
{
	//log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());
	initProject();
	width = height = initialSize;
	this->rb = (float*)malloc(width*height*kNumChannels*sizeof(float));
}

mtap_MayaRenderer::~mtap_MayaRenderer()
{
	Logging::debug("~mtap_MayaRenderer");
	asr::global_logger().remove_target(log_target.get());
	mrenderer.release();
	project.release();
	objectArray.clear();
}

void mtap_MayaRenderer::initEnv()
{
	MString mayaRoot = getenv("MAYA_LOCATION");
	MString envMapAttrName = mayaRoot + "/presets/Assets/IBL/Exterior1_Color.exr";

	asr::ParamArray fileparams;
	fileparams.insert("filename", envMapAttrName.asChar());
	fileparams.insert("color_space", "linear_rgb");

	asf::SearchPaths searchPaths;
	asf::auto_release_ptr<asr::Texture> textureElement(
		asr::DiskTexture2dFactory().create(
		"envTex",
		fileparams,
		searchPaths));

	project->get_scene()->textures().insert(textureElement);

	asr::ParamArray tInstParams;
	tInstParams.insert("addressing_mode", "clamp");
	tInstParams.insert("filtering_mode", "bilinear");

	MString textureInstanceName = "envTex_texInst";
	asf::auto_release_ptr<asr::TextureInstance> tinst = asr::TextureInstanceFactory().create(
		textureInstanceName.asChar(),
		tInstParams,
		"envTex");
	project->get_scene()->texture_instances().insert(tinst);


	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
		"sky_edf",
		asr::ParamArray()
		.insert("radiance", textureInstanceName.asChar())
		.insert("radiance_multiplier", 1.0)
		.insert("horizontal_shift", -45.0f)
		.insert("vertical_shift", 10.0f)
		);

	asr::EnvironmentEDF *sky = project->get_scene()->environment_edfs().get_by_name("sky_edf");
	if (sky != nullptr)
		project->get_scene()->environment_edfs().remove(sky);
	project->get_scene()->environment_edfs().insert(environmentEDF);

	asr::EnvironmentShader *shader = project->get_scene()->environment_shaders().get_by_name("sky_shader");
	if (shader != nullptr)
		project->get_scene()->environment_shaders().remove(shader);

	project->get_scene()->environment_shaders().insert(
		asr::EDFEnvironmentShaderFactory().create(
		"sky_shader",
		asr::ParamArray()
		.insert("environment_edf", "sky_edf")));

	project->get_scene()->set_environment(
		asr::EnvironmentFactory::create(
		"sky",
		asr::ParamArray()
		.insert("environment_edf", "sky_edf")
		.insert("environment_shader", "sky_shader")));

}

void mtap_MayaRenderer::initProject()
{
	project = asr::ProjectFactory::create("mayaRendererProject");
	project->add_default_configurations();
	project->configurations().get_by_name("final")->get_parameters().insert("rendering_threads", 16);
	project->configurations().get_by_name("interactive")->get_parameters().insert("rendering_threads", 16);
#ifdef _DEBUG
	project->configurations().get_by_name("final")->get_parameters().insert_path("uniform_pixel_renderer.samples", "4");
#else
	project->configurations().get_by_name("final")->get_parameters().insert_path("uniform_pixel_renderer.samples", "16");
	//project->configurations().get_by_name("interactive")->get_parameters().insert_path("uniform_pixel_renderer.samples", "2");
#endif
	defineScene(project.get());
	defineMasterAssembly(project.get());
	defineDefaultMaterial(project.get());

	asr::Configuration *cfg = project->configurations().get_by_name("interactive");
	asr::ParamArray &params = cfg->get_parameters();
	params.insert("sample_renderer", "generic");
	params.insert("sample_generator", "generic");
	params.insert("tile_renderer", "generic");
	params.insert("frame_renderer", "progressive");
	params.insert("lighting_engine", "pt");
	params.insert_path("progressive_frame_renderer.max_fps", "4");
	params.insert_path("progressive_frame_renderer.max_samples", "12000000");

	MColor color(0,0,0);
	defineColor(project.get(), "black", color, 1.0f);

	initEnv();

	width = height = initialSize;
	MString res = MString("") + width + " " + height;
	tileSize = 64;
	MString tileString = MString("") + tileSize + " " + tileSize;

	project->set_frame(
		asr::FrameFactory::create(
		"beauty",
		asr::ParamArray()
		.insert("resolution", res.asChar())
		.insert("tile_size", tileString.asChar())
		.insert("color_space", "linear_rgb")));

	project->get_frame()->get_parameters().insert("pixel_format", "float");

	this->tileCallbackFac.reset(new TileCallbackFactory(this));

	mrenderer = std::auto_ptr<asr::MasterRenderer>(new asr::MasterRenderer(
		this->project.ref(),
		this->project->configurations().get_by_name("interactive")->get_inherited_parameters(),
		&controller,
		this->tileCallbackFac.get()));
	//mrenderer = std::auto_ptr<asr::MasterRenderer>(new asr::MasterRenderer(
	//	this->project.ref(),
	//	this->project->configurations().get_by_name("final")->get_inherited_parameters(),
	//	&controller,
	//	this->tileCallbackFac.get()));

	for (uint i = 0; i < MayaTo::getWorldPtr()->shaderSearchPath.length(); i++)
	{
		Logging::debug(MString("Search path: ") + MayaTo::getWorldPtr()->shaderSearchPath[i]);
		project->search_paths().push_back(MayaTo::getWorldPtr()->shaderSearchPath[i].asChar());
	}


}

bool mtap_MayaRenderer::isRunningAsync()
{
	Logging::debug("isRunningAsync");
	return true;
}
void* mtap_MayaRenderer::creator()
{
	return new mtap_MayaRenderer();
}

void mtap_MayaRenderer::render()
{
	ProgressParams pp;
	pp.progress = 0.0;
	progress(pp);

	if (mrenderer.get())
		mrenderer->render();
	else
		return;
#ifdef _DEBUG
	asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/mayaRenderer.xml");
#endif
	Logging::debug("renderthread done.");

	pp.progress = 2.0;
	progress(pp);

}

static void startRenderThread(mtap_MayaRenderer* renderPtr)
{
	renderPtr->render();
}

MStatus mtap_MayaRenderer::startAsync(const JobParams& params)
{
	Logging::debug("startAsync:");
	Logging::debug(MString("\tJobDescr: ") + params.description + " max threads: " + params.maxThreads);
	//renderThread = std::thread(startRenderThread, this);
	Logging::debug(MString("started async"));
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::stopAsync()
{
	Logging::debug("stopAsync");
	controller.status = asr::IRendererController::AbortRendering;
	if (renderThread.joinable())
		renderThread.join();
	return MS::kSuccess;
}

MStatus mtap_MayaRenderer::beginSceneUpdate()
{
	Logging::debug("beginSceneUpdate");
	controller.status = asr::IRendererController::AbortRendering;
	if (renderThread.joinable())
		renderThread.join();
	if (project.get() == nullptr)
		initProject();
	project->get_frame()->clear_main_image();

	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateMesh(const MUuid& id, const MObject& node)
{
	MObject mobject = node;
	MFnDependencyNode depFn(mobject);
	MString meshName = depFn.name();
	MString meshIdName = meshName;
	//MString meshIdName = meshName + "_" + id;
	MString meshInstName = meshIdName + "_instance";
	Logging::debug(MString("translateMesh ") + meshIdName);

	asr::Object *obj = GETASM()->objects().get_by_name(meshIdName.asChar());
	if (obj != nullptr)
	{
		Logging::debug(MString("Mesh object ") + meshName + " is already defined, removing...");
		GETASM()->objects().remove(obj);
		GETASM()->bump_version_id();
	}
	asf::auto_release_ptr<asr::MeshObject> mesh = MTAP_GEOMETRY::createMesh(mobject);
	mesh->set_name(meshIdName.asChar());
	GETASM()->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));

	IdNameStruct idName;
	idName.id = id;
	idName.name = meshIdName;
	idName.mobject = node;
	objectArray.push_back(idName);
	lastShapeId = id;
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
	MFnDependencyNode depFn(node);
	MString lightName = depFn.name();
	MString lightInstName = lightName + "_instance";
	MString lightIdName = lightName + "_" + id;

	Logging::debug(MString("translateLightSource: ") + depFn.name() + " from type: " + node.apiTypeStr());
	if (node.hasFn(MFn::kAreaLight))
	{		
		asr::MeshObject *meshPtr = (asr::MeshObject *)GETASM()->objects().get_by_name(lightIdName.asChar());
		if (meshPtr != nullptr)
			GETASM()->objects().remove(meshPtr);

		asf::auto_release_ptr<asr::MeshObject> plane = MTAP_GEOMETRY::defineStandardPlane();
		plane->set_name(lightIdName.asChar());
		GETASM()->objects().insert(asf::auto_release_ptr<asr::Object>(plane));

		meshPtr = (asr::MeshObject *)GETASM()->objects().get_by_name(lightIdName.asChar());

		MString areaLightMaterialName = lightIdName + "_material";
		MString physicalSurfaceName = lightIdName + "_physical_surface_shader";
		MString areaLightColorName = lightIdName + "_color";
		MString edfName = lightIdName + "_edf";
		asr::ParamArray edfParams;
		MColor color = getColorAttr("color", depFn);
		defineColor(project.get(), areaLightColorName.asChar(), color, getFloatAttr("intensity", depFn, 1.0f) * 1);
		edfParams.insert("radiance", areaLightColorName.asChar());

		asr::EDF *edfPtr = GETASM()->edfs().get_by_name(edfName.asChar());
		if (edfPtr != nullptr)
			GETASM()->edfs().remove(edfPtr);

		asf::auto_release_ptr<asr::EDF> edf = asr::DiffuseEDFFactory().create(edfName.asChar(), edfParams);
		GETASM()->edfs().insert(edf);

		asr::SurfaceShader *ss = GETASM()->surface_shaders().get_by_name(physicalSurfaceName.asChar());
		if (ss != nullptr)
			GETASM()->surface_shaders().remove(ss);

		GETASM()->surface_shaders().insert(
			asr::PhysicalSurfaceShaderFactory().create(
			physicalSurfaceName.asChar(),
			asr::ParamArray()));

		asr::Material *ma = GETASM()->materials().get_by_name(areaLightMaterialName.asChar());
		if (ma != nullptr)
			GETASM()->materials().remove(ma);

		GETASM()->materials().insert(
			asr::GenericMaterialFactory().create(
			areaLightMaterialName.asChar(),
			asr::ParamArray()
			.insert("surface_shader", physicalSurfaceName.asChar())
			.insert("edf", edfName.asChar())));

		IdNameStruct idName;
		idName.id = id;
		idName.name = lightIdName;
		idName.mobject = node;
		objectArray.push_back(idName);
		lastShapeId = id;
	}
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateCamera(const MUuid& id, const MObject& node)
{
	Logging::debug("translateCamera");
	
	asr::Camera *camera = project->get_scene()->get_camera();
	MFnDependencyNode depFn(node);
	MString camName = depFn.name();
	
	float imageAspect = (float)width / (float)height;
	asr::ParamArray camParams;
	float horizontalFilmAperture = getFloatAttr("horizontalFilmAperture", depFn, 24.892f);
	float verticalFilmAperture = getFloatAttr("verticalFilmAperture", depFn, 18.669f);
	float focalLength = getFloatAttr("focalLength", depFn, 35.0f);
	float focusDistance = getFloatAttr("focusDistance", depFn, 10.0f);
	float fStop = getFloatAttr("fStop", depFn, 1000.0f);

	// maya aperture is given in inces so convert to cm and convert to meters
	horizontalFilmAperture = horizontalFilmAperture * 2.54f * 0.01f;
	verticalFilmAperture = verticalFilmAperture * 2.54f * 0.01f;
	verticalFilmAperture = horizontalFilmAperture / imageAspect;
	MString filmBack = MString("") + horizontalFilmAperture + " " + verticalFilmAperture;
	MString focalLen = MString("") + focalLength * 0.001f;

	camParams.insert("film_dimensions", filmBack.asChar());
	camParams.insert("focal_length", focalLen.asChar());
	camParams.insert("focal_distance", (MString("") + focusDistance).asChar());
	camParams.insert("f_stop", (MString("") + fStop).asChar());

	asf::auto_release_ptr<asr::Camera> appleCam = asr::PinholeCameraFactory().create(
		camName.asChar(),
		camParams);
	project->get_scene()->set_camera(appleCam);

	IdNameStruct idName;
	idName.id = id;
	idName.name = camName + "_" + id;
	idName.mobject = node;
	objectArray.push_back(idName);
	lastShapeId = id;
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
	Logging::debug("translateEnvironment");
	IdNameStruct idName;
	idName.id = id;
	idName.name = MString("Environment");
	idName.mobject = MObject::kNullObj;
	objectArray.push_back(idName);
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
	Logging::debug(MString("translateTransform id: ") + id + " childId " + childId);
	MObject shapeNode;
	IdNameStruct idNameObj;
	for (auto idobj : objectArray)
	{
		if (idobj.id == lastShapeId)
		{
			Logging::debug(MString("Found id object for transform: ") + idobj.name);
			shapeNode = idobj.mobject;
			idNameObj = idobj;
		}
	}
	MString elementInstName = idNameObj.name + "_instance";
	MString elementName = idNameObj.name;
	MMatrix mayaMatrix = matrix;
	asf::Matrix4d appleMatrix;
	MMatrixToAMatrix(mayaMatrix, appleMatrix);

	if (idNameObj.mobject.hasFn(MFn::kMesh))
	{
		asr::ObjectInstance *objInst = GETASM()->object_instances().get_by_name(elementInstName.asChar());
		if (objInst != nullptr)
		{
			Logging::debug(MString("Removing already existing inst object: ") + elementInstName);
			GETASM()->object_instances().remove(objInst);
		}
		GETASM()->object_instances().get_by_name(elementInstName.asChar());
		GETASM()->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			elementInstName.asChar(),
			asr::ParamArray(),
			elementName.asChar(),
			asf::Transformd::from_local_to_parent(appleMatrix),
			asf::StringDictionary()
			.insert("slot0", lastMaterialName),
			asf::StringDictionary()
			.insert("slot0", lastMaterialName)));
	}

	if (idNameObj.mobject.hasFn(MFn::kCamera))
	{
		asr::Camera *cam = project->get_scene()->get_camera();
		cam->transform_sequence().set_transform(0, asf::Transformd::from_local_to_parent(appleMatrix));
	}
	if (idNameObj.mobject.hasFn(MFn::kAreaLight))
	{
		MTransformationMatrix tm;
		double rotate90Deg[3] = { -M_PI_2, 0, 0 };
		tm.setRotation(rotate90Deg, MTransformationMatrix::kXYZ);
		MMatrix lightMatrix = tm.asMatrix() * mayaMatrix;
		MMatrixToAMatrix(lightMatrix, appleMatrix);

		asr::ObjectInstance *objInst = GETASM()->object_instances().get_by_name(elementInstName.asChar());
		if (objInst != nullptr)
		{
			Logging::debug(MString("Removing already existing inst object: ") + elementInstName);
			GETASM()->object_instances().remove(objInst);
		}
		Logging::debug(MString("area light transform: ") + idNameObj.name);
		MString areaLightMaterialName = elementName + "_material";
		asr::ParamArray instParams;
		instParams.insert_path("visibility.camera", false); // set primary visibility to false for area lights
		
		GETASM()->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			elementInstName.asChar(),
			instParams,
			elementName.asChar(),
			asf::Transformd::from_local_to_parent(appleMatrix),
			asf::StringDictionary()
			.insert("slot0", areaLightMaterialName.asChar()),
			asf::StringDictionary()
			.insert("slot0", areaLightMaterialName.asChar())
			));
	}


	IdNameStruct idName;
	idName.id = id;
	idName.name = elementInstName;
	idName.mobject = shapeNode;
	objectArray.push_back(idName);

	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateShader(const MUuid& id, const MObject& node)
{
	MFnDependencyNode depFn(node);
	MString shaderGroupName = depFn.name() + "_" + id + "_SG";
	MString shaderMaterialName = depFn.name() + "_" + "Material";
	Logging::debug(MString("translateShader: ") + shaderGroupName);

	MStatus status;
	asf::StringArray materialNames;
	MAYATO_OSLUTIL::OSLUtilClass oslClass;
	MString surfaceShader;
	MObject surfaceShaderNode = node;
	ShadingNetwork network(surfaceShaderNode);
	size_t numNodes = network.shaderList.size();

	asr::ShaderGroup *existingShaderGroup = GETASM()->shader_groups().get_by_name(shaderGroupName.asChar());
	if (existingShaderGroup != nullptr)
	{
		existingShaderGroup->clear();
		oslClass.group = (OSL::ShaderGroup *)existingShaderGroup;
	}
	else{
		asf::auto_release_ptr<asr::ShaderGroup> oslShaderGroup = asr::ShaderGroupFactory().create(shaderGroupName.asChar());
		GETASM()->shader_groups().insert(oslShaderGroup);
		MString physicalSurfaceName = shaderGroupName + "_physical_surface_shader";
		GETASM()->surface_shaders().insert(
			asr::PhysicalSurfaceShaderFactory().create(
			physicalSurfaceName.asChar(),
			asr::ParamArray()));

		GETASM()->materials().insert(
			asr::OSLMaterialFactory().create(
			shaderMaterialName.asChar(),
			asr::ParamArray()
			.insert("surface_shader", physicalSurfaceName.asChar())
			.insert("osl_surface", shaderGroupName.asChar())));

		existingShaderGroup = GETASM()->shader_groups().get_by_name(shaderGroupName.asChar());
	}
	oslClass.group = (OSL::ShaderGroup *)existingShaderGroup;
	oslClass.createOSLProjectionNodes(surfaceShaderNode);
	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = network.shaderList[shadingNodeId];
		Logging::debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		if (shadingNodeId == (numNodes - 1))
			Logging::debug(MString("LastNode Surface Shader: ") + snode.fullName);
		//oslClass.createOSLHelperNodes(network.shaderList[shadingNodeId]);
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

	IdNameStruct idName;
	idName.id = id;
	idName.name = shaderMaterialName;
	idName.mobject = node;
	objectArray.push_back(idName);
	lastMaterialName = shaderMaterialName;
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
	Logging::debug(MString("setProperty bool: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
	Logging::debug(MString("setProperty int: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
	Logging::debug(MString("setProperty float: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
	Logging::debug(MString("setProperty string: ") + name + " " + value);

	IdNameStruct idNameObj;
	for (auto idobj : objectArray)
	{
		if (idobj.id == id)
		{
			Logging::debug(MString("Found id object for string property: ") + idobj.name);
			if (idobj.name == "Environment")
			{
				if (name == "imageFile")
				{
					Logging::debug(MString("Setting environment image file to: ") + value);
					asr::Texture *tex = project->get_scene()->textures().get_by_name("envTex");
					if ( tex != nullptr)
					{
						Logging::debug(MString("Removing already existing env texture."));
						project->get_scene()->textures().remove(tex);
					}
					MString imageFile = value;
					if (value.length() == 0)
					{
						MString mayaRoot = getenv("MAYA_LOCATION");
						imageFile = mayaRoot + "/presets/Assets/IBL/black.exr";

						//asr::ParamArray& pa = project->get_scene()->environment_edfs().get_by_name("sky_edf")->get_parameters();
						//if (MString(pa.get_path("radiance")) != "black")
						//{
						//	pa.insert("radiance", "black");
						//	project->get_scene()->environment_edfs().get_by_name("sky_edf")->bump_version_id();
						//}
					}

					asr::ParamArray& pa = project->get_scene()->environment_edfs().get_by_name("sky_edf")->get_parameters();
					if (MString(pa.get_path("radiance")) != "envTex_texInst")
					{
						pa.insert("radiance", "envTex_texInst");
						project->get_scene()->environment_edfs().get_by_name("sky_edf")->bump_version_id();
					}

					asr::ParamArray fileparams;
					fileparams.insert("filename", imageFile.asChar());
					fileparams.insert("color_space", "linear_rgb");

					asf::SearchPaths searchPaths;
					asf::auto_release_ptr<asr::Texture> textureElement(
						asr::DiskTexture2dFactory().create(
						"envTex",
						fileparams,
						searchPaths));

					project->get_scene()->textures().insert(textureElement);
				}
			}
		}
	}

	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
	Logging::debug("setShader");
	IdNameStruct objElement, shaderElement;
	for (auto element : objectArray)
	{
		Logging::debug(MString("Search for obj id: ") + id + " in " + element.id + " name: " + element.name);
		if (element.id == id)
		{
			objElement = element;
			break;
		}
	}
	for (auto element : objectArray)
	{
		Logging::debug(MString("Search for shader id: ") + shaderId + " in " + element.id + " name: " + element.name);
		if (element.id == shaderId)
		{
			shaderElement = element;
			break;
		}
	}
	if ((objElement.mobject == MObject::kNullObj) || (shaderElement.mobject == MObject::kNullObj))
	{
		Logging::error(MString("Unable to find obj or shader for assignment. ShaderName: ") + shaderElement.name + " obj name " + objElement.name);
		return MS::kFailure;
	}
	Logging::debug(MString("--------- Assign shader ") + shaderElement.name + " to object named " + objElement.name);


	asr::ObjectInstance *objInstance = GETASM()->object_instances().get_by_name(objElement.name.asChar());
	if ( objInstance != nullptr)
		objInstance->get_front_material_mappings().insert("slot0", shaderElement.name.asChar());
	else
		Logging::debug(MString("unable to assign shader "));


	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setResolution(unsigned int w, unsigned int h)
{
	Logging::debug(MString("setResolution to") + w + " " + h);
	this->width = w;
	this->height = h;
	// Update resolution buffer
	this->rb = (float*)realloc(this->rb, w*h*kNumChannels*sizeof(float));

	for (uint x = 0; x < width; x++)
	{
		for (uint y = 0; y < height; y++)
		{
			uint index = (y * width + x) * kNumChannels;
			rb[index + 0] = x / float(width);
			rb[index + 1] = 0.0f;
			rb[index + 2] = y / float(height);
			rb[index + 3] = 1.0f;
		}
	}

	MString res = MString("") + width + " " + height;
	MString tileString = MString("") + tileSize + " " + tileSize;

	asr::Camera *cam = project->get_scene()->get_camera();
	MString camName = "";
	if (cam != nullptr)
	{
		camName = project->get_scene()->get_camera()->get_name();
		asr::ParamArray &camParams = cam->get_parameters();
		MString dim = camParams.get_path("film_dimensions");
		MStringArray values;
		dim.split(' ', values);
		float filmWidth = values[0].asFloat();
		float filmHeight = filmWidth * h / (float)w;
		camParams.insert("film_dimensions", (MString("") + filmWidth + " " + filmHeight).asChar());
	}

	project->set_frame(
		asr::FrameFactory::create(
		"beauty",
		asr::ParamArray()
		.insert("camera", camName.asChar())
		.insert("resolution", res.asChar())
		.insert("tile_size", tileString.asChar())
		.insert("pixel_format", "float")
		.insert("color_space", "linear_rgb")));

	project->get_frame()->get_parameters().insert("pixel_format", "float");

	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::endSceneUpdate()
{
	Logging::debug("endSceneUpdate");
	controller.status = asr::IRendererController::ContinueRendering;
	ProgressParams progressParams;
	progressParams.progress = 0.0;
	progress(progressParams);

	renderThread = std::thread(startRenderThread, this);

	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::destroyScene()
{
	Logging::debug("destroyScene");
	controller.status = asr::IRendererController::AbortRendering;
	if (renderThread.joinable())
		renderThread.join();
	//mrenderer.release();
	//project.release();
	//objectArray.clear();

	ProgressParams progressParams;
	progressParams.progress = -1.0f;
	progress(progressParams);
	return MStatus::kSuccess;
};

bool mtap_MayaRenderer::isSafeToUnload()
{
	Logging::debug("isSafeToUnload");
	return true;
};

void mtap_MayaRenderer::copyTileToBuffer(asf::Tile& tile, int tile_x, int tile_y)
{
	size_t tw = tile.get_width();
	size_t th = tile.get_height();

	for (int y = 0; y < th; y++)
	{
		for (int x = 0; x < tw; x++)
		{
			int index = ((height - 1) - (tile_y * tileSize + y)) * width + (tile_x * tileSize) + x;
			index *= kNumChannels;
			
			rb[index] = tile.get_component<float>(x, y, 0);
			rb[index + 1] = tile.get_component<float>(x, y, 1);
			rb[index + 2] = tile.get_component<float>(x, y, 2);
			rb[index + 3] = tile.get_component<float>(x, y, 3);
		}
	}
	////The image is R32G32B32A32_Float format
	refreshParams.bottom = 0;
	refreshParams.top = height - 1;
	refreshParams.bytesPerChannel = sizeof(float);
	refreshParams.channels = kNumChannels;
	refreshParams.left = 0;
	refreshParams.right = width - 1;
	refreshParams.width = width;
	refreshParams.height = height;
	refreshParams.data = rb;
	refresh(refreshParams);
}

void mtap_MayaRenderer::copyFrameToBuffer(float *frame, int w, int h)
{
	if ((w != width) || (h != height))
	{
		Logging::error("wh ungleich.");
		return;
	}
	int numBytes = width * height * kNumChannels * sizeof(float);
	memcpy(rb, frame, numBytes);

	refreshParams.bottom = 0;
	refreshParams.top = height - 1;
	refreshParams.bytesPerChannel = sizeof(float);
	refreshParams.channels = kNumChannels;
	refreshParams.left = 0;
	refreshParams.right = width - 1;
	refreshParams.width = width;
	refreshParams.height = height;
	refreshParams.data = rb;
	refresh(refreshParams);

}

std::mutex tile_mutex;

void TileCallback::post_render_tile(const asr::Frame* frame, const size_t tile_x, const size_t tile_y)
{
	Logging::debug("TileCallback::post_render_tile");	
	asf::Tile& tile = frame->image().tile(tile_x, tile_y);
	this->renderer->copyTileToBuffer(tile, tile_x, tile_y);
}

void TileCallback::post_render(const asr::Frame* frame)
{
	Logging::debug("TileCallback::post_render frame");
	asf::Image img = frame->image();
	const asf::CanvasProperties& frame_props = img.properties();
	int tileSize = frame_props.m_tile_height;
	size_t numPixels = frame_props.m_canvas_width * frame_props.m_canvas_height;
	int width = frame_props.m_canvas_width;
	int height = frame_props.m_canvas_height;

	Logging::debug(MString("TileCallback:: wh ") + width + " " + height + " tileSize " + tileSize);

	std::shared_ptr<float> buffer = std::shared_ptr<float>(new float[numPixels * kNumChannels]);
	float *rb = buffer.get();

	for (int tile_x = 0; tile_x < frame_props.m_tile_count_x; tile_x++)
	{
		for (int tile_y = 0; tile_y < frame_props.m_tile_count_y; tile_y++)
		{
			const asf::Tile& tile = frame->image().tile(tile_x, tile_y);
			size_t tw = tile.get_width();
			size_t th = tile.get_height();
			
			for (int y = 0; y < th; y++)
			{
				for (int x = 0; x < tw; x++)
				{
					int index = ((height - 1) - (tile_y * tileSize + y)) * width + (tile_x * tileSize) + x;
					index *= kNumChannels;

					rb[index] = tile.get_component<float>(x, y, 0);
					rb[index + 1] = tile.get_component<float>(x, y, 1);
					rb[index + 2] = tile.get_component<float>(x, y, 2);
					rb[index + 3] = tile.get_component<float>(x, y, 3);
				}
			}

		}
	}

	this->renderer->copyFrameToBuffer(buffer.get(), width, height);
}

#endif
