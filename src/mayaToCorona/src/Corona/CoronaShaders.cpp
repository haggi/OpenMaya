#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"


#include "CoronaMap.h"

static Logging logger;

void CoronaRenderer::defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{
	Corona::NativeMtlData data;
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
	MapLoader loader;
		
	if( obj->shadingGroups.length() > 0)
	{
		Material mat(obj->shadingGroups[0]);
		if( mat.surfaceShaderNet.shaderList.size() > 0)
		{
			ShadingNode ss = mat.surfaceShaderNet.shaderList[0];
			logger.debug(MString("Found surface shader ") + ss.fullName);
			MColor colorVal;
			MString fileTexturePath = "";
			Corona::Rgb rgbColor;
			Corona::Abstract::Map *texmap = NULL;
			Corona::ColorOrMap com;

			MFnDependencyNode depFn(ss.mobject);
			if( ss.typeName == "CoronaSurface")
			{
				MColor overrideColor(1,1,1);
				if( obj->attributes != NULL)
					if( obj->attributes->hasColorOverride)
						overrideColor = obj->attributes->colorOverride;
				
				
				MColor diffuse(0,0,0);
				getColor("diffuse", depFn, diffuse);
				rgbColor = Corona::Rgb(diffuse.r,diffuse.g,diffuse.b);
				if(getConnectedFileTexturePath(MString("diffuse"), ss.fullName, fileTexturePath))
				{
					texmap = loader.loadBitmap(fileTexturePath.asChar());
				}else{
					texmap = new CheckerMap;
				}
				com = Corona::ColorOrMap(rgbColor, texmap);
				data.components.diffuse = com;
				//data.components.diffuse.setColor(Corona::Rgb(colorVal.r,colorVal.g,colorVal.b));

				MColor translucency(0,0,0);
				getColor("translucency", depFn, translucency);
				rgbColor = Corona::Rgb(translucency.r,translucency.g,translucency.b);
				if(getConnectedFileTexturePath(MString("translucency"), ss.fullName, fileTexturePath))
				{
					texmap = loader.loadBitmap(fileTexturePath.asChar());
				}else{
					texmap = NULL;
				}
				com = Corona::ColorOrMap(rgbColor, texmap);
				data.components.translucency = com;

				//data.components.translucency.setMap();
				
				//data.components.reflect.setColor();
				//data.components.reflect.setMap();

				//data.components.refract.setColor();
				//data.components.refract.setMap();

				// ---- emission ----
				MColor emissionColor(0,0,0);
				getColor("emissionColor", depFn, emissionColor);
				colorVal *= overrideColor;
				data.emission.color.setColor(Corona::Rgb(emissionColor.r,emissionColor.g,emissionColor.b));
				bool disableSampling = false;
				getBool("emissionDisableSampling", depFn, disableSampling);
				//data.emission.disableSampling = disableSampling;
				bool sharpnessFake = false;
				getBool("sharpnessFake", depFn, sharpnessFake);
				//data.emission.sharpnessFake = sharpnessFake;
				//data.emission.sharpnessFakePoint
			}else if(ss.typeName == "lambert"){
				getColor("color", depFn, colorVal);
				data.components.diffuse.setColor(Corona::Rgb(colorVal.r,colorVal.g,colorVal.b));
			}else{
				data.components.diffuse.setColor(Corona::Rgb(.2, .2, 1.0));
			}
		}else{
			data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
		}

		MFnDependencyNode sn;

	}else{
		data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
	}
		
	//Corona::IMaterial *mat = data.createMtl(this->context.settings);
	Corona::IMaterial *mat = data.createMaterial(this->context.settings);
	obj->instance->addMaterial(Corona::IMaterialSet(mat));
}

