#include "Corona.h"
#include "CoronaMixNormals.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/attrTools.h"

CoronaMixNormals::CoronaMixNormals(MObject shaderObject)
{
	
	MFnDependencyNode depFn(shaderObject);
};

CoronaMixNormals::~CoronaMixNormals(){};

Corona::Rgb CoronaMixNormals::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	outAlpha = 1.0f;
	return Corona::Rgb(1.0f, 1.0f, 1.0f);
}

float CoronaMixNormals::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	return evalColor(context, cache, outAlpha).grayValue();
}

Corona::Dir CoronaMixNormals::evalBump(const Corona::IShadeContext&, Corona::TextureCache*)
{
	STOP;
	//inputA.
}

void CoronaMixNormals::renderTo(Corona::Bitmap<Corona::Rgb>& output)
{
	STOP; //currently not supported
}

