#ifndef CORONA_SKY_MAP
#define CORONA_SKY_MAP

#include <maya/MObject.h>
#include "CoronaCore/api/Api.h"
#include "CoronaCore/misc/Shaders/SkyShader.h"

class CoronaRenderer;

class SkyMap : public Corona::Abstract::Map, Corona::SkyShader
{
public:
	CoronaRenderer *coronaRenderer;
	SkyMap();
	SkyMap(MObject sObject);
	~SkyMap();
	Corona::SkyParams *params;
	Corona::String mapName = "SkyMap";

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	void initSky();
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
	virtual Corona::Rgb sc(const int samples = 1000);
	virtual bool exportMap(Corona::IResourceManager& resourceManager, Corona::XmlWriterNode& outXml) const { return true; };
	virtual Corona::String name() const { return mapName; };

};


#endif