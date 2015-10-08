#ifndef CORONA_FRONTBACK_MAP
#define CORONA_FRONTBACK_MAP

#include <maya/MObject.h>
#include "CoronaCore/api/Api.h"

class FrontBackMap : public Corona::Abstract::Map
{
public:
	FrontBackMap(MObject sObject);
	~FrontBackMap();
	Corona::String mapName = "FrontBackMap";
	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
	virtual bool exportMap(Corona::IResourceManager& resourceManager, Corona::XmlWriterNode& outXml) const { return true; };
	virtual Corona::String name() const { return mapName; };
};


#endif