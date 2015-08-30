#include "Corona.h"
#include "CoronaAOShader.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"

Corona::AoShader::ExcludeMode excludeModes[] = { Corona::AoShader::EXCLUDE_LIST, Corona::AoShader::EXCLUDE_SAME, Corona::AoShader::EXCLUDE_OTHER };

AoMap::AoMap(MObject shaderObject)
{
	
	MFnDependencyNode depFn(shaderObject);
	maxDist = getFloatAttr("maxDist", depFn, 1.0f);
	params.maxSamplesCount = getIntAttr("maxSamplesCount", depFn, 8);
	params.phongExponent = getFloatAttr("phongExponent", depFn, 1.0f);
	params.mixExponent = getFloatAttr("mixExponent", depFn, 1.0f);
	params.invert = getBoolAttr("invert", depFn, false);
	params.offset = Corona::Dir::ZERO;
	params.excludeMode = excludeModes[getEnumInt("excludeMode", depFn)];
	if (getBoolAttr("useOffset", depFn, false))
	{
		MVector off = getVectorAttr("offset", depFn);
		params.offset = Corona::Dir(off.x, off.y, off.z);
	}
};

AoMap::~AoMap(){};

Corona::Rgb AoMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	Corona::Dir d = context.getRay().direction;
	float f = eval(context, maxDist);
	return Corona::Rgb(f, f, f);
}

float AoMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir AoMap::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP; //currently not supported
}

void AoMap::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}

