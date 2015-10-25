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
	if (ext == "gif")
		return true;
	if (ext == "jpg")
		return true;
	if( ext == "bmp")
		return true;
	if (ext == "png")
		return true;
	if (ext == "hdr")
		return true;
	if (ext == "tga")
		return true;
	if (ext == "tif")
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
	fileName = getStringAttr("fileTextureName", depFn, "");
	int enviroMapping = getEnumInt("environmentMappingType", depFn);
	Corona::TextureShader::EnviroMapping envM[] = { Corona::TextureShader::EnviroMapping::MAPPING_SPHERICAL, Corona::TextureShader::EnviroMapping::MAPPING_SCREEN };
	tsConfig.mapping.enviroMapping = envM[enviroMapping];
	tsConfig.mapping.uvwMapChannel = 0; // at the moment

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
		bool mirrorU = getBoolAttr("mirrorU", placeFn, false);
		bool mirrorV = getBoolAttr("mirrorV", placeFn, false);
		bool wrapU = getBoolAttr("wrapU", placeFn, true);
		bool wrapV = getBoolAttr("wrapV", placeFn, true);
		bool stagger = getBoolAttr("stagger", placeFn, false);

		tsConfig.mapping.wrapModeU = Corona::TextureShader::WrapMode::WRAPMODE_CLAMP;
		tsConfig.mapping.wrapModeV = Corona::TextureShader::WrapMode::WRAPMODE_CLAMP;
		if (wrapU)
			if (mirrorU)
				tsConfig.mapping.wrapModeU = Corona::TextureShader::WrapMode::WRAPMODE_MIRROR;
			else
				tsConfig.mapping.wrapModeU = Corona::TextureShader::WrapMode::WRAPMODE_REPEAT;

		tsConfig.mapping.wrapModeV = Corona::TextureShader::WrapMode::WRAPMODE_CLAMP;
		if (wrapV)
			if (mirrorV)
				tsConfig.mapping.wrapModeV = Corona::TextureShader::WrapMode::WRAPMODE_MIRROR;
			else
				tsConfig.mapping.wrapModeV = Corona::TextureShader::WrapMode::WRAPMODE_REPEAT;


		Corona::AffineTm atm(Corona::Matrix33::IDENTITY);
		atm.translate(Corona::Dir(-0.5f, -0.5f, 0.0f));
		atm.rotateZ(rotateUV);
		atm.translate(Corona::Dir(0.5f, 0.5f, 0.0f));
		atm.scale(Corona::Dir(repeatU, repeatV, 1.0f));
		atm.translate(Corona::Dir(offsetU, offsetV, 0.0f));
		tsConfig.mapping.uvTransform = atm;
	}
}

mtco_MapLoader::~mtco_MapLoader()
{}
