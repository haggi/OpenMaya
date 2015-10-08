#include "Corona.h"
#include "CoronaSky.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"
#include "CoronaUtils.h"

SkyMap::SkyMap()
{
	this->params = new Corona::SkyParams;
};

SkyMap::SkyMap(MObject sObject)
{
	MFnDependencyNode depFn(sObject);

	this->params = new Corona::SkyParams;

	params->multiplier = getFloatAttr("pSkyMultiplier", depFn, 1.0);
	int skyModel = getEnumInt("pSkyModel", depFn);
	if (skyModel == 0)
		params->mode = Corona::SkyModelType::MODEL_PREETHAM;
	if (skyModel == 1)
		params->mode = Corona::SkyModelType::MODEL_RAWAFAKE;
	if (skyModel == 2)
		params->mode = Corona::SkyModelType::MODEL_HOSEK;
	params->groundColor = toCorona(getColorAttr("pSkyGroundColor", depFn));
	params->horizonBlur = getFloatAttr("pSkyHorizBlur", depFn, .1f);
	params->skyAffectGround = getBoolAttr("pSkyAffectGround", depFn, true);
	params->preetham.turbidity = getFloatAttr("pSkyPreethamTurb", depFn, 2.5f);
	params->rawafake.horizon = toCorona(getColorAttr("pSkyHorizon", depFn));
	params->rawafake.horizon = toCorona(getColorAttr("pSkyZenith", depFn));
	params->rawafake.sunBleed = getFloatAttr("pSkySunBleed", depFn, 1.0f);
	params->rawafake.sunFalloff = getFloatAttr("pSkySunFalloff", depFn, 3.0f);
	params->rawafake.sunGlow = getFloatAttr("pSkySunGlow", depFn, 1.0f);
	params->rawafake.sunSideGlow = getFloatAttr("pSkySunSideGlow", depFn, .2f);

	this->initSky();
};

SkyMap::~SkyMap(){};

void SkyMap::initSky()
{
	std::shared_ptr<CoronaRenderer> renderer = std::static_pointer_cast<CoronaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	Corona::Sun sun = renderer->context.scene->getSun();
	this->init(this->params, &sun);
}

Corona::Rgb SkyMap::sc(const int samples)
{
	return this->avgSunColor(samples);
}

Corona::Rgb SkyMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	Corona::Dir d = context.getRay().direction;
	return evalSky(d);
}

float SkyMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir SkyMap::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP; //currently not supported
}

void SkyMap::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}

