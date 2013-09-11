#include "appleseed.h"
#include <maya/MFnLight.h>
#include <maya/MPlugArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include "../mtap_common/mtap_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"

static Logging logger;

using namespace AppleRender;

bool AppleseedRenderer::isSunLightTransform(mtap_MayaObject *obj)
{
	if(obj->dagPath.node().hasFn(MFn::kTransform))
	{
		MDagPath dp = obj->dagPath;
		dp.extendToShape();
		if(dp.node().hasFn(MFn::kDirectionalLight))
		{
		}
	}
	return false;
}

// it is a directional sunlight if it is connected to appleseedGlobals node
bool AppleseedRenderer::isSunLight(mtap_MayaObject *obj)
{
	if( !renderGlobals->physicalSun )
		return false;

	MStatus stat;
	MDagPath dp = obj->dagPath;
	
	// we need a transform node to check for message connections, so if we have a light shape, we have to go one level up
	if( !obj->mobject.hasFn(MFn::kTransform))
		dp.pop(1); // go one element up
	MFnDependencyNode depFn(dp.node(), &stat);
	
	if( stat )
	{
		MPlug msgPlug = depFn.findPlug("message", &stat);
		if( stat )
		{
			if( !msgPlug.isConnected())
			{
				return false;
			}
			MPlugArray plugArray;
			msgPlug.connectedTo(plugArray, 0, 1, &stat);
			if( stat )
			{
				MPlug destPlug = plugArray[0];
				//logger.debug(MString("AppleseedRenderer::isSunLight found destPlug: ") + destPlug.name());
				if(pystring::endswith(destPlug.name().asChar(), ".physicalSunConnection"))
				{
					//logger.debug(MString("AppleseedRenderer::isSunLight found a physicalSunConnection, this is a sun light."));
					return true;
				}
			}else{
				//logger.debug(MString("AppleseedRenderer::isSunLight Problem message connections "));
				return false;
			}
		}else{
			//logger.debug(MString("AppleseedRenderer::isSunLight Problem getting msg plug of node: ") + depFn.name());
			return false;
		}
	}else{
		//logger.debug(MString("AppleseedRenderer::isSunLight Problem getting dep node from dagpath: ") + dp.fullPathName());
		return false;
	}
	return false;
}


void AppleseedRenderer::defineLight(mtap_MayaObject *obj, asr::Assembly *ass, bool update)
{
	MStatus stat;
	
	mtap_MayaObject *mlight = obj;
	asf::Matrix4d appMatrix = asf::Matrix4d::identity();
	logger.debug(MString("Creating light: ") + mlight->shortName);
	MMatrix colMatrix = mlight->transformMatrices[0];
	this->MMatrixToAMatrix(colMatrix, appMatrix);

	MFnLight lightFn(mlight->mobject, &stat);
	if( !stat )
	{
		logger.error(MString("Could not get light info from ") + mlight->shortName);
		return;
	}

	float intensity = 1.0f;
	getFloat(MString("intensity"), lightFn, intensity);
	intensity *= 100 * this->renderGlobals->sceneScale;
		
	// multiplier 30 was the default value in the example
	MString colorAttribute = this->defineColor(lightFn, MString("color"), "srgb", 1.0f);

	asf::auto_release_ptr<asr::Light> light;
	if( mlight->mobject.hasFn(MFn::kSpotLight))
	{
		// redefinition because it is possible that this value is textured
		colorAttribute = defineColorAttributeWithTexture(lightFn, MString("color"), 1.0f);
		logger.debug(MString("Creating spotLight: ") + lightFn.name());
		float coneAngle = 45.0f;
		float penumbraAngle = 3.0f;
		getFloat(MString("coneAngle"), lightFn, coneAngle);
		getFloat(MString("penumbraAngle"), lightFn, penumbraAngle);
		coneAngle = (float)RadToDeg(coneAngle);
		penumbraAngle = (float)RadToDeg(penumbraAngle);
		float inner_angle = coneAngle;
		float outer_angle = coneAngle + penumbraAngle;
			
		// spot light is pointing in -z, appleseeds spot light is pointing in y, at least until next update...
		// I create a rotation matrix for this case.
		//MMatrix rotMatrix;
		//rotMatrix.setToIdentity();
		//MTransformationMatrix tm(rotMatrix);
		//MEulerRotation e(-90.0, 0.0, 0.0);
		//tm.rotateBy(e, MSpace::kWorld);
		//rotMatrix = tm.asMatrix();

		if( !update )
		{
			light = asf::auto_release_ptr<asr::Light>(
				asr::SpotLightFactory().create(
					mlight->shortName.asChar(),
					asr::ParamArray()
						.insert("radiance", colorAttribute.asChar())
						.insert("radiance_multiplier", intensity)
						.insert("inner_angle", inner_angle)
						.insert("outer_angle", outer_angle)
						));

			ass->lights().insert(light);		
		}else{
			asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
			pLight->get_parameters().insert("radiance", colorAttribute.asChar());
			pLight->get_parameters().insert("radiance_multiplier", intensity);
			pLight->get_parameters().insert("inner_angle", inner_angle);
			pLight->get_parameters().insert("outer_angle", outer_angle);
		}
	}
	if( mlight->mobject.hasFn(MFn::kDirectionalLight))
	{
		if( !update )
		{
			if( this->isSunLight(mlight))
			{
				logger.debug(MString("Found sunlight."));
				light = asf::auto_release_ptr<asr::Light>(
					asr::SunLightFactory().create(
					"sunLight",
					asr::ParamArray()
						.insert("environment_edf", "sky_edf")
						.insert("turbidity", renderGlobals->sunTurbidity)
						.insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f)
						));
			}else{
				light = asf::auto_release_ptr<asr::Light>(
					asr::DirectionalLightFactory().create(
						mlight->shortName.asChar(),
						asr::ParamArray()
							.insert("radiance", colorAttribute.asChar())
							.insert("radiance_multiplier", intensity)
							));
			}
			ass->lights().insert(light);
			
		}else{
			asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
			if( this->isSunLight(mlight))
			{
				pLight->get_parameters().insert("environment_edf", "sky_edf");
				pLight->get_parameters().insert("turbidity", renderGlobals->sunTurbidity);
				pLight->get_parameters().insert("radiance_multiplier", renderGlobals->sunExitanceMultiplier * intensity / 30.0f);
			}else{
				pLight->get_parameters().insert("radiance", colorAttribute.asChar());
				pLight->get_parameters().insert("radiance_multiplier", intensity);
			}
		}
	}

	if( mlight->mobject.hasFn(MFn::kPointLight))
	{

		if( !update )
		{
			light = asf::auto_release_ptr<asr::Light>(
				asr::PointLightFactory().create(
					mlight->shortName.asChar(),
					asr::ParamArray()
						.insert("radiance", colorAttribute.asChar())
						.insert("radiance_multiplier", intensity)
						));
			ass->lights().insert(light);
		}else{
			asr::Light *pLight = ass->lights().get_by_name(obj->shortName.asChar());
			pLight->get_parameters().insert("radiance", colorAttribute.asChar());
			pLight->get_parameters().insert("radiance_multiplier", intensity);
		}
	}
}

void AppleseedRenderer::defineDefaultLight()
{
	// default light will be created only if there are no other lights in the scene
	if( this->mtap_scene->lightList.size() > 0)
		return;
	if( !this->renderGlobals->createDefaultLight )
		return;
	
    // Create a color called "light_exitance" and insert it into the assembly.
    static const float LightExitance[] = { 1.0f, 1.0f, 1.0f };
	this->masterAssembly->colors().insert(
        asr::ColorEntityFactory::create(
            "light_exitance",
            asr::ParamArray()
                .insert("color_space", "srgb")
                .insert("multiplier", "30.0"),
            asr::ColorValueArray(3, LightExitance)));

    // Create a point light called "light" and insert it into the assembly.
    asf::auto_release_ptr<asr::Light> light(
        asr::PointLightFactory().create(
            "light",
            asr::ParamArray()
                .insert("radiance", "light_exitance")));
    light->set_transform(asf::Transformd::from_local_to_parent(
        asf::Matrix4d::translation(asf::Vector3d(0.6, 2.0, 1.0))));
    this->masterAssembly->lights().insert(light);
}

void AppleseedRenderer::updateLight(mtap_MayaObject *obj)
{	
	mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
	asr::Assembly *ass = NULL;
	if( att->assemblyObject && att->assemblyObject->objectAssembly)
		ass = att->assemblyObject->objectAssembly;

	if( ass != NULL )
	{
		logger.debug("Found LightAssembly, search for light...");

		asr::Light *light = NULL;
		light = ass->lights().get_by_name(obj->shortName.asChar());
		if( isSunLight(obj))
			light = ass->lights().get_by_name("sunLight");

		if( light == NULL )
		{
			logger.debug("Light not found in lightAssembly, creating one...");
			this->defineLight(obj, ass);
		}else{
			logger.debug("Light found in lightAssembly, updating...");
			this->defineLight(obj, ass, true);
		}
		
		MString lightAssemblyInst = obj->shortName + "assembly_inst";
		asr::AssemblyInstance *ai = this->masterAssembly->assembly_instances().get_by_name(lightAssemblyInst.asChar());
		if(ai != NULL)
		{

		}else{
			//logger.error("LightAssemblyInstance not found, something's wrong.");
		}
		// update instance
	}else{
		//logger.debug("LightAssembly not found, something's wrong.");
	}
}