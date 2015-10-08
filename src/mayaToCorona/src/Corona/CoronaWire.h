#ifndef CORONA_WIRE_MAP
#define CORONA_WIRE_MAP

#include <maya/MObject.h>
#include "CoronaCore/api/Api.h"
#include "CoronaCore/misc/Shaders/WireShader.h"

class WireMap : public Corona::Abstract::Map, Corona::WireShader
{
public:
	WireMap(MObject sObject);
	~WireMap();
	Corona::String mapName = "WireMap";

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
	virtual bool exportMap(Corona::IResourceManager& resourceManager, Corona::XmlWriterNode& outXml) const { return true; };
	virtual Corona::String name() const { return mapName; };

};


#endif