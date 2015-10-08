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

	// set background
	if (!isConnected("bgColor", depFn.object(), true))
	{
		this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, nullptr));
		return;
	}

	MObject inObj = getConnectedInNode(depFn.object(), "bgColor");
	if (inObj == MObject::kNullObj)
	{
		Logging::error(MString("Unable to get connected element for renderGlobals.bgColor attribute"));
		return;
	}
	// at the moment we are restricted to two possibilites: sky map or fileTexture
	MFnDependencyNode inDepFn(inObj);
	if((inDepFn.typeName() != "CoronaSky") && (!inObj.hasFn(MFn::kFileTexture)))
	{
		Logging::error(MString("Connections to bgColor attribute of type: ") + inDepFn.typeName() + " are not yet supported");
		return;
	}

	if (inObj.hasFn(MFn::kFileTexture))
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
			
			mtco_MapLoader loader(fileTextureObject);

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

	if (inDepFn.typeName() == "CoronaSky")
	{
		SkyMap *texmap = new SkyMap(inDepFn.object());
		this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, texmap));
	}

}


