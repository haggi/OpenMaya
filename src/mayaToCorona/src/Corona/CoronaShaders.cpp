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

void CoronaRenderer::defineColorOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com)
{
	MapLoader loader;
	Corona::Abstract::Map *texmap = NULL;
	MColor col(0,0,0);
	MString fileTexturePath = "";
	getColor(attributeName, depFn, col);
	Corona::Rgb rgbColor = Corona::Rgb(col.r,col.g,col.b);
	if(getConnectedFileTexturePath(attributeName, depFn.name(), fileTexturePath))
	{
		texmap = loader.loadBitmap(fileTexturePath.asChar());
	}else{
		texmap = NULL;
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineFloatOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com)
{
	MapLoader loader;
	Corona::Abstract::Map *texmap = NULL;
	float f;
	MString fileTexturePath = "";
	getFloat(attributeName, depFn, f);
	Corona::Rgb rgbColor = Corona::Rgb(f,f,f);
	if(getConnectedFileTexturePath(attributeName, depFn.name(), fileTexturePath))
	{
		texmap = loader.loadBitmap(fileTexturePath.asChar());
	}else{
		texmap = NULL;
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{
	Corona::NativeMtlData data;
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
		
	if( obj->shadingGroups.length() > 0)
	{
		Material mat(obj->shadingGroups[0]);
		if( mat.surfaceShaderNet.shaderList.size() > 0)
		{
			// give me the last node in the node list, this should be the surface shader
			ShadingNode ss = mat.surfaceShaderNet.shaderList.back();
			//ShadingNode ss = mat.surfaceShaderNet.shaderList[ mat.surfaceShaderNet.shaderList.size() - 1];
			logger.debug(MString("Found surface shader ") + ss.fullName);
			MColor colorVal;
			
			Corona::Rgb rgbColor;
			Corona::ColorOrMap com;

			MFnDependencyNode depFn(ss.mobject);
			if( ss.typeName == "CoronaSurface")
			{
				MColor overrideColor(1,1,1);
				if( obj->attributes != NULL)
					if( obj->attributes->hasColorOverride)
						overrideColor = obj->attributes->colorOverride;
				
				
				this->defineColorOrMap(MString("diffuse"), depFn, data.components.diffuse);				

				this->defineColorOrMap(MString("translucency"), depFn, data.components.translucency);				
				
				this->defineColorOrMap(MString("reflectivity"), depFn, data.components.reflect);	

				const Corona::BrdfLobeType brdfs[] = {Corona::BrdfLobeType::BRDF_ASHIKHMIN, Corona::BrdfLobeType::BRDF_FAKE_WARD, Corona::BrdfLobeType::BRDF_PHONG, Corona::BrdfLobeType::BRDF_WARD};
				int id;
				getEnum(MString("brdfType"), depFn, id);
				data.reflect.brdfType = brdfs[id];
				this->defineFloatOrMap(MString("reflectionGlossiness"), depFn, data.reflect.glossiness);
				this->defineFloatOrMap(MString("fresnelIor"), depFn, data.reflect.fresnelIor);	
				this->defineFloatOrMap(MString("anisotropy"), depFn, data.reflect.anisotropy);	
				this->defineFloatOrMap(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation);	
				

				this->defineColorOrMap(MString("refractivity"), depFn, data.components.refract);				
				this->defineFloatOrMap(MString("refractionIndex"), depFn, data.refract.ior);				
				this->defineFloatOrMap(MString("refractionGlossiness"), depFn, data.refract.glossiness);	

				MColor attenuationColor(0,0,0);
				getColor("attenuationColor", depFn, attenuationColor);
				data.refract.attenuationColor = Corona::Rgb(attenuationColor.r,attenuationColor.g,attenuationColor.b);				
				
				float attenuationDist = 0.0f;
				getFloat("attenuationDist", depFn, attenuationDist);
				data.refract.attenuationDist = attenuationDist;		

				int glassMode = 0;
				getEnum(MString("glassMode"), depFn, glassMode);
				Corona::GlassMode glassModes[] = {Corona::GlassMode::GLASS_ONESIDED, Corona::GlassMode::GLASS_TWOSIDED, Corona::GlassMode::GLASS_HYBRID};
				data.refract.glassMode = glassModes[glassMode];

				//data.energyConservation

				// ---- emission ----
				this->defineColorOrMap(MString("emissionColor"), depFn, data.emission.color);				

				bool disableSampling = false;
				getBool("emissionDisableSampling", depFn, disableSampling);
				data.emission.disableSampling = disableSampling;

				bool sharpnessFake = false;
				getBool("emissionSharpnessFake", depFn, sharpnessFake);
				data.emission.sharpnessFake = sharpnessFake;

				MVector point(0,0,0);
				getPoint(MString("emissionSharpnessFakePoint"), depFn, point);
				data.emission.sharpnessFakePoint = Corona::AnimatedPos(Corona::Pos(point.x, point.y, point.z));

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

