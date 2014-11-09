#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "CoronaMap.h"
#include "CoronaShaders.h"
#include "CoronaOSLMap.h"
#include "CoronaSky.h"

static Logging logger;


void CoronaRenderer::defineEnvironment()
{
	Corona::Rgb bgRgb(this->mtco_renderGlobals->bgColor.r, this->mtco_renderGlobals->bgColor.g, this->mtco_renderGlobals->bgColor.b);
	if (this->mtco_renderGlobals->bgType == 0)
	{
		MString texName;
		MObject fileTextureObject;
		if (getConnectedFileTexturePath(MString("bgColor"), MString("coronaGlobals"), texName, fileTextureObject))
		{
			Corona::String fileName = texName.asChar();
			logger.debug(MString("Found bg texture: ") + texName);
			if (!textureFileSupported(texName))
			{
				logger.error(MString("Sorry, textures of this type are not supported: ") + texName);
				return;
			}
			
			//MObject obj = objectFromName(MString("coronaGlobals"));
			//const Corona::ColorOrMap com = defineAttribute(MString("bgColor"), obj);			
			//this->context.scene->setBackground(com);
			MapLoader loader;
			Corona::SharedPtr<Corona::Abstract::Map> texmap = loader.loadBitmap(fileName);

			if (texmap.getReference() == NULL)
			{
				logger.error(MString("Unable to read bg file: ") + texName);
			}
			else{
				this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, texmap));
			}
		}
		else{
			this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, NULL));
		}
	}
	if (this->mtco_renderGlobals->bgType == 1)
	{
		SkyMap *texmap = new SkyMap;
		texmap->coronaRenderer = this;
		texmap->params->multiplier = this->mtco_renderGlobals->pSkyMultiplier;
		if (this->mtco_renderGlobals->pSkyModel == 0)
			texmap->params->mode = Corona::SkyModelType::MODEL_PREETHAM;
		if (this->mtco_renderGlobals->pSkyModel == 1)
			texmap->params->mode = Corona::SkyModelType::MODEL_RAWAFAKE;
		if (this->mtco_renderGlobals->pSkyModel == 2)
			texmap->params->mode = Corona::SkyModelType::MODEL_HOSEK;
		MColor gc = this->mtco_renderGlobals->pSkyGroundColor;
		Corona::Rgb groundCol(gc.r, gc.g, gc.b);
		texmap->params->groundColor = groundCol;
		texmap->params->horizonBlur = this->mtco_renderGlobals->pSkyHorizBlur;
		texmap->params->skyAffectGround = this->mtco_renderGlobals->pSkyAffectGround;
		texmap->params->preetham.turbidity = this->mtco_renderGlobals->pSkyPreethamTurb;
		MColor hc = this->mtco_renderGlobals->pSkyHorizon;
		Corona::Rgb hCol(hc.r, hc.g, hc.b);
		texmap->params->rawafake.horizon = hCol;
		MColor zc = this->mtco_renderGlobals->pSkyZenith;
		Corona::Rgb zCol(zc.r, zc.g, zc.b);
		texmap->params->rawafake.horizon = zCol;
		texmap->params->rawafake.sunBleed = this->mtco_renderGlobals->pSkySunBleed;
		texmap->params->rawafake.sunFalloff = this->mtco_renderGlobals->pSkySunFalloff;
		texmap->params->rawafake.sunGlow = this->mtco_renderGlobals->pSkySunGlow;
		texmap->params->rawafake.sunSideGlow = this->mtco_renderGlobals->pSkySunSideGlow;
		texmap->initSky();
		this->context.scene->setBackground(Corona::ColorOrMap(bgRgb, texmap));
	}
}


