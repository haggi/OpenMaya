#include "Lux.h"
#include <maya/MFnLight.h>
#include <maya/MPlugArray.h>

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"

#include "utilities/logging.h"
static Logging logger;

// it is a directional sunlight if it is connected to luxGlobals node
bool LuxRenderer::isSunLight(mtlu_MayaObject *obj)
{
	if( !this->mtlu_renderGlobals->usePhysicalSky )
		return false;

	MStatus stat;
	MDagPath dp = obj->dagPath;
	dp.pop(1); // go one element up
	MFnDependencyNode depFn(dp.node(), &stat);
	
	if( stat )
	{
		MPlug msgPlug = depFn.findPlug("message", &stat);
		if( stat )
		{
			if( !msgPlug.isConnected())
				return false;
			MPlugArray plugArray;
			msgPlug.connectedTo(plugArray, 0, 1, &stat);
			if( stat )
			{
				MPlug destPlug = plugArray[0];
				logger.debug(MString("LuxRenderer::isSunLight found destPlug: ") + destPlug.name());
				if(pystring::endswith(destPlug.name().asChar(), ".physicalSunConnection"))
				{
					logger.debug(MString("LuxRenderer::isSunLight found a physicalSunConnection, this is a sun light."));
					return true;
				}
			}else{
				logger.debug(MString("LuxRenderer::isSunLight Problem message connections "));
				return false;
			}
		}else{
			logger.debug(MString("LuxRenderer::isSunLight Problem getting msg plug of node: ") + depFn.name());
			return false;
		}
	}else{
		logger.debug(MString("LuxRenderer::isSunLight Problem getting dep node from dagpath: ") + dp.fullPathName());
		return false;
	}
	return false;
}

void LuxRenderer::defineAreaLight(mtlu_MayaObject *obj)
{
	MFnLight lightFn(obj->mobject);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;

	int nsamples;
	getInt(MString("mtlu_areaLight_samples"), lightFn, nsamples);
	float power = 1.0f;
	getFloat(MString("mtlu_areaLight_power"), lightFn, power);
	float efficacy = 17.0f;
	getFloat(MString("mtlu_areaLight_efficacy"), lightFn, efficacy);
	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);
	MString ies;
	getString(MString("mtlu_areaLight_ies"), lightFn, ies);
	const std::string iesname = ies.asChar();

	ParamSet lp = CreateParamSet();
	lp->AddRGBColor("L", L, 1);
	lp->AddFloat("gain", &gain, 1);
	lp->AddFloat("power", &power, 1);
	lp->AddFloat("efficacy", &efficacy, 1);
	lp->AddInt("nsamples", &nsamples, 1);
	if( ies.length() > 0)
		lp->AddString("iesname", &iesname, 1);

	//lux->areaLightSource("area",  boost::get_pointer(lp));

	createAreaLightMesh(obj);

	//lux->transformBegin();
	//{
	//	float fm[16];
	//	MMatrix tm = obj->transformMatrices[0];
	//	setZUp(tm, fm);
	//	this->lux->transform(fm);
	//	ParamSet sp = CreateParamSet();
	//	float r = 2.0f;
	//	sp->AddFloat("radius", &r, 1); 
	//	lux->shape("sphere", boost::get_pointer(sp));
	//}
	//lux->transformEnd();
}


void LuxRenderer::defineDirectionalLight(mtlu_MayaObject *obj)
{
	MFnLight lightFn(obj->mobject);
	float from[3]= {0,20,50};
	float to[3]= {0,0,0};
	MVector lightDir(0,0,1);
	lightDir *= obj->dagPath.inclusiveMatrix();
	logger.debug(MString("Translating directional light ") + obj->shortName);
	logger.debug(MString("from orig ") + lightDir.x + " " + lightDir.y + " " + lightDir.z);
	setZUp(lightDir);
	from[0] = lightDir.x;
	from[1] = lightDir.y;
	from[2] = lightDir.z;
	logger.debug(MString("from up ") + lightDir.x + " " + lightDir.y + " " + lightDir.z);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;

	float theta = 0.0f;
	getFloat(MString("mtlu_dirLight_theta"), lightFn, theta);
	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);
	ParamSet lp = CreateParamSet();
	lp->AddPoint("from", from, 1);
	lp->AddPoint("to", to, 1);
	lp->AddRGBColor("L", L, 1);
	lp->AddFloat("theta", &theta, 1);
	lp->AddFloat("gain", &gain, 1);

	lux->lightSource("distant", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"distant\" \"point from\" [" << from[0]<<" " << from[1]<<" " << from[2]<<"] \"point to\" [" << to[0]<<" " << to[1]<<" " << to[2]<<"]\n"; 
}

void LuxRenderer::definePointLight(mtlu_MayaObject *obj)
{
	MFnLight lightFn(obj->mobject);
	logger.debug(MString("Translating point light ") + obj->shortName);

	float from[3];
	MMatrix m = obj->dagPath.inclusiveMatrix();
	MPoint p;
	p.x = m[3][0];
	p.y = m[3][1];
	p.z = m[3][2];
	setZUp(p, from);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;
	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);
	float power = 100.0f;
	getFloat(MString("mtlu_pointLight_power"), lightFn, power);
	float efficacy = 17.0f;
	getFloat(MString("mtlu_pointLight_efficacy"), lightFn, efficacy);

	ParamSet lp = CreateParamSet();
	lp->AddPoint("from", from, 1);
	lp->AddRGBColor("L", L, 1);
	lp->AddFloat("gain", &gain, 1);
	lp->AddFloat("power", &power, 1);
	lp->AddFloat("efficacy", &efficacy, 1);

	lux->lightSource("point", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"point\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineSpotLight(mtlu_MayaObject *obj)
{
	MFnLight lightFn(obj->mobject);
	logger.debug(MString("Translating spot light ") + obj->shortName);

	float from[3];
	float to[3];
	MMatrix m = obj->dagPath.inclusiveMatrix();
	MPoint p;
	p.x = m[3][0];
	p.y = m[3][1];
	p.z = m[3][2];
	setZUp(p, from);
	MPoint lightDir(0,0,-1);
	lightDir *= obj->dagPath.inclusiveMatrix();
	setZUp(lightDir, to);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;
	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);
	float power = 100.0f;
	getFloat(MString("mtlu_spotLight_power"), lightFn, power);
	float efficacy = 17.0f;
	getFloat(MString("mtlu_spotLight_efficacy"), lightFn, efficacy);
	float coneAngle = 40.0f;
	getFloat(MString("coneAngle"), lightFn, coneAngle);
	coneAngle = (float)RadToDeg(coneAngle);
	float penumbraAngle = 0.0;
	getFloat(MString("penumbraAngle"), lightFn, penumbraAngle);
	penumbraAngle = (float)RadToDeg(penumbraAngle);

	ParamSet lp = CreateParamSet();
	lp->AddPoint("from", from, 1);
	lp->AddPoint("to", to, 1);
	lp->AddRGBColor("L", L, 1);
	lp->AddFloat("gain", &gain, 1);
	lp->AddFloat("power", &power, 1);
	lp->AddFloat("efficacy", &efficacy, 1);
	lp->AddFloat("coneangle", &coneAngle, 1);
	lp->AddFloat("conedeltaangle", &penumbraAngle, 1);

	lux->lightSource("spot", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"spot\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineSunLight(mtlu_MayaObject *obj)
{
	logger.debug(MString("Translating sun light ") + obj->shortName);
	MFnLight lightFn(obj->mobject);
	MVector sunDir(0,0,1);
	sunDir *= obj->dagPath.inclusiveMatrix();
	float sundir[3];
	setZUp(sunDir, sundir);

	float relsize = this->mtlu_renderGlobals->sunRelSize;
	float turbidity = this->mtlu_renderGlobals->turbidity;
	int nsamples = this->mtlu_renderGlobals->skySamples;

	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);

	ParamSet lp = CreateParamSet();
	lp->AddVector("sundir", sundir, 1);
	lp->AddFloat("gain", &gain, 1);
	lp->AddInt("nsamples", &nsamples, 1);
	lp->AddFloat("turbidity", &turbidity, 1);
	lp->AddFloat("relsize", &relsize, 1);

	// three cases
	// 1) no physical sky but sun
	// 2) physical sky but no physical sun
	// 3) physical sky and physical sun

	if( !this->mtlu_renderGlobals->usePhysicalSky )
	{
		lux->lightSource("sun", boost::get_pointer(lp));
	}else{
		if( this->mtlu_renderGlobals->physicalSun )
			lux->lightSource("sunsky2", boost::get_pointer(lp));
		else
			lux->lightSource("sky2", boost::get_pointer(lp));
	}

	//if( this->mtlu_renderGlobals->exportSceneFile)
	//	this->luxFile << "LightSource \"distant\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineEnvironmentLight(mtlu_MayaObject *obj)
{

	MFnLight lightFn(obj->mobject);
	logger.debug(MString("Translating environment light ") + obj->shortName);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;

	MString mapName;
	getString(MString("mtlu_ambientLight_map"), lightFn, mapName);
	float gain = 1.0f;
	getFloat(MString("intensity"), lightFn, gain);
	int nsamples = 1;
	getInt(MString("mtlu_ambientLight_samples"), lightFn, nsamples);
	const char *mapname = mapName.asChar();

	ParamSet lp = CreateParamSet();
	lp->AddRGBColor("L", L, 1);
	lp->AddFloat("gain", &gain, 1);
	lp->AddInt("nsamples", &nsamples, 1);
	if( mapName.length() > 0)
		lp->AddString("mapname", &mapname, 1);

	lux->lightSource("infinite", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"infinite\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineLights()
{
	for( size_t lightId = 0; lightId < this->mtlu_scene->lightList.size(); lightId++)
	{
		mtlu_MayaObject *obj = (mtlu_MayaObject *)this->mtlu_scene->lightList[lightId];

		if( !obj->visible )
			continue;

		if( obj->mobject.hasFn(MFn::kDirectionalLight))
		{
			if( isSunLight(obj) )
				this->defineSunLight(obj);
			else
				this->defineDirectionalLight(obj);
		}

		if( obj->mobject.hasFn(MFn::kPointLight))
		{
			this->definePointLight(obj);
		}

		if( obj->mobject.hasFn(MFn::kSpotLight))
		{
			this->defineSpotLight(obj);
		}

		if( obj->mobject.hasFn(MFn::kAmbientLight))
		{
			this->defineEnvironmentLight(obj);
		}

		if( obj->mobject.hasFn(MFn::kAreaLight))
		{
			this->defineAreaLight(obj);
		}
	}
}
