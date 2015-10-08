#ifndef CORONA_ROUNDCORNERS_MAP
#define CORONA_ROUNDCORNERS_MAP

#include <maya/MObject.h>
#include "CoronaCore/api/Api.h"
#include "CoronaCore/misc/Shaders/RoundEdgesShader.h"

class CoronaRenderer;

class RoundCorners : public Corona::Abstract::Map, Corona::RoundEdgesShader
{
public:
	CoronaRenderer *coronaRenderer;
	RoundCorners();
	RoundCorners(MObject sObject);
	~RoundCorners();
	Corona::String mapName = "RoundCorners";
	int samplesCount = 10;
	float radius = 0.01f;

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
	//virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output){};
	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
	virtual bool exportMap(Corona::IResourceManager& resourceManager, Corona::XmlWriterNode& outXml) const { return true; };
	virtual Corona::String name() const { return mapName; };

};


#endif