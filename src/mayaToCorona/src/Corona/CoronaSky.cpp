#include "Corona.h"
#include "CoronaSky.h"

SkyMap::SkyMap()
{
	this->params = new Corona::SkyParams;
};
SkyMap::~SkyMap(){};

void SkyMap::initSky()
{
	Corona::Sun sun = this->coronaRenderer->context.scene->getSun();
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

