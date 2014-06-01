#include "TheaXMLUtil.h"
#include <string>

static std::map<std::string, std::string> typeMap;
static std::map<std::string, std::string> paramNameMap;
static std::map<std::string, std::string> texObjMap;

MString Parameter::get()
{
	MString pnm = paramNameMap[name.asChar()].c_str();
	if( pnm == "" )
		pnm = MString("[UNDEF: ") + name + "]";
	MString p = MString("<Parameter Name=\"") + pnm + "\" Type=\"" + typeMap[type.asChar()].c_str() + "\" Value=\"" + value + "\"/>";
	return p;
};


Object colorTextureObject(MString attName, MColor c)
{
	//Object Identifier="./Diffuse/Constant Texture" Label="Constant Texture" Name="" Type="Texture"
	//<Parameter Name="Color" Type="RGB" Value="0.482866 0.456655 0.792157"/>
	MString identifier = MString("./") + texObjMap[attName.asChar()].c_str() + "/Constant Texture";
	MString label = "Constant Texture";
	MString name = "";
	MString type = "Texture";
	Object o("subobject", name, type, label, identifier);
	Parameter p("Color", c);
	o.parameterList.push_back(p);
	return o;
}

void initMaps()
{
	typeMap["float"] = "Real";
	typeMap["color"] = "RGB";
	typeMap["bool"] = "Boolean";
	typeMap["string"] = "String";
	typeMap["int"] = "Integer";
	typeMap["file"] = "File";


	paramNameMap["emitterMinRays"] = "Emitter Min Rays";
	paramNameMap["emitterMaxRays"] = "Emitter Max Rays";
	paramNameMap["dirtLevel"] = "Dirt Level";
	paramNameMap["tracingDepth"] = "Tracing Depth";
	paramNameMap["blurredMinSubdivs"] = "Blurred Min Subdivs";
	paramNameMap["perceptualLevel"] = "Perceptual Level";
	paramNameMap["blurredMaxSubdivs"] = "Blurred Max Subdivs";
	paramNameMap["ambientLevel"] = "Ambient Level";
	paramNameMap["emitterAccuracy"] = "Emitter Accuracy";

	paramNameMap["fileTextureName"] = "Filename";
	paramNameMap["reflectance90Deg"] = "Color";
	paramNameMap["reflectanceColor"] = "Color";
	paramNameMap["translucentColor"] = "Color";
	paramNameMap["Color"] = "Color";
	paramNameMap["roughness"] = "Roughness";
	paramNameMap["anisotropy"] = "Anisotropy";
	paramNameMap["rotation"] = "Rotation";
	paramNameMap["ior"] = "Index of Refraction";
	paramNameMap["traceReflections"] = "Trace Reflections";
	paramNameMap["traceRefractions"] = "Trace Refractions";
	paramNameMap["kappa"] = "Extinction Coefficient";
	paramNameMap["sigma"] = "Sigma";
	paramNameMap["bump"] = "Bump Strength";
	paramNameMap["normalMapping"] = "Normal Mapping";
	paramNameMap["absorptionDensity"] = "Absorption Density";
	paramNameMap["absorptionColor"] = "Absorption Color";
	paramNameMap["microRoughness"] = "./Micro Roughness/Enable";
	paramNameMap["microRoughnessWidth"] = "./Micro Roughness/Width (um)";
	paramNameMap["microRoughnessHeight"] = "./Micro Roughness/Height (um)";
	paramNameMap["repeatU"] = "Scale X";
	paramNameMap["repeatV"] = "Scale Y";
	paramNameMap["offsetU"] = "Offset X";
	paramNameMap["offsetV"] = "Offset Y";
	paramNameMap["rotateUV"] = "Rotation";
	paramNameMap["twosided"] = "Two-Sided";
	
	paramNameMap["transmittedRoughness"] = "Roughness Tr.";

	paramNameMap["globalEmitter"] = "Global Emitter";
	paramNameMap["causticEmitter"] = "Caustic Emitter";
	paramNameMap["passiveEmitter"] = "Passive Emitter";
	paramNameMap["repaintable"] = "Repaintable";
	paramNameMap["ambientEmitter"] = "Ambient Emitter";
	paramNameMap["activeDirt"] = "Active Dirt";
	paramNameMap["shadowCatcher"] = "Shadow Catcher";

	paramNameMap["attenuation"] = "Attenuation";
	paramNameMap["power"] = "Power";
	paramNameMap["frontSide"] = "Front Side";
	paramNameMap["backSide"] = "Back Side";
	paramNameMap["emitter"] = "Emitter";
	paramNameMap["unit"] = "Unit";
	paramNameMap["efficacy"] = "Efficacy";

	texObjMap["color"] = "Color";
	texObjMap["diffuseColor"] = "Diffuse";
	texObjMap["translucentColor"] = "Translucent";
	texObjMap["reflectanceColor"] = "Reflectance 0";
	texObjMap["reflectance90Deg"] = "Reflectance 90";
}

MString getType(std::string att)
{
	return MString(typeMap[att].c_str());
}
MString getParamName(std::string att)
{
	return MString(paramNameMap[att].c_str());
}
MString getTexMap(std::string att)
{
	return MString(texObjMap[att].c_str());
}
