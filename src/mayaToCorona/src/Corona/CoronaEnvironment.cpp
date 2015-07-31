#include "Corona.h"
#include "maya/MPlugArray.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "CoronaMap.h"
#include "CoronaShaders.h"
#include "CoronaOSLMap.h"
#include "CoronaSky.h"
#include "CoronaUtils.h"


static Logging logger;


void CoronaRenderer::defineEnvironment()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	Corona::Rgb bgRgb = toCorona(getColorAttr("bgColor", depFn));
	int bgType = getEnumInt("bgType", depFn);
	if (bgType == 0)
	{
		MString texName;
		MObject fileTextureObject;
		if (getConnectedFileTexturePath(MString("bgColor"), MString("coronaGlobals"), texName, fileTextureObject))
		{
			Corona::String fileName = texName.asChar();
			Logging::debug(MString("Found bg texture: ") + texName);
			if (!textureFileSupported(texName))
			{
				Logging::error(MString("Sorry, textures of this type are not supported: ") + texName);
				return;
			}
			
			//MObject obj = objectFromName(MString("coronaGlobals"));
			//const Corona::ColorOrMap com = defineAttribute(MString("bgColor"), obj);			
			//this->context.scene->setBackground(com);

			mtco_MapLoader loader;
			if (fileTextureObject != MObject::kNullObj)
			{
				MFnDependencyNode fileNode(fileTextureObject);
				loader.colorGain = getColorAttr("colorGain", fileNode);
				loader.colorOffset = getColorAttr("colorOffset", fileNode);
				loader.exposure = getFloatAttr("exposure", fileNode, 0.0);
			}
			Logging::debug(MString("bgTex: ") + fileName);
			Corona::SharedPtr<Corona::Abstract::Map> texmap = loader.loadBitmap(fileName);

			if (texmap.getReference() == nullptr)
			{
				Logging::error(MString("Unable to read bg file: ") + texName);
			}
			else{
				this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, texmap));
			}
		}
		else{
			this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, nullptr));
		}
	}
	if (bgType == 1)
	{
		SkyMap *texmap = new SkyMap;
		texmap->coronaRenderer = this;
		texmap->params->multiplier = getFloatAttr("pSkyMultiplier", depFn, 1.0);
		int skyModel = getEnumInt("pSkyModel", depFn);
		if (skyModel == 0)
			texmap->params->mode = Corona::SkyModelType::MODEL_PREETHAM;
		if (skyModel == 1)
			texmap->params->mode = Corona::SkyModelType::MODEL_RAWAFAKE;
		if (skyModel == 2)
			texmap->params->mode = Corona::SkyModelType::MODEL_HOSEK;
		texmap->params->groundColor = toCorona(getColorAttr("pSkyGroundColor", depFn));
		texmap->params->horizonBlur = getFloatAttr("pSkyHorizBlur", depFn, .1f);
		texmap->params->skyAffectGround = getBoolAttr("pSkyAffectGround", depFn, true);
		texmap->params->preetham.turbidity = getFloatAttr("pSkyPreethamTurb", depFn, 2.5f);
		texmap->params->rawafake.horizon = toCorona(getColorAttr("pSkyHorizon", depFn));
		texmap->params->rawafake.horizon = toCorona(getColorAttr("pSkyZenith", depFn));
		texmap->params->rawafake.sunBleed = getFloatAttr("pSkySunBleed", depFn, 1.0f);
		texmap->params->rawafake.sunFalloff = getFloatAttr("pSkySunFalloff", depFn, 3.0f);
		texmap->params->rawafake.sunGlow = getFloatAttr("pSkySunGlow", depFn, 1.0f);
		texmap->params->rawafake.sunSideGlow = getFloatAttr("pSkySunSideGlow", depFn, .2f);
		texmap->initSky();
		this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, texmap));
	}

	// set global volume environment
	if (depFn.findPlug("globalVolume").isConnected())
	{
		Logging::debug("globalVolume is connected.");
		MPlugArray plugArray;
		MPlug gv = depFn.findPlug("globalVolume");
		gv.connectedTo(plugArray, true, false);
		if (plugArray.length() > 0)
		{
			Logging::debug(MString("Connected plug: ") + plugArray[0].name());
			MFnDependencyNode sourceNode(plugArray[0].node());
			if (sourceNode.typeName() == "CoronaVolume")
			{
				Logging::debug(MString("Connected node is from type CoronaVolume: ") + sourceNode.name());
				this->context.scene->setGlobalMedium(defineCoronaMaterial(sourceNode.object(), nullptr));
			}
		}
	}
	//
}


