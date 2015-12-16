#include "appleseed.h"
#include <maya/MFnLight.h>
#include <maya/MPlugArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MAngle.h>
#include "mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "world.h"
#include "appleseedUtils.h"
#include "renderer/api/edf.h"

static Logging logger;

using namespace AppleRender;
#define colorAttr(c) (MString("") + c.r + " " + c.g + " " + c.g)

void AppleseedRenderer::defineLight(std::shared_ptr<MayaObject> obj)
{
	asr::Assembly *lightAssembly = getCreateObjectAssembly(obj);
	asr::AssemblyInstance *lightAssemblyInstance = getExistingObjectAssemblyInstance(obj.get());
	asr::Light *light = lightAssembly->lights().get_by_name(obj->shortName.asChar());
	MFnDependencyNode depFn(obj->mobject);

	if (obj->mobject.hasFn(MFn::kPointLight))
	{
		bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		MColor col = getColorAttr("color", depFn);
		float intensity = getFloatAttr("intensity", depFn, 1.0f);
		MString colorAttribute = obj->shortName + "_intensity";
		defineColor(colorAttribute, col, intensity);
		int decay = getEnumInt("decayRate", depFn);
		if (light == nullptr)
		{
			asf::auto_release_ptr<asr::Light> lp = asf::auto_release_ptr<asr::Light>(
				asr::PointLightFactory().create(
				obj->shortName.asChar(),
				asr::ParamArray()));
			lightAssembly->lights().insert(lp);
			light = lightAssembly->lights().get_by_name(obj->shortName.asChar());
		}
		asr::ParamArray& params = light->get_parameters();
		params.insert("intensity", colorAttribute);
		params.insert("intensity_multiplier", intensity);
		params.insert("importance_multiplier", importance_multiplier);
		params.insert("cast_indirect_light", cast_indirect_light);
		fillTransformMatrices(obj, light);
	}
	if (obj->mobject.hasFn(MFn::kSpotLight))
	{
		// redefinition because it is possible that this value is textured
		bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		MColor col = getColorAttr("color", depFn);
		float intensity = getFloatAttr("intensity", depFn, 1.0f);
		MString colorAttribute = obj->shortName + "_intensity";
		defineColor(colorAttribute, col, intensity);
		Logging::debug(MString("Creating spotLight: ") + depFn.name());
		float coneAngle = getDegree("coneAngle", depFn);
		float penumbraAngle = getDegree("penumbraAngle", depFn);
		float inner_angle = coneAngle;
		float outer_angle = coneAngle + penumbraAngle;

		if (light == nullptr)
		{
			asf::auto_release_ptr<asr::Light> lp = asr::SpotLightFactory().create(
				obj->shortName.asChar(),
				asr::ParamArray());
			lightAssembly->lights().insert(lp);
			light = lightAssembly->lights().get_by_name(obj->shortName.asChar());
		}

		asr::ParamArray& params = light->get_parameters();
		params.insert("radiance", colorAttribute);
		params.insert("radiance_multiplier", intensity);
		params.insert("inner_angle", inner_angle);
		params.insert("outer_angle", outer_angle);
		params.insert("importance_multiplier", importance_multiplier);
		params.insert("cast_indirect_light", cast_indirect_light);
		MMatrix matrix = obj->transformMatrices[0];
		fillTransformMatrices(obj, light);
	}
	if (obj->mobject.hasFn(MFn::kDirectionalLight))
	{
		bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		MVector lightDir(0, 0, -1);
		MVector lightDirTangent(1, 0, 0);
		MVector lightDirBiTangent(0, 1, 0);
		MColor col = getColorAttr("color", depFn);
		float intensity = getFloatAttr("intensity", depFn, 1.0f);
		MString colorAttribute = obj->shortName + "_intensity";
		defineColor(colorAttribute, col, intensity);

		if (isSunLight(obj->mobject))
		{
			//Logging::debug(MString("Found sunlight."));
			//light = asf::auto_release_ptr<asr::Light>(
			//	asr::SunLightFactory().create(
			//	"sunLight",
			//	asr::ParamArray()
			//		.insert("environment_edf", "sky_edf")
			//		.insert("turbidity", renderGlobals->sunTurbidity)
			//		.insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f)
			//		));
		}
		else{
			if (light == nullptr)
			{
				asf::auto_release_ptr<asr::Light> lp = asr::DirectionalLightFactory().create(
					obj->shortName.asChar(),
					asr::ParamArray());
				lightAssembly->lights().insert(lp);
				light = lightAssembly->lights().get_by_name(obj->shortName.asChar());
			}
			asr::ParamArray& params = light->get_parameters();
			params.insert("irradiance", colorAttribute);
			params.insert("irradiance_multiplier", intensity);
			params.insert("importance_multiplier", importance_multiplier);
			params.insert("cast_indirect_light", cast_indirect_light);
		}
		fillTransformMatrices(obj, light);
	}

	if (obj->mobject.hasFn(MFn::kAreaLight))
	{
		MString areaLightName = obj->fullNiceName;
		asf::auto_release_ptr<asr::MeshObject> plane = defineStandardPlane();
		plane->set_name(areaLightName.asChar());
		MayaObject *assemblyObject = getAssemblyMayaObject(obj.get());
		asr::Assembly *ass = getCreateObjectAssembly(obj);
		ass->objects().insert(asf::auto_release_ptr<asr::Object>(plane));
		asr::MeshObject *meshPtr = (asr::MeshObject *)ass->objects().get_by_name(areaLightName.asChar());
		MString objectInstanceName = getObjectInstanceName(obj.get());
		MMatrix assemblyObjectMatrix = assemblyObject->dagPath.inclusiveMatrix();
		// rotate the defalt up pointing standard plane by 90 degree to match the area light direction
		MTransformationMatrix tm;
		double rotate90Deg[3] = { -M_PI_2, 0, 0 };
		tm.setRotation(rotate90Deg, MTransformationMatrix::kXYZ);
		MMatrix objectMatrix = tm.asMatrix();
		MMatrix diffMatrix = objectMatrix;// *assemblyObjectMatrix;
		asf::Matrix4d appleMatrix;
		MMatrixToAMatrix(diffMatrix, appleMatrix);

		MString areaLightMaterialName = areaLightName + "_material";

		MString physicalSurfaceName = areaLightName + "_physical_surface_shader";
		MString areaLightColorName = areaLightName + "_color";
		MString edfName = areaLightName + "_edf";
		asr::ParamArray edfParams;
		MString lightColor = lightColorAsString(depFn);
		MColor color = getColorAttr("color", depFn);
		defineColor(areaLightColorName, color, getFloatAttr("intensity", depFn, 1.0f));
		edfParams.insert("radiance", areaLightColorName.asChar());
		//edfParams.insert("radiance_multiplier", getFloatAttr("intensity", depFn, 1.0f));

		asf::auto_release_ptr<asr::EDF> edf = asr::DiffuseEDFFactory().create(edfName.asChar(), edfParams);
		ass->edfs().insert(edf);

		ass->surface_shaders().insert(
			asr::PhysicalSurfaceShaderFactory().create(
			physicalSurfaceName.asChar(),
			asr::ParamArray()));

		ass->materials().insert(
			asr::GenericMaterialFactory().create(
			areaLightMaterialName.asChar(),
			asr::ParamArray()
			.insert("surface_shader", physicalSurfaceName.asChar())
			.insert("edf", edfName.asChar())));

		asr::ParamArray objInstanceParamArray;
		addVisibilityFlags(obj, objInstanceParamArray);

		ass->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			objectInstanceName.asChar(),
			objInstanceParamArray,
			meshPtr->get_name(),
			asf::Transformd::from_local_to_parent(appleMatrix),
			asf::StringDictionary()
			.insert("slot0", areaLightMaterialName.asChar()),
			asf::StringDictionary()
			.insert("slot0", "default")));

		if (lightAssemblyInstance != nullptr)
			fillMatrices(obj, lightAssemblyInstance->transform_sequence());
	}
}

void AppleseedRenderer::defineLights()
{
	MStatus stat;
	MFnDependencyNode rGlNode(getRenderGlobalsNode());
	// first get the globals node and serach for a directional light connection
	MObject coronaGlobals = getRenderGlobalsNode();
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	for (auto mobj : mayaScene->lightList)
	{
		std::shared_ptr<mtap_MayaObject> obj(std::static_pointer_cast<mtap_MayaObject>(mobj));
		
		if (!obj->visible)
			continue;

		if (isSunLight(obj->mobject))
			continue;

		defineLight(obj);
		//asr::Assembly *lightAssembly = getCreateObjectAssembly(obj.get());

		//MFnDependencyNode depFn(obj->mobject);
		//asf::auto_release_ptr<asr::Light> light;

		//if (obj->mobject.hasFn(MFn::kPointLight))
		//{
		//	bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		//	float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		//	MColor col = getColorAttr("color", depFn);
		//	float intensity = getFloatAttr("intensity", depFn, 1.0f);
		//	MString colorAttribute = obj->shortName + "_intensity";
		//	defineColor(colorAttribute, col, intensity);
		//	int decay = getEnumInt("decayRate", depFn);
		//	light = asf::auto_release_ptr<asr::Light>(
		//		asr::PointLightFactory().create(
		//			obj->shortName.asChar(),
		//			asr::ParamArray()
		//			.insert("intensity", colorAttribute)
		//			.insert("intensity_multiplier", intensity)
		//			.insert("importance_multiplier", importance_multiplier)
		//			.insert("cast_indirect_light", cast_indirect_light)
		//			));
		//}
		//if (obj->mobject.hasFn(MFn::kSpotLight))
		//{
		//	// redefinition because it is possible that this value is textured
		//	bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		//	float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		//	MColor col = getColorAttr("color", depFn);
		//	float intensity = getFloatAttr("intensity", depFn, 1.0f);
		//	MString colorAttribute = obj->shortName + "_intensity";
		//	defineColor(colorAttribute, col, intensity);
		//	Logging::debug(MString("Creating spotLight: ") + depFn.name());
		//	float coneAngle = getDegree("coneAngle", depFn);
		//	float penumbraAngle = getDegree("penumbraAngle", depFn);
		//	float inner_angle = coneAngle;
		//	float outer_angle = coneAngle + penumbraAngle;

		//	light = asf::auto_release_ptr<asr::Light>(
		//		asr::SpotLightFactory().create(
		//			obj->shortName.asChar(),
		//			asr::ParamArray()
		//			.insert("radiance", colorAttribute)
		//				.insert("radiance_multiplier", intensity)
		//				.insert("inner_angle", inner_angle)
		//				.insert("outer_angle", outer_angle)
		//				.insert("importance_multiplier", importance_multiplier)
		//				.insert("cast_indirect_light", cast_indirect_light)
		//				));
		//}
		//if (obj->mobject.hasFn(MFn::kDirectionalLight))
		//{
		//	bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
		//	float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
		//	MVector lightDir(0, 0, -1);
		//	MVector lightDirTangent(1, 0, 0);
		//	MVector lightDirBiTangent(0, 1, 0);
		//	MColor col = getColorAttr("color", depFn);
		//	float intensity = getFloatAttr("intensity", depFn, 1.0f);
		//	MString colorAttribute = obj->shortName + "_intensity";
		//	defineColor(colorAttribute, col, intensity);

		//	if( isSunLight(obj->mobject))
		//	{
		//		//Logging::debug(MString("Found sunlight."));
		//		//light = asf::auto_release_ptr<asr::Light>(
		//		//	asr::SunLightFactory().create(
		//		//	"sunLight",
		//		//	asr::ParamArray()
		//		//		.insert("environment_edf", "sky_edf")
		//		//		.insert("turbidity", renderGlobals->sunTurbidity)
		//		//		.insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f)
		//		//		));
		//	}else{
		//		light = asf::auto_release_ptr<asr::Light>(
		//			asr::DirectionalLightFactory().create(
		//				obj->shortName.asChar(),
		//				asr::ParamArray()
		//				.insert("irradiance", colorAttribute)
		//				.insert("irradiance_multiplier", intensity)
		//				.insert("importance_multiplier", importance_multiplier)
		//				.insert("cast_indirect_light", cast_indirect_light)
		//				));
		//	}
		//}

		//if (obj->mobject.hasFn(MFn::kAreaLight))
		//{
		//	MString areaLightName = obj->fullNiceName;
		//	asf::auto_release_ptr<asr::MeshObject> plane = defineStandardPlane();
		//	plane->set_name(areaLightName.asChar());
		//	MayaObject *assemblyObject = getAssemblyMayaObject(obj.get());
		//	asr::Assembly *ass = getCreateObjectAssembly(obj.get());
		//	ass->objects().insert(asf::auto_release_ptr<asr::Object>(plane));
		//	asr::MeshObject *meshPtr = (asr::MeshObject *)ass->objects().get_by_name(areaLightName.asChar());
		//	MString objectInstanceName = getObjectInstanceName(obj.get());
		//	MMatrix assemblyObjectMatrix = assemblyObject->dagPath.inclusiveMatrix();
		//	// rotate the defalt up pointing standard plane by 90 degree to match the area light direction
		//	MTransformationMatrix tm;
		//	double rotate90Deg[3] = { -M_PI_2, 0, 0 };
		//	tm.setRotation(rotate90Deg, MTransformationMatrix::kXYZ);
		//	MMatrix objectMatrix = tm.asMatrix();
		//	MMatrix diffMatrix = objectMatrix;// *assemblyObjectMatrix;
		//	asf::Matrix4d appleMatrix;
		//	MMatrixToAMatrix(diffMatrix, appleMatrix);

		//	MString areaLightMaterialName = areaLightName + "_material";

		//	MString physicalSurfaceName = areaLightName + "_physical_surface_shader";
		//	MString areaLightColorName = areaLightName + "_color";
		//	MString edfName = areaLightName + "_edf";
		//	asr::ParamArray edfParams;
		//	MString lightColor = lightColorAsString(depFn);
		//	MColor color = getColorAttr("color", depFn);
		//	defineColor(areaLightColorName, color, getFloatAttr("intensity", depFn, 1.0f));
		//	edfParams.insert("radiance", areaLightColorName.asChar());
		//	//edfParams.insert("radiance_multiplier", getFloatAttr("intensity", depFn, 1.0f));

		//	asf::auto_release_ptr<asr::EDF> edf = asr::DiffuseEDFFactory().create(edfName.asChar(), edfParams);
		//	ass->edfs().insert(edf);

		//	ass->surface_shaders().insert(
		//		asr::PhysicalSurfaceShaderFactory().create(
		//		physicalSurfaceName.asChar(),
		//		asr::ParamArray()));
		//	
		//	ass->materials().insert(
		//		asr::GenericMaterialFactory().create(
		//		areaLightMaterialName.asChar(),
		//		asr::ParamArray()
		//		.insert("surface_shader", physicalSurfaceName.asChar())
		//		.insert("edf", edfName.asChar())));

		//	asr::ParamArray objInstanceParamArray;
		//	addVisibilityFlags(obj, objInstanceParamArray);

		//	ass->object_instances().insert(
		//		asr::ObjectInstanceFactory::create(
		//		objectInstanceName.asChar(),
		//		objInstanceParamArray,
		//		meshPtr->get_name(),
		//		asf::Transformd::from_local_to_parent(appleMatrix),
		//		asf::StringDictionary()
		//		.insert("slot0", areaLightMaterialName.asChar()),
		//		asf::StringDictionary()
		//		.insert("slot0", "default")));

		//}
		//if ( light.get() != nullptr)
		//	lightAssembly->lights().insert(light);
	}

}

