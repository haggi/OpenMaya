#include "Corona.h"
#include "CoronaLights.h"
#include "CoronaSky.h"
#include "CoronaShaders.h"
#include "CoronaUtils.h"
#include "CoronaOSLMap.h"
#include <maya/MObjectArray.h>
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "mayaScene.h"
#include "world.h"
#include "../mtco_common/mtco_mayaObject.h"

static Logging logger;

bool CoronaRenderer::isSunLight(std::shared_ptr<MayaObject> obj)
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
	MFnDependencyNode rGlNode(getRenderGlobalsNode());
	// first get the globals node and serach for a directional light connection
	MObject coronaGlobals = getRenderGlobalsNode();
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	MObjectArray nodeList;
	MStatus stat;

	MFnDependencyNode depFn(getRenderGlobalsNode());
	Corona::Rgb bgRgb = toCorona(getColorAttr("bgColor", depFn));
	int bgType = getEnumInt("bgType", depFn);

	if( renderGlobals->useSunLightConnection)
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
				lightDir *= renderGlobals->globalConversionMatrix;
				lightDir.normalize();
		
				MObject sunDagObj =	sunDagNode.child(0, &stat);
				MColor sunColor(1);
				float colorMultiplier = 1.0f;
				if(sunDagObj.hasFn(MFn::kDirectionalLight))
				{
					MFnDependencyNode sunNode(sunDagObj);
					getColor("color", sunNode, sunColor);
					colorMultiplier = getFloatAttr("mtco_sun_multiplier", sunNode, 1.0f);
				}else{
					Logging::warning(MString("Sun connection is not a directional light - using transform only."));
				}
				const float intensityFactor = (1.f - cos(Corona::SUN_PROJECTED_HALF_ANGLE)) / (1.f - cos(getFloatAttr("sunSizeMulti", rGlNode, 1.0f) * Corona::SUN_PROJECTED_HALF_ANGLE));
				sunColor *= colorMultiplier * intensityFactor * 1.0;// 2000000;
				Corona::Sun sun;

				Corona::ColorOrMap bgCoMap = this->context.scene->getBackground();
				SkyMap *sky = dynamic_cast<SkyMap *>(bgCoMap.getMap());
				Corona::Rgb avgColor(1, 1, 1);
				if (sky != nullptr)
				{
					avgColor = sky->sc();
				}

				Corona::Rgb sColor(sunColor.r, sunColor.g, sunColor.b);
				sun.color = sColor * avgColor;
				sun.active = true;
				sun.dirTo = Corona::Dir(lightDir.x, lightDir.y, lightDir.z).getNormalized();
				//sun.color = Corona::Rgb(sunColor.r,sunColor.g,sunColor.b);
				sun.visibleDirect = true;
				sun.visibleReflect = true;
				sun.visibleRefract = true;
				sun.sizeMultiplier = getFloatAttr("sunSizeMulti", rGlNode, 1.0);
				this->context.scene->getSun() = sun;
				sky->initSky();
				if (sky != nullptr)
				{
					avgColor = sky->sc();
					this->context.scene->getSun().color = sColor * avgColor;
				}
			}
		}
	}

	for (auto mobj : mayaScene->lightList)
	{
		std::shared_ptr<mtco_MayaObject> obj(std::static_pointer_cast<mtco_MayaObject>(mobj));

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
			MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			PointLight *pl = new PointLight;
			pl->LP = LP;
			pl->distFactor = 1.0/renderGlobals->scaleFactor;
			pl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			pl->lightIntensity = intensity;
			getEnum(MString("decayRate"), depFn, pl->decayType);
			pl->lightRadius = getFloatAttr("lightRadius", depFn, 0.0) * renderGlobals->scaleFactor;
			pl->doShadows = getBoolAttr("useRayTraceShadows", depFn, true);
			col = getColorAttr("shadowColor", depFn);
			pl->shadowColor = Corona::Rgb(col.r, col.g, col.b);
			for (auto excludedObj : obj->excludedObjects)
			{
				pl->excludeList.nodes.push(excludedObj.get());
			}
			this->context.scene->addLightShader(pl);
		}
		if( obj->mobject.hasFn(MFn::kSpotLight))
		{
			MVector lightDir(0, 0, -1);
			MColor col;
			getColor("color", depFn, col);
			float intensity = 1.0f;
			getFloat("intensity", depFn, intensity);
			MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
			lightDir *= obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
			lightDir.normalize();
			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			SpotLight *sl = new SpotLight;
			sl->LP = LP;
			sl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			sl->lightIntensity = intensity;
			sl->LD = Corona::Dir(lightDir.x, lightDir.y, lightDir.z);
			sl->angle = 45.0f;
			sl->distFactor = 1.0/renderGlobals->scaleFactor;
			getEnum(MString("decayRate"), depFn, sl->decayType);
			getFloat("coneAngle", depFn, sl->angle);
			getFloat("penumbraAngle", depFn, sl->penumbraAngle);
			getFloat("dropoff", depFn, sl->dropoff);
			sl->lightRadius = getFloatAttr("lightRadius", depFn, 0.0) * renderGlobals->scaleFactor;
			sl->doShadows = getBoolAttr("useRayTraceShadows", depFn, true);
			col = getColorAttr("shadowColor", depFn);
			sl->shadowColor = Corona::Rgb(col.r, col.g, col.b);
			for (auto excludedObj:obj->excludedObjects)
			{
				sl->excludeList.nodes.push(excludedObj.get());
			}
			Corona::AffineTm tm;
			setTransformationMatrix(sl->lightWorldInverseMatrix, m);
			ShadingNetwork network(obj->mobject);
			sl->lightColorMap = defineAttribute(MString("color"), depFn, network);
			
			this->context.scene->addLightShader(sl);
		}
		if( obj->mobject.hasFn(MFn::kDirectionalLight))
		{
			MVector lightDir(0, 0, -1);
			MVector lightDirTangent(1, 0, 0);
			MVector lightDirBiTangent(0, 1, 0);
			MColor col;
			getColor("color", depFn, col);
			float intensity = 1.0f;
			getFloat("intensity", depFn, intensity);
			MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
			lightDir *= m;
			lightDirTangent *= m;
			lightDirBiTangent *= m;
			lightDir.normalize();

			Corona::Pos LP(m[3][0],m[3][1],m[3][2]);
			DirectionalLight *dl = new DirectionalLight;
			dl->LP = LP;
			dl->lightColor = Corona::Rgb(col.r, col.g, col.b);
			dl->lightIntensity = intensity;
			dl->LD = Corona::Dir(lightDir.x, lightDir.y, lightDir.z);
			dl->LT = Corona::Dir(lightDirTangent.x, lightDirTangent.y, lightDirTangent.z);
			dl->LBT = Corona::Dir(lightDirBiTangent.x, lightDirBiTangent.y, lightDirBiTangent.z);
			dl->lightAngle = getFloatAttr("lightAngle", depFn, 0.0);
			dl->doShadows = getBoolAttr("useRayTraceShadows", depFn, true);
			col = getColorAttr("shadowColor", depFn);
			dl->shadowColor = Corona::Rgb(col.r, col.g, col.b);
			for (auto excludedObj : obj->excludedObjects)
			{
				dl->excludeList.nodes.push(excludedObj.get());
			}

			this->context.scene->addLightShader(dl);
		}
		if( obj->mobject.hasFn(MFn::kAreaLight))
		{
			MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
			obj->geom = defineStdPlane();
			Corona::AnimatedAffineTm atm;
			this->setAnimatedTransformationMatrix(atm, obj);
			obj->instance = obj->geom->addInstance(atm, nullptr, nullptr);
			if (getBoolAttr("mtco_envPortal", depFn, false))
			{
				Corona::EnviroPortalMtlData data;
				Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
				Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
				obj->instance->addMaterial(ms);
			}
			else{
				Corona::NativeMtlData data;
				MColor lightColor = getColorAttr("color", depFn);
				float intensity = getFloatAttr("intensity", depFn, 1.0f);
				lightColor *= intensity;
				Corona::ColorOrMap com = defineAttribute(MString("color"), obj->mobject);			
				OSLMap *oslmap = (OSLMap *)com.getMap();
				if (oslmap != nullptr)
				{
					oslmap->multiplier = intensity;
				}
				else{
					Corona::Rgb col = com.getConstantColor() * intensity;
					com.setColor(col);
				}
				data.emission.color = com;
				data.castsShadows = false; // a light should never cast shadows

				for (auto excludedObj : obj->excludedObjects)
				{
					data.emission.excluded.nodes.push(excludedObj.get());
				}
				data.emission.disableSampling = false;
				data.emission.useTwoSidedEmission = getBoolAttr("mtco_doubleSided", depFn, false);

				Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
				Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
				ms.visibility.direct = getBoolAttr("mtco_areaVisible", depFn, true);
				ms.visibility.reflect = getBoolAttr("mtco_visibleInReflection", depFn, true);
				ms.visibility.refract = getBoolAttr("mtco_visibleInRefraction", depFn, true);
				
				obj->instance->addMaterial(ms);
			}
		}
	}
}
