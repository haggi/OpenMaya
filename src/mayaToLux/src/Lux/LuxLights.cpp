#include "Lux.h"
#include <maya/MFnLight.h>

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "utilities/logging.h"
static Logging logger;

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
	lp->AddPoint("from", from, 3);
	lp->AddPoint("to", to, 3);
	lp->AddRGBColor("l", L, 3);
	lp->AddFloat("theta", &theta, 1);
	lp->AddFloat("gain", &gain, 1);

	lux->lightSource("distant", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"distant\" \"color L\" [" << "\n"; 
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
	lp->AddPoint("from", from, 3);
	lp->AddRGBColor("l", L, 3);
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
	lp->AddPoint("from", from, 3);
	lp->AddPoint("to", to, 3);
	lp->AddRGBColor("l", L, 3);
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
	lp->AddPoint("from", from, 3);
	lp->AddPoint("to", to, 3);
	lp->AddRGBColor("l", L, 3);
	lp->AddFloat("theta", &theta, 1);
	lp->AddFloat("gain", &gain, 1);

	lux->lightSource("distant", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"distant\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineEnvironmentLight(mtlu_MayaObject *obj)
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
	lp->AddPoint("from", from, 3);
	lp->AddPoint("to", to, 3);
	lp->AddRGBColor("l", L, 3);
	lp->AddFloat("theta", &theta, 1);
	lp->AddFloat("gain", &gain, 1);

	lux->lightSource("distant", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"distant\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineLights()
{
	for( size_t lightId = 0; lightId < this->mtlu_scene->lightList.size(); lightId++)
	{
		mtlu_MayaObject *obj = (mtlu_MayaObject *)this->mtlu_scene->lightList[lightId];

		if( obj->mobject.hasFn(MFn::kDirectionalLight))
		{
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
	}
}
