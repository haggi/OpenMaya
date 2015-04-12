#include "appleseed.h"

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"
#include "renderer/modeling/environmentedf/sphericalcoordinates.h"

#include <maya/MFnTransform.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "../mtap_common/mtap_mayaScene.h"

static Logging logger;

using namespace AppleRender;


void AppleseedRenderer::defineEnvironment(mtap_RenderGlobals *renderGlobals)
{
	//------------------------------------------------------------------------
    // Environment
    //------------------------------------------------------------------------

	//stat = eAttr.addField( "Constant", 0 );
	//stat = eAttr.addField( "Gradient", 1 );
	//stat = eAttr.addField( "Latitude Longitude", 2 );
	//stat = eAttr.addField( "Mirror Ball", 3 );

	MString textInstName = "bg_texture_inst";
	renderGlobals->environmentColor.a = 0.0f;
	renderGlobals->gradientHorizon.a = 0.0f;
	renderGlobals->gradientZenit.a = 0.0f;
	MString envName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";
	MObject rGlobalsObject = objectFromName(MString("appleseedGlobals"));
	MStatus stat;
	MFnDependencyNode appleseedGlobals(rGlobalsObject, &stat);
	if( stat )
	{
		getFloat(MString("latlongHoShift"), appleseedGlobals, renderGlobals->latlongHoShift);		
		getFloat(MString("latlongVeShift"), appleseedGlobals, renderGlobals->latlongVeShift);
		getEnum(MString("skyModel"), appleseedGlobals, renderGlobals->skyModel);
		getFloat(MString("ground_albedo"), appleseedGlobals, renderGlobals->ground_albedo);
		getFloat(MString("horizon_shift"), appleseedGlobals, renderGlobals->horizon_shift);
		getFloat(MString("luminance_multiplier"), appleseedGlobals, renderGlobals->luminance_multiplier);
		getFloat(MString("saturation_multiplier"), appleseedGlobals, renderGlobals->saturation_multiplier);
		getFloat(MString("turbidity"), appleseedGlobals, renderGlobals->turbidity);
		getFloat(MString("turbidity_max"), appleseedGlobals, renderGlobals->turbidity_max);
		getFloat(MString("turbidity_min"), appleseedGlobals, renderGlobals->turbidity_min);
		getInt(MString("environmentSamples"), appleseedGlobals, renderGlobals->environmentSamples);
		getBool(MString("physicalSun"), appleseedGlobals, renderGlobals->physicalSun);
		getFloat(MString("sunTurbidity"), appleseedGlobals, renderGlobals->sunTurbidity);
		getFloat(MString("sunExitanceMultiplier"), appleseedGlobals, renderGlobals->sunExitanceMultiplier);
	}
	this->defineColor(envName, renderGlobals->environmentColor, renderGlobals->environmentIntensity);
	this->defineColor(gradHorizName, renderGlobals->gradientHorizon, renderGlobals->environmentIntensity);
	this->defineColor(gradZenitName, renderGlobals->gradientZenit, renderGlobals->environmentIntensity);

	MFnDependencyNode globalsFn(renderGlobals->renderGlobalsMobject);

	MString envMapAttrName = this->defineColorAttributeWithTexture(globalsFn, envMapName);
	int skyModel = this->renderGlobals->skyModel;

	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF;
	
	bool doUpdate = false;
	if(this->scenePtr->environment_shaders().get_by_name("sky_shader") != nullptr)
	{
		 //this->scenePtr->environment_shaders().remove(this->scenePtr->environment_shaders().get_by_name("sky_shader"));
		 doUpdate = true;
	}

	if( this->scenePtr->environment_edfs().get_by_name("sky_edf")  != nullptr)
	{
		 //this->scenePtr->environment_edfs().remove(this->scenePtr->environment_edfs().get_by_name("sky_edf"));
		 doUpdate = true;
	}

	switch(renderGlobals->environmentType){
	case 0:
		if( doUpdate )
		{
			this->scenePtr->environment_edfs().get_by_name("sky_edf")->get_parameters().insert("radiance", envName);
		}else{
			environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("radiance", envName));
		}
		break;
	case 1:
		if( doUpdate )
		{
			this->scenePtr->environment_edfs().get_by_name("sky_edf")->get_parameters().insert("radiance", envName);
		}else{
			environmentEDF = asr::GradientEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("horizon_radiance", gradHorizName)
					.insert("zenith_radiance", gradZenitName)
					);
		}
		break;
	case 2:
            //m_u_shift = m_params.get_optional<double>("horizontal_shift", 0.0) / 360.0;
            //m_v_shift = m_params.get_optional<double>("vertical_shift", 0.0) / 360.0;
		if( doUpdate )
		{
			this->scenePtr->environment_edfs().get_by_name("sky_edf")->get_parameters()
				.insert("radiance", envMapAttrName.asChar())
				.insert("radiance_multiplier", renderGlobals->environmentIntensity)
				.insert("horizontal_shift", renderGlobals->latlongHoShift)
				.insert("vertical_shift", renderGlobals->latlongVeShift);
		}else{
			environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("radiance", envMapAttrName.asChar())
					.insert("radiance_multiplier", renderGlobals->environmentIntensity)
					.insert("horizontal_shift", renderGlobals->latlongHoShift)
					.insert("vertical_shift", renderGlobals->latlongVeShift)
					);
		}
		break;
	case 3:
		if( doUpdate )
		{
			this->scenePtr->environment_edfs().get_by_name("sky_edf")->get_parameters()
					.insert("radiance", envMapAttrName.asChar())
					.insert("radiance_multiplier", renderGlobals->environmentIntensity);
		}else{
			environmentEDF = asr::MirrorBallMapEnvironmentEDFFactory().create(
					"sky_edf",
					asr::ParamArray()
					.insert("radiance", envMapAttrName.asChar())
					.insert("radiance_multiplier", renderGlobals->environmentIntensity));
		}
		break;
	case 4: // physical sky
		{
			asf::Vector3d unitVector(0.0, 0.0, 0.0);
			double theta = 90.0f - renderGlobals->sun_theta, phi = renderGlobals->sun_phi;

			if( renderGlobals->physicalSun )
			{	
				// get the connected sun light
				// physicalSunConnection
				MObject rGlobals = renderGlobals->getRenderGlobalsNode();
				MFnDependencyNode gFn(rGlobals);
				MPlug physicalSunConnectionPlug = gFn.findPlug("physicalSunConnection");
				if( physicalSunConnectionPlug.isConnected() )
				{
					MStatus stat;
					MObject lightNode = getOtherSideNode(physicalSunConnectionPlug);
					MFnTransform tn(lightNode);
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
						renderGlobals->sun_theta = theta;
						renderGlobals->sun_phi = RadToDeg(phi);

						Logging::debug(MString("converted unit to theta: ") + renderGlobals->sun_theta + " and phi: " + renderGlobals->sun_phi);
					}else{
						Logging::warning(MString("Problems creating tmatrix from: ") + getObjectName(lightNode));
					}
				}else{
					Logging::warning("physicalSunConnection plug has no connection, but use physical sun is turned on, please correct.");
				}
			}

			asr::EnvironmentEDF *sky = this->scenePtr->environment_edfs().get_by_name("sky_edf");
			if( sky != nullptr )
				this->scenePtr->environment_edfs().remove(sky);

			if(skyModel == 0) // preetham
			{

				environmentEDF = asr::PreethamEnvironmentEDFFactory().create(
						"sky_edf",
						asr::ParamArray()
						.insert("horizon_shift", renderGlobals->horizon_shift)
						.insert("luminance_multiplier", renderGlobals->luminance_multiplier)
						.insert("saturation_multiplier", renderGlobals->saturation_multiplier)
						.insert("sun_phi", renderGlobals->sun_phi)
						.insert("sun_theta", 90.0f - renderGlobals->sun_theta)
						.insert("turbidity", renderGlobals->turbidity)
						.insert("turbidity_max", renderGlobals->turbidity_max)
						.insert("turbidity_min", renderGlobals->turbidity_min));
			}else{ // hosek
				environmentEDF = asr::HosekEnvironmentEDFFactory().create(
						"sky_edf",
						asr::ParamArray()
						.insert("ground_albedo", renderGlobals->ground_albedo)
						.insert("horizon_shift", renderGlobals->horizon_shift)
						.insert("luminance_multiplier", renderGlobals->luminance_multiplier)
						.insert("saturation_multiplier", renderGlobals->saturation_multiplier)
						.insert("sun_phi", renderGlobals->sun_phi)
						.insert("sun_theta", 90.0f - renderGlobals->sun_theta)
						.insert("turbidity", renderGlobals->turbidity)
						.insert("turbidity_max", renderGlobals->turbidity_max)
						.insert("turbidity_min", renderGlobals->turbidity_min));
			}
		}
		break;
	default:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("radiance", envName));
	}

	if( !doUpdate)
	{
		this->scenePtr->environment_edfs().insert(environmentEDF);


		// Create an environment shader called "sky_shader" and insert it into the scene.
		this->scenePtr->environment_shaders().insert(
			asr::EDFEnvironmentShaderFactory().create(
				"sky_shader",
				asr::ParamArray()
					.insert("environment_edf", "sky_edf")));

		// Create an environment called "sky" and bind it to the scene.
		this->scenePtr->set_environment(
			asr::EnvironmentFactory::create(
				"sky",
				asr::ParamArray()
					.insert("environment_edf", "sky_edf")
					.insert("environment_shader", "sky_shader")));	
	}
}


void AppleseedRenderer::updateEnv(MObject mobj)
{
	MFnDependencyNode appleseedGlobals(mobj);

	int envType = 0;
	getEnum(MString("environmentType"), appleseedGlobals, envType);

	MColor envColor;
	getColor(MString("environmentColor"), appleseedGlobals,envColor);

	MColor envMap;
	getColor(MString("environmentMap"), appleseedGlobals, envMap);

	float intens = 1.0f;
	getFloat(MString("environmentIntensity"), appleseedGlobals, intens);

	MColor grHoriz;
	getColor(MString("gradientHorizon"), appleseedGlobals, grHoriz);

	MColor grZeni;
	getColor(MString("gradientZenit"), appleseedGlobals, grZeni);

	//this->project->
	MString envName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";

	asr::ColorEntity *entity = nullptr;
	entity = this->project->get_scene()->colors().get_by_name(envName.asChar());
	if( entity != nullptr )
	{
		Logging::debug("Found envColor entity");
		asr::ColorValueArray cva = entity->get_values();
		cva[0] = envColor.r;
		cva[1] = envColor.g;
		cva[2] = envColor.b;
	}
}
