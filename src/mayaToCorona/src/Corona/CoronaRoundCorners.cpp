#include "Corona.h"
#include "CoronaRoundCorners.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"
#include "world.h"
#include "renderGlobals.h"

RoundCorners::RoundCorners(MObject shaderObject)
{
	MFnDependencyNode depFn(shaderObject);
	radius = getFloatAttr("radius", depFn, 0.0f);
	samplesCount = getIntAttr("samples", depFn, 10);
	float globalScaleFactor = 1.0f;
	if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr != nullptr)
		globalScaleFactor = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->scaleFactor;
	else
		globalScaleFactor = MayaTo::getWorldPtr()->scaleFactor;
	radius *= globalScaleFactor;
};

RoundCorners::RoundCorners()
{};

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

Corona::Dir RoundCorners::evalBump(const Corona::IShadeContext& context, Corona::TextureCache* cache)
{
	Corona::Dir inNormal(0,0,0);
	
	if (normalCamera.getMap() != nullptr)
		inNormal = normalCamera.getMap()->evalBump(context, cache);
	return evalPerturbation(context, radius) + inNormal;
}

//void RoundCorners::renderTo(Corona::Bitmap<Corona::Rgb>& output)
//{
//	STOP; //currently not supported
//}
