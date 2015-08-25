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

//bool AppleseedRenderer::isSunLightTransform(std::shared_ptr<MayaObject> obj)
//{
//	if(obj->dagPath.node().hasFn(MFn::kTransform))
//	{
//		MDagPath dp = obj->dagPath;
//		dp.extendToShape();
//		if(dp.node().hasFn(MFn::kDirectionalLight))
//		{
//		}
//	}
//	return false;
//}

// it is a directional sunlight if it is connected to appleseedGlobals node
//bool AppleseedRenderer::isSunLight(std::shared_ptr<MayaObject> obj)
//{
//	if( !renderGlobals->physicalSun )
//		return false;
//
//	MStatus stat;
//	MDagPath dp = obj->dagPath;
//	
//	// we need a transform node to check for message connections, so if we have a light shape, we have to go one level up
//	if( !obj->mobject.hasFn(MFn::kTransform))
//		dp.pop(1); // go one element up
//	MFnDependencyNode depFn(dp.node(), &stat);
//	
//	if( stat )
//	{
//		MPlug msgPlug = depFn.findPlug("message", &stat);
//		if( stat )
//		{
//			if( !msgPlug.isConnected())
//			{
//				return false;
//			}
//			MPlugArray plugArray;
//			msgPlug.connectedTo(plugArray, 0, 1, &stat);
//			if( stat )
//			{
//				MPlug destPlug = plugArray[0];
//				//Logging::debug(MString("AppleseedRenderer::isSunLight found destPlug: ") + destPlug.name());
//				if(pystring::endswith(destPlug.name().asChar(), ".physicalSunConnection"))
//				{
//					//Logging::debug(MString("AppleseedRenderer::isSunLight found a physicalSunConnection, this is a sun light."));
//					return true;
//				}
//			}else{
//				//Logging::debug(MString("AppleseedRenderer::isSunLight Problem message connections "));
//				return false;
//			}
//		}else{
//			//Logging::debug(MString("AppleseedRenderer::isSunLight Problem getting msg plug of node: ") + depFn.name());
//			return false;
//		}
//	}else{
//		//Logging::debug(MString("AppleseedRenderer::isSunLight Problem getting dep node from dagpath: ") + dp.fullPathName());
//		return false;
//	}
//	return false;
//}


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

		asr::Assembly *lightAssembly = getCreateObjectAssembly(obj.get());

		MFnDependencyNode depFn(obj->mobject);
		asf::auto_release_ptr<asr::Light> light;

		if (obj->mobject.hasFn(MFn::kPointLight))
		{
			bool cast_indirect_light = getBoolAttr("mtap_cast_indirect_light", depFn, true);
			float importance_multiplier = getFloatAttr("mtap_importance_multiplier", depFn, 1.0f);
			MColor col = getColorAttr("color", depFn);
			float intensity = getFloatAttr("intensity", depFn, 1.0f);
			MString colorAttribute = obj->shortName + "_intensity";
			defineColor(colorAttribute, col, intensity);
			int decay = getEnumInt("decayRate", depFn);
			light = asf::auto_release_ptr<asr::Light>(
				asr::PointLightFactory().create(
					obj->shortName.asChar(),
					asr::ParamArray()
					.insert("intensity", colorAttribute)
					.insert("intensity_multiplier", intensity)
					.insert("importance_multiplier", importance_multiplier)
					.insert("cast_indirect_light", cast_indirect_light)
					));
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

			light = asf::auto_release_ptr<asr::Light>(
				asr::SpotLightFactory().create(
					obj->shortName.asChar(),
					asr::ParamArray()
					.insert("radiance", colorAttribute)
						.insert("radiance_multiplier", intensity)
						.insert("inner_angle", inner_angle)
						.insert("outer_angle", outer_angle)
						.insert("importance_multiplier", importance_multiplier)
						.insert("cast_indirect_light", cast_indirect_light)
						));
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

			if( isSunLight(obj->mobject))
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
			}else{
				light = asf::auto_release_ptr<asr::Light>(
					asr::DirectionalLightFactory().create(
						obj->shortName.asChar(),
						asr::ParamArray()
						.insert("irradiance", colorAttribute)
						.insert("irradiance_multiplier", intensity)
						.insert("importance_multiplier", importance_multiplier)
						.insert("cast_indirect_light", cast_indirect_light)
						));
			}
		}

		if (obj->mobject.hasFn(MFn::kAreaLight))
		{
			MString areaLightName = obj->fullNiceName;
			asf::auto_release_ptr<asr::MeshObject> plane = defineStandardPlane();
			plane->set_name(areaLightName.asChar());
			MayaObject *assemblyObject = getAssemblyMayaObject(obj.get());
			asr::Assembly *ass = getCreateObjectAssembly(obj.get());
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

		}
		if ( light.get() != nullptr)
			lightAssembly->lights().insert(light);
	}
	//std::shared_ptr<MayaObject> mlight = obj;
	//asf::Matrix4d appMatrix = asf::Matrix4d::identity();
	//Logging::debug(MString("Creating light: ") + mlight->shortName);
	//MMatrix colMatrix = mlight->transformMatrices[0];
	//this->MMatrixToAMatrix(colMatrix, appMatrix);

	//MFnLight lightFn(mlight->mobject, &stat);
	//if( !stat )
	//{
	//	Logging::error(MString("Could not get light info from ") + mlight->shortName);
	//	return;
	//}

	//float intensity = 1.0f;
	//getFloat(MString("intensity"), lightFn, intensity);
	//intensity *= 100 * this->renderGlobals->sceneScale;

	//// multiplier 30 was the default value in the example
	//MString colorAttribute = this->defineColor(lightFn, MString("color"), "srgb", 1.0f);

	//asf::auto_release_ptr<asr::Light> light;
	//if( mlight->mobject.hasFn(MFn::kSpotLight))
	//{
	//	// redefinition because it is possible that this value is textured
	//	colorAttribute = defineColorAttributeWithTexture(lightFn, MString("color"), 1.0f);
	//	Logging::debug(MString("Creating spotLight: ") + lightFn.name());
	//	float coneAngle = 45.0f;
	//	float penumbraAngle = 3.0f;
	//	getFloat(MString("coneAngle"), lightFn, coneAngle);
	//	getFloat(MString("penumbraAngle"), lightFn, penumbraAngle);
	//	coneAngle = (float)RadToDeg(coneAngle);
	//	penumbraAngle = (float)RadToDeg(penumbraAngle);
	//	float inner_angle = coneAngle;
	//	float outer_angle = coneAngle + penumbraAngle;

	//	if( !update )
	//	{
	//		light = asf::auto_release_ptr<asr::Light>(
	//			asr::SpotLightFactory().create(
	//				mlight->shortName.asChar(),
	//				asr::ParamArray()
	//					.insert("radiance", colorAttribute.asChar())
	//					.insert("radiance_multiplier", intensity)
	//					.insert("inner_angle", inner_angle)
	//					.insert("outer_angle", outer_angle)
	//					));

	//		ass->lights().insert(light);		
	//	}else{
	//		asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
	//		pLight->get_parameters().insert("radiance", colorAttribute.asChar());
	//		pLight->get_parameters().insert("radiance_multiplier", intensity);
	//		pLight->get_parameters().insert("inner_angle", inner_angle);
	//		pLight->get_parameters().insert("outer_angle", outer_angle);
	//	}
	//}
	//if( mlight->mobject.hasFn(MFn::kDirectionalLight))
	//{
	//	if( !update )
	//	{
	//		if( this->isSunLight(mlight))
	//		{
	//			Logging::debug(MString("Found sunlight."));
	//			light = asf::auto_release_ptr<asr::Light>(
	//				asr::SunLightFactory().create(
	//				"sunLight",
	//				asr::ParamArray()
	//					.insert("environment_edf", "sky_edf")
	//					.insert("turbidity", renderGlobals->sunTurbidity)
	//					.insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f)
	//					));
	//		}else{
	//			light = asf::auto_release_ptr<asr::Light>(
	//				asr::DirectionalLightFactory().create(
	//					mlight->shortName.asChar(),
	//					asr::ParamArray()
	//						.insert("radiance", colorAttribute.asChar())
	//						.insert("radiance_multiplier", intensity)
	//						));
	//		}
	//		ass->lights().insert(light);
	//		
	//	}else{
	//		asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
	//		if( this->isSunLight(mlight))
	//		{
	//			pLight->get_parameters().insert("environment_edf", "sky_edf");
	//			pLight->get_parameters().insert("turbidity", renderGlobals->sunTurbidity);
	//			pLight->get_parameters().insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f);
	//		}else{
	//			pLight->get_parameters().insert("radiance", colorAttribute.asChar());
	//			pLight->get_parameters().insert("radiance_multiplier", intensity);
	//		}
	//	}
	//}

	//if( mlight->mobject.hasFn(MFn::kPointLight))
	//{

	//	if( !update )
	//	{
	//		light = asf::auto_release_ptr<asr::Light>(
	//			asr::PointLightFactory().create(
	//				mlight->shortName.asChar(),
	//				asr::ParamArray()
	//					.insert("radiance", colorAttribute.asChar())
	//					.insert("radiance_multiplier", intensity)
	//					));
	//		ass->lights().insert(light);
	//	}else{
	//		asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
	//		pLight->get_parameters().insert("radiance", colorAttribute.asChar());
	//		pLight->get_parameters().insert("radiance_multiplier", intensity);
	//	}
	//}
}

//void AppleseedRenderer::defineDefaultLight()
//{
//	// default light will be created only if there are no other lights in the scene
//	if( this->mtap_scene->lightList.size() > 0)
//		return;
//	if( !this->renderGlobals->createDefaultLight )
//		return;
//	
//    // Create a color called "light_exitance" and insert it into the assembly.
//    static const float LightExitance[] = { 1.0f, 1.0f, 1.0f };
//	this->masterAssembly->colors().insert(
//        asr::ColorEntityFactory::create(
//            "light_exitance",
//            asr::ParamArray()
//                .insert("color_space", "srgb")
//                .insert("multiplier", "30.0"),
//            asr::ColorValueArray(3, LightExitance)));
//
//    // Create a point light called "light" and insert it into the assembly.
//    asf::auto_release_ptr<asr::Light> light(
//        asr::PointLightFactory().create(
//            "light",
//            asr::ParamArray()
//                .insert("radiance", "light_exitance")));
//    light->set_transform(asf::Transformd::from_local_to_parent(
//        asf::Matrix4d::translation(asf::Vector3d(0.6, 2.0, 1.0))));
//    this->masterAssembly->lights().insert(light);
//}
//
//void AppleseedRenderer::updateLight(std::shared_ptr<MayaObject> obj)
//{	
//	std::shared_ptr<ObjectAttributes>att = (std::shared_ptr<ObjectAttributes>)obj->attributes;
//	asr::Assembly *ass = nullptr;
//	if( att->assemblyObject && att->assemblyObject->objectAssembly)
//		ass = att->assemblyObject->objectAssembly;
//
//	if( ass != nullptr )
//	{
//		Logging::debug("Found LightAssembly, search for light...");
//
//		asr::Light *light = nullptr;
//		light = ass->lights().get_by_name(obj->shortName.asChar());
//		if( isSunLight(obj))
//			light = ass->lights().get_by_name("sunLight");
//
//		if( light == nullptr )
//		{
//			Logging::debug("Light not found in lightAssembly, creating one...");
//			this->defineLight(obj, ass);
//		}else{
//			Logging::debug("Light found in lightAssembly, updating...");
//			this->defineLight(obj, ass, true);
//		}
//		
//		MString lightAssemblyInst = obj->shortName + "assembly_inst";
//		asr::AssemblyInstance *ai = this->masterAssembly->assembly_instances().get_by_name(lightAssemblyInst.asChar());
//		if(ai != nullptr)
//		{
//
//		}else{
//			//Logging::error("LightAssemblyInstance not found, something's wrong.");
//		}
//		// update instance
//	}else{
//		//Logging::debug("LightAssembly not found, something's wrong.");
//	}
//}
