#include "Corona.h"
#include "CoronaWire.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"

WireMap::WireMap(MObject shaderObject)
{
	MFnDependencyNode depFn(shaderObject);
	useWorldSpace = getBoolAttr("useWorldSpace", depFn, false);
	showVertices = getBoolAttr("showVertices", depFn, false);
	allEdges = getBoolAttr("allEdges", depFn, false);
	showEdges = getBoolAttr("showEdges", depFn, true);
	edgeWidth = getFloatAttr("edgeWidth", depFn, 1.0f);
	vertexWidth = getFloatAttr("vertexWidth", depFn, 1.0f);
};

WireMap::~WireMap(){};

Corona::Rgb WireMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	float f = 1.0f;
	Corona::WireShader::Result res = eval(context);
	if (res == Corona::WireShader::Result::HIT_EDGE)
		return Corona::Rgb(1.0);
	if (res == Corona::WireShader::Result::HIT_VERTEX)
		return Corona::Rgb(1.0);
	if (res == Corona::WireShader::Result::HIT_NONE)
		return Corona::Rgb(0.0);
	return Corona::Rgb(0.0);
}

float WireMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir WireMap::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP; //currently not supported
}

void WireMap::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}

