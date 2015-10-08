#include "CoronaMap.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/pystring.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include <vector>

bool textureFileSupported(MString fileName)
{
	std::vector<std::string> stringArray;
	pystring::split(fileName.asChar(), stringArray, ".");
	std::string ext = stringArray.back();
	ext = pystring::lower(ext);

	if( ext == "exr")
		return true;
	if( ext == "jpg")
		return true;
	if( ext == "bmp")
		return true;
	if( ext == "png")
		return true;

	return false;
}


mtco_MapLoader::mtco_MapLoader(MObject& mobject)
{
	if (mobject == MObject::kNullObj)
		return;
	MFnDependencyNode depFn(mobject);
	colorGain = getColorAttr("colorGain", depFn);
	colorOffset = getColorAttr("colorOffset", depFn);
	exposure = getFloatAttr("exposure", depFn, 0.0f);
	MObject place2dNode = getConnectedInNode(mobject, "uvCoord");
	if ((place2dNode != MObject::kNullObj) && (place2dNode.hasFn(MFn::kPlace2dTexture)))
	{
		MFnDependencyNode placeFn(place2dNode);
		float coverageU = getFloatAttr("coverageU", placeFn, 1.0f);
		float coverageV = getFloatAttr("coverageV", placeFn, 1.0f);
		float translateFrameU = getFloatAttr("translateFrameU", placeFn, 0.0f);
		float translateFrameV = getFloatAttr("translateFrameV", placeFn, 0.0f);
		float rotateFrame = getFloatAttr("rotateFrame", placeFn, 0.0f);
		float repeatU = getFloatAttr("repeatU", placeFn, 1.0f);
		float repeatV = getFloatAttr("repeatV", placeFn, 1.0f);
		float offsetU = getFloatAttr("offsetU", placeFn, 0.0f);
		float offsetV = getFloatAttr("offsetV", placeFn, 0.0f);
		float rotateUV = getFloatAttr("rotateUV", placeFn, 0.0f);
		float noiseU = getFloatAttr("noiseU", placeFn, 1.0f);
		float noiseV = getFloatAttr("noiseV", placeFn, 1.0f);
		float mirrorU = getBoolAttr("mirrorU", placeFn, false);
		float mirrorV = getBoolAttr("mirrorV", placeFn, false);
		float wrapU = getBoolAttr("wrapU", placeFn, true);
		float wrapV = getBoolAttr("wrapV", placeFn, true);
		float stagger = getBoolAttr("stagger", placeFn, false);
		Corona::TextureShader::Config tsConfig;
		
	}
}

mtco_MapLoader::~mtco_MapLoader()
{}
