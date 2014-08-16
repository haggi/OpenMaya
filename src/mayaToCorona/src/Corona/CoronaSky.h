#include "CoronaCore/api/Api.h"
#include "CoronaCore/misc/Shaders/SkyShader.h"

class CoronaRenderer;

class SkyMap : public Corona::Abstract::Map, Corona::SkyShader
{
public:
	CoronaRenderer *coronaRenderer;
	SkyMap()
	{
		this->params = new Corona::SkyParams;
	};
	~SkyMap(){};
	Corona::SkyParams *params;

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	void initSky();
};

void SkyMap::initSky()
{
	this->init(this->params);
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
