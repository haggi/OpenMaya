#include "Corona.h"
#include "CoronaLights.h"

#include <maya/MObjectArray.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"

static Logging logger;

bool CoronaRenderer::isSunLight(mtco_MayaObject *obj)
{
	// a sun light has a transform connection to the coronaGlobals.sunLightConnection plug
	MObject coronaGlobals = objectFromName(MString("coronaGlobals"));
	MObjectArray nodeList;
	MStatus stat;
	getConnectedInNodes(MString("sunLightConnection"), coronaGlobals, nodeList);
	if( nodeList.length() > 0)
	{
		MObject sunObj = nodeList[0];
		if(sunObj.hasFn(MFn::kTransform))
		{
			MFnDagNode sunDagNode(sunObj);
			MObject sunDagObj =	sunDagNode.child(0, &stat);
			if( sunDagObj == obj->mobject)
				return true;
		}
	}
	return false;
}

void CoronaRenderer::defineLights()
{
	// first get the globals node and serach for a directional light connection
	MObject coronaGlobals = objectFromName(MString("coronaGlobals"));
	MObjectArray nodeList;
	MStatus stat;

	if( this->mtco_renderGlobals->useSunLightConnection )
	{
		getConnectedInNodes(MString("sunLightConnection"), coronaGlobals, nodeList);
		if( nodeList.length() > 0)
		{
			MObject sunObj = nodeList[0];
			if(sunObj.hasFn(MFn::kTransform))
			{
				// we suppose what's connected here is a dir light transform
				MVector lightDir(0, 0, 1); // default dir light dir
				MFnDagNode sunDagNode(sunObj);
				lightDir *= sunDagNode.transformationMatrix();
				lightDir *= this->mtco_renderGlobals->globalConversionMatrix;
				lightDir.normalize();
		
				MObject sunDagObj =	sunDagNode.child(0, &stat);
				if( !stat )
					logger.error("no child 0");
				MColor sunColor(1);
				float colorMultiplier = 1.0f;
				if(sunDagObj.hasFn(MFn::kDirectionalLight))
				{
					MFnDependencyNode sunNode(sunDagObj);
					getColor("color", sunNode, sunColor);
					getFloat("mtco_sun_multiplier", sunNode, colorMultiplier);
				}else{
					logger.warning(MString("Sun connection is not a directional light - using transform only."));
				}
				sunColor *= colorMultiplier * 10000.0;
				Corona::Sun sun;
				sun.active = true;
				sun.dirTo = Corona::Dir(lightDir.x, lightDir.y, lightDir.z).getNormalized();
				sun.color = Corona::Rgb(sunColor.r,sunColor.g,sunColor.b);
				sun.visibleDirect = true;
				sun.visibleReflect = true;
				sun.visibleRefract = true;
				sun.sizeMultiplier = 2.0f;
				this->context.scene->getSun() = sun;
			}
		}
	}

	for( size_t lightId = 0; lightId < this->mtco_scene->lightList.size();  lightId++)
	{
		mtco_MayaObject *obj =  (mtco_MayaObject *)this->mtco_scene->lightList[lightId];
		if(!obj->visible)
			continue;

		if( this->isSunLight(obj))
			continue;
		
		MFnDependencyNode depFn(obj->mobject);

		if( obj->mobject.hasFn(MFn::kPointLight))
		{
			MColor col;
			getColor("color", depFn, col);
			float intensity = 1.0f;
			getFloat("intensity", depFn, intensity);
			int decay = 0;
			getEnum(MString("decayRate"), depFn, decay);
			MMatrix m = obj->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			PointLight *pl = new PointLight;
			pl->LP = LP;
			pl->distFactor = 1.0/this->mtco_renderGlobals->scaleFactor;
			pl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			pl->lightIntensity = intensity;
			getEnum(MString("decayRate"), depFn, pl->decayType);
			this->context.scene->addLightShader(pl);
		}
		if( obj->mobject.hasFn(MFn::kSpotLight))
		{
			MVector lightDir(0, 0, -1);
			MColor col;
			getColor("color", depFn, col);
			float intensity = 1.0f;
			getFloat("intensity", depFn, intensity);
			MMatrix m = obj->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
			lightDir *= obj->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
			lightDir.normalize();

			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			SpotLight *sl = new SpotLight;
			sl->LP = LP;
			sl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			sl->lightIntensity = intensity;
			sl->LD = Corona::Dir(lightDir.x, lightDir.y, lightDir.z);
			sl->angle = 45.0f;
			sl->distFactor = 1.0/this->mtco_renderGlobals->scaleFactor;
			getEnum(MString("decayRate"), depFn, sl->decayType);
			getFloat("coneAngle", depFn, sl->angle);
			getFloat("penumbraAngle", depFn, sl->penumbraAngle);
			getFloat("dropoff", depFn, sl->dropoff);
			this->context.scene->addLightShader(sl);
		}
		if( obj->mobject.hasFn(MFn::kDirectionalLight))
		{
			MVector lightDir(0, 0, -1);
			MColor col;
			getColor("color", depFn, col);
			float intensity = 1.0f;
			getFloat("intensity", depFn, intensity);
			MMatrix m = obj->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
			lightDir *= m;
			lightDir.normalize();

			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			DirectionalLight *dl = new DirectionalLight;
			dl->LP = LP;
			dl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			dl->lightIntensity = intensity;
			dl->LD = Corona::Dir(lightDir.x, lightDir.y, lightDir.z);
			this->context.scene->addLightShader(dl);
		}
		if( obj->mobject.hasFn(MFn::kAreaLight))
		{
			logger.warning(MString("Area light: ") + obj->shortName + " not yet supported.");
			MMatrix m = obj->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
			obj->geom = defineStdPlane();
			Corona::AnimatedAffineTm atm;
			this->setAnimatedTransformationMatrix(atm, obj);
			obj->instance = obj->geom->addInstance(atm, NULL, NULL);
			//this->defineMaterial(obj->instance, obj);

		}
	}


}
