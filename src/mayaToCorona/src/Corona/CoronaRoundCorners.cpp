#include "Corona.h"
#include "CoronaRoundCorners.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"

RoundCorners::RoundCorners(MObject shaderObject)
{
	MFnDependencyNode depFn(shaderObject);
};

RoundCorners::~RoundCorners(){};

Corona::Rgb RoundCorners::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	float f = 1.0f;
	return Corona::Rgb(f, f, f);
}

float RoundCorners::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir RoundCorners::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP; //currently not supported
}

void RoundCorners::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}
