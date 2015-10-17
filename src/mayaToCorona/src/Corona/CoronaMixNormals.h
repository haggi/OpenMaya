#ifndef CORONA_AO_MAP
#define CORONA_AO_MAP

#include <maya/MObject.h>
#include "CoronaCore/api/Api.h"

class CoronaMixNormals : public Corona::Abstract::Map
{
public:
	CoronaMixNormals(MObject sObject);
	~CoronaMixNormals();
	Corona::ColorOrMap inputA;
	Corona::ColorOrMap inputB;
	Corona::String mapName = "CoronaMixNormals";

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
	virtual bool exportMap(Corona::IResourceManager& resourceManager, Corona::XmlWriterNode& outXml) const { return true; };
	virtual Corona::String name() const { return mapName; };

};


#endif