#ifndef CORONA_AO_MAP
#define CORONA_AO_MAP

#include "CoronaCore/api/Api.h"
#include "CoronaCore/misc/Shaders/AoShader.h"

class AoMap : public Corona::Abstract::Map, Corona::AoShader
{
public:
	AoMap(MObject sObject);
	~AoMap();
	float maxDist;

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
};


#endif