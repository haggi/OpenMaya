#include "appleseed.h"

#include <maya/MFnTransform.h>

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"
#include "renderer/modeling/environmentedf/oslenvironmentedf.h"
#include "renderer/modeling/environmentedf/sphericalcoordinates.h"
#include "renderer/modeling/shadergroup/shadergroup.h"

#include "appleseedUtils.h"

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "mayaScene.h"
#include "world.h"

static Logging logger;

using namespace AppleRender;


void AppleseedRenderer::defineEnvironment()
{
	asr::Scene *scene = project->get_scene();
	// Create a color called "sky_radiance" and insert it into the scene.
	//static const float SkyRadiance[] = { 0.75f, 0.80f, 1.0f };
	//scene->colors().insert(
	//	asr::ColorEntityFactory::create(
	//	"sky_radiance",
	//	asr::ParamArray()
	//	.insert("color_space", "srgb")
	//	.insert("multiplier", "3.0"),
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

	MFnDependencyNode appleseedGlobals(getRenderGlobalsNode());
	MString textInstName = "bg_texture_inst";
	MString envColorName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";
	MStatus stat;
	float latlongHoShift = getFloatAttr("latlongHoShift", appleseedGlobals, 0.0f);
	float latlongVeShift = getFloatAttr("latlongVeShift", appleseedGlobals, 0.0f);
	int skyModel = getEnumInt("skyModel", appleseedGlobals);
	int environmentType = getEnumInt("environmentType", appleseedGlobals);
	float ground_albedo = getFloatAttr("ground_albedo", appleseedGlobals, 0.0f);
	float horizon_shift = getFloatAttr("horizon_shift", appleseedGlobals, 0.0f);
	float luminance_multiplier = getFloatAttr("luminance_multiplier", appleseedGlobals, 1.0f);
	float saturation_multiplier = getFloatAttr("saturation_multiplier", appleseedGlobals, 1.0f);
	float turbidity = getFloatAttr("turbidity", appleseedGlobals, 2.0f);
	float turbidity_max = getFloatAttr("turbidity_max", appleseedGlobals, 3.0f);
	float turbidity_min = getFloatAttr("turbidity_min", appleseedGlobals, 3.0f);
	int environmentSamples = getIntAttr("environmentSamples", appleseedGlobals, 1);
	bool physicalSun = getBoolAttr("physicalSun", appleseedGlobals, false);
	float sunTurbidity = getFloatAttr("sunTurbidity", appleseedGlobals, 3.0f);
	float sunExitanceMultiplier = getFloatAttr("sunExitanceMultiplier", appleseedGlobals, 1.0f);
	MColor environmentColorColor = getColorAttr("environmentColor", appleseedGlobals);
	MColor gradientHorizonColor = getColorAttr("gradientHorizon", appleseedGlobals);
	MColor gradientZenitColor = getColorAttr("gradientZenit", appleseedGlobals);
	float environmentIntensity = getFloatAttr("environmentIntensity", appleseedGlobals, 1.0f);

	defineColor(envColorName, environmentColorColor, environmentIntensity);
	defineColor(gradHorizName, gradientHorizonColor, environmentIntensity);
	defineColor(gradZenitName, gradientZenitColor, environmentIntensity);

	MString envMapAttrName = colorOrMap(appleseedGlobals, envMapName);

	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF;
	
	bool updateSkyShader = (scene->environment_shaders().get_by_name("sky_shader") != nullptr);
	bool updateSkyEdf = (scene->environment_edfs().get_by_name("sky_edf") != nullptr);

	switch (environmentType)
	{
	case 0: //constant
	{
		environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
				"sky_edf",
				asr::ParamArray()
				.insert("radiance", envColorName));
		break;
	}
	case 1: //ConstantHemisphere
	{
		environmentEDF = asr::ConstantHemisphereEnvironmentEDFFactory().create(
			"sky_edf",
			asr::ParamArray()
			.insert("radiance", envColorName)
			.insert("upper_hemi_radiance", gradHorizName)
			.insert("lower_hemi_radiance", gradHorizName));
		break;
	}
	case 2: //Gradient
	{
		environmentEDF = asr::GradientEnvironmentEDFFactory().create(
				"sky_edf",
				asr::ParamArray()
				.insert("horizon_radiance", gradHorizName)
				.insert("zenith_radiance", gradZenitName)
				);
		break;
	}
	case 3: //Latitude Longitude
	{
		environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
				"sky_edf",
				asr::ParamArray()
				.insert("radiance", envMapAttrName.asChar())
				.insert("radiance_multiplier", environmentIntensity)
				.insert("horizontal_shift", latlongHoShift)
				.insert("vertical_shift", latlongVeShift)
				);
		break;
	}
	case 4: //Mirror Ball
	{
		environmentEDF = asr::MirrorBallMapEnvironmentEDFFactory().create(
				"sky_edf",
				asr::ParamArray()
				.insert("radiance", envMapAttrName.asChar())
				.insert("radiance_multiplier", environmentIntensity));
		break;
	}
	case 5: //Physical Sky
	{
		asf::Vector3d unitVector(0.0, 0.0, 0.0);
		float sun_theta = getFloatAttr("sun_theta", appleseedGlobals, 1.0f);
		float sun_phi = getFloatAttr("sun_phi", appleseedGlobals, 1.0f);
		bool usePhysicalSun = getBoolAttr("physicalSun", appleseedGlobals, true);
		double theta = 90.0f - sun_theta, phi = sun_phi;

		if (usePhysicalSun)
		{	
			// get the connected sun light
			// physicalSunConnection
			MObject connectedNode = getConnectedInNode(getRenderGlobalsNode(), "physicalSunConnection");
			if ( connectedNode != MObject::kNullObj)
			{
				MFnTransform tn(connectedNode);
				MMatrix tm = tn.transformationMatrix(&stat);
				if( stat )
				{
					MVector sunOrient(0,0,1);
					sunOrient *= tm;
					sunOrient.normalize();
					unitVector.x = sunOrient.x;
					unitVector.y = sunOrient.y;
					unitVector.z = sunOrient.z;
					asr::unit_vector_to_angles(unitVector, theta, phi);						
					theta = 90.0f - RadToDeg(theta);
					theta = theta;
					sun_phi = RadToDeg(phi);
				}
			}else{
				Logging::warning("physicalSunConnection plug has no connection, but use physical sun is turned on. Please correct.");
			}
		}

		if(skyModel == 0) // preetham
		{

			environmentEDF = asr::PreethamEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("horizon_shift", horizon_shift)
					.insert("luminance_multiplier", luminance_multiplier)
					.insert("saturation_multiplier", saturation_multiplier)
					.insert("sun_phi", sun_phi)
					.insert("sun_theta", 90.0f - sun_theta)
					.insert("turbidity", turbidity)
					.insert("turbidity_max", turbidity_max)
					.insert("turbidity_min", turbidity_min));
		}else{ // hosek
			environmentEDF = asr::HosekEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("ground_albedo", ground_albedo)
					.insert("horizon_shift", horizon_shift)
					.insert("luminance_multiplier", luminance_multiplier)
					.insert("saturation_multiplier", saturation_multiplier)
					.insert("sun_phi", sun_phi)
					.insert("sun_theta", 90.0f - sun_theta)
					.insert("turbidity", turbidity)
					.insert("turbidity_max", turbidity_max)
					.insert("turbidity_min", turbidity_min));
		}

		break;
	}
	case 6: //OSL Sky
	{
		asf::auto_release_ptr<asr::ShaderGroup> oslShaderGroup = asr::ShaderGroupFactory().create("OSL_Sky");
		oslShaderGroup->add_shader("surface", "testBG", "BGLayer", asr::ParamArray());
		scene->shader_groups().insert(oslShaderGroup);
		environmentEDF = asr::OSLEnvironmentEDFFactory().create("sky_edf", asr::ParamArray()
			.insert("osl_background", "OSL_Sky")
			.insert("radiance_multiplier", environmentIntensity));

		break;
	}
	};

	asr::EnvironmentEDF *sky = scene->environment_edfs().get_by_name("sky_edf");
	if (sky != nullptr)
		scene->environment_edfs().remove(sky);

	scene->environment_edfs().insert(environmentEDF);

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
}


//void AppleseedRenderer::updateEnv(MObject mobj)
//{
//	MFnDependencyNode appleseedGlobals(mobj);
//
//	int envType = 0;
//	getEnum(MString("environmentType"), appleseedGlobals, envType);
//
//	MColor envColor;
//	getColor(MString("environmentColor"), appleseedGlobals,envColor);
//
//	MColor envMap;
//	getColor(MString("environmentMap"), appleseedGlobals, envMap);
//
//	float intens = 1.0f;
//	getFloat(MString("environmentIntensity"), appleseedGlobals, intens);
//
//	MColor grHoriz;
//	getColor(MString("gradientHorizon"), appleseedGlobals, grHoriz);
//
//	MColor grZeni;
//	getColor(MString("gradientZenit"), appleseedGlobals, grZeni);
//
//	//this->project->
//	MString envName = "environmentColor";
//	MString envMapName = "environmentMap";
//	MString gradHorizName = "gradientHorizon";
//	MString gradZenitName = "gradientZenit";
//
//	asr::ColorEntity *entity = nullptr;
//	entity = this->project->get_scene()->colors().get_by_name(envName.asChar());
//	if( entity != nullptr )
//	{
//		Logging::debug("Found envColor entity");
//		asr::ColorValueArray cva = entity->get_values();
//		cva[0] = envColor.r;
//		cva[1] = envColor.g;
//		cva[2] = envColor.b;
//	}
//}
