#include "CoronaShaders.h"
#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "shadingtools/shadingNode.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "osl/oslUtils.h"
#include "../coronaOSL/coronaOSLMapUtil.h"

static Logging logger;

Corona::Rgb defineColor(MString& attributeName, MFnDependencyNode& depFn)
{
	MColor col = getColorAttr(attributeName.asChar(), depFn);
	return Corona::Rgb(col.r, col.g, col.b);
}

float defineFloat(MString& attributeName, MFnDependencyNode& depFn)
{
	return getFloatAttr(attributeName.asChar(), depFn, 0.0f);
}

Corona::ColorOrMap defineAttribute(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = NULL;

	Corona::Rgb rgbColor(0.0);
	logger.debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if (isConnected(attributeName.asChar(), depFn, true, true))
	{
		logger.debug(MString("it is connected"));
 		rgbColor = Corona::Rgb(0.0, 0.0, 1.0);
		//texmap = getOslTexMap(attributeName, depFn, sn);
		texmap = getOslTexMap(attributeName, depFn, sn);
	}
	else{
		if (getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_COLOR)
		{
			MColor col = getColorAttr(attributeName.asChar(), depFn);
			rgbColor = Corona::Rgb(col.r, col.g, col.b);
		}
		if (getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_FLOAT)
		{
			float f = getFloatAttr(attributeName.asChar(), depFn, 0.0f);
			rgbColor = Corona::Rgb(f, f, f);
		}
	}
	return Corona::ColorOrMap(rgbColor, texmap);
}


Corona::SharedPtr<Corona::IMaterial> defineDefaultMaterial()
{
	Corona::NativeMtlData data;
	data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
	return data.createMaterial();
}

Corona::SharedPtr<Corona::IMaterial> defineCoronaMaterial(MObject& materialNode)
{

	MAYATO_OSL::initOSLUtil();

	ShadingNetwork network(materialNode);

	if (network.shaderList.size() == 0)
		return defineDefaultMaterial();

	logger.debug(MString("Defining corona material from node: ") + network.rootNodeName);

	MFnDependencyNode depFn(materialNode);
	if (depFn.typeName() == "CoronaSurface")
	{
		Corona::NativeMtlData data;
		data.components.diffuse = defineAttribute(MString("diffuse"), depFn, network);
		data.components.translucency = defineAttribute(MString("translucency"), depFn, network);
		data.components.reflect = defineAttribute(MString("reflectivity"), depFn, network);
		const Corona::BsdfLobeType bsdfType[] = { Corona::BSDF_ASHIKHMIN, Corona::BSDF_PHONG, Corona::BSDF_WARD };
		int id;
		getEnum(MString("brdfType"), depFn, id);
		data.reflect.bsdfType = bsdfType[id];
		data.reflect.glossiness = defineAttribute(MString("reflectionGlossiness"), depFn, network);

		data.reflect.glossiness = defineAttribute(MString("reflectionGlossiness"), depFn, network);
		data.reflect.fresnelIor = defineAttribute(MString("fresnelIor"), depFn, network);
		data.reflect.anisotropy = defineAttribute(MString("anisotropy"), depFn, network);
		data.reflect.anisoRotation = defineAttribute(MString("anisotropicRotation"), depFn, network);

		data.components.refract = defineAttribute(MString("refractivity"), depFn, network);
		data.refract.ior = defineAttribute(MString("refractionIndex"), depFn, network);
		data.refract.glossiness = defineAttribute(MString("refractionGlossiness"), depFn, network);

		int glassMode = 0;
		getEnum(MString("glassMode"), depFn, glassMode);
		Corona::GlassMode glassModes[] = { Corona::GLASS_ONESIDED, Corona::GLASS_TWOSIDED, Corona::GLASS_HYBRID };
		data.refract.glassMode = glassModes[glassMode];

		// --- volume ----
		data.volume.attenuationColor = defineAttribute(MString("attenuationColor"), depFn, network);
		data.volume.attenuationDist = defineFloat(MString("attenuationDist"), depFn);
		//data.volume.attenuationDist *= this->mtco_renderGlobals->scaleFactor;
		data.volume.emissionColor = defineColor(MString("volumeEmissionColor"), depFn);
		data.volume.emissionDist = defineFloat(MString("volumeEmissionDist"), depFn);

		//// -- volume sss --
		data.volume.meanCosine = getFloatAttr("volumeMeanCosine", depFn, 0.0f);
		data.volume.scatteringAlbedo = defineAttribute(MString("volumeScatteringAlbedo"), depFn, network);
		data.volume.sssMode = getBoolAttr("volumeSSSMode", depFn, false);

		data.opacity = defineAttribute(MString("opacity"), depFn, network);

		// ---- emission ----
		data.emission.color = defineAttribute(MString("emissionColor"), depFn, network);

		return data.createMaterial();
	}

	if (depFn.typeName() == "lambert")
	{
		Corona::NativeMtlData data;
		MColor col = getColorAttr("color", depFn);
		data.components.diffuse.setColor(Corona::Rgb(col.r, col.g, col.b));
		data.components.reflect.setColor(0.0f);
		return data.createMaterial();
	}

	return defineDefaultMaterial();
}