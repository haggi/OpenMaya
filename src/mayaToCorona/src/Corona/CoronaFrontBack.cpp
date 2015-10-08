#include "Corona.h"
#include "CoronaFrontBack.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"

FrontBackMap::FrontBackMap(MObject shaderObject)
{
	MFnDependencyNode depFn(shaderObject);
};

FrontBackMap::~FrontBackMap(){};

Corona::Rgb FrontBackMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	float f = 1.0f;
	return Corona::Rgb(f, f, f);
}

float FrontBackMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir FrontBackMap::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP; //currently not supported
}

void FrontBackMap::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}

