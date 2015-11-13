#include <fstream>
#include <map>
#include "CoronaShaders.h"
#include "CoronaRoundCorners.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "shadingtools/shadingNode.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "osl/oslUtils.h"
#include "../coronaOSL/coronaOSLMapUtil.h"
#include "mayaScene.h"
#include "threads/renderQueueWorker.h"
#include "CoronaUtils.h"
#include "CoronaMap.h"
#include "CoronaAOShader.h"
#include "CoronaFrontBack.h"
#include "CoronaWire.h"
#include "world.h"
#include "renderGlobals.h"

#include "CoronaMap.h"

static Logging logger;

void clearDataList()
{
	dataList.clear();
}

Corona::Rgb defineColor(MString& attributeName, MFnDependencyNode& depFn)
{
	MColor col = getColorAttr(attributeName.asChar(), depFn);
	return Corona::Rgb(col.r, col.g, col.b);
}

float defineFloat(MString& attributeName, MFnDependencyNode& depFn)
{
	return getFloatAttr(attributeName.asChar(), depFn, 0.0f);
}

// if we have something like a color attribute which we want to parse, we derive the 
// shading network from the attribute name and depFn
Corona::ColorOrMap defineAttribute(MString& attributeName, MObject& node, OSL::OSLShadingNetworkRenderer *oslRenderer)
{
	ShadingNetwork network(node);
	MFnDependencyNode depFn(node);

	return defineAttribute(attributeName, depFn, network, oslRenderer);
}

Corona::ColorOrMap defineAttribute(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, OSL::OSLShadingNetworkRenderer *oslRenderer)
{
	MStatus stat;

	Corona::SharedPtr<Corona::Abstract::Map> texmap = nullptr;

	Corona::Rgb rgbColor(0.0);
	//Logging::debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if (isConnected(attributeName.asChar(), depFn, true, true))
	{
 		rgbColor = Corona::Rgb(0.0, 0.0, 1.0);

		// check if the connected node is a original corona node or not
		MObject connectedObject  = getConnectedInNode(depFn.object(), attributeName.asChar());
		MFnDependencyNode connectedNode(connectedObject);
		const char *allowedNameStrings[] = { "CoronaAO", "CoronaSky", "CoronaWire", "CoronaFrontBack" };
		MStringArray coronaTextureNodeNames(allowedNameStrings, 4);
		for (uint i = 0; i < coronaTextureNodeNames.length(); i++)
		{
			if (connectedNode.typeName() == coronaTextureNodeNames[i])
			{
				Logging::debug(MString("Found a connected corona map : ") + connectedNode.name() + " -> " + attributeName);
				if (connectedNode.typeName() == "CoronaAO")
				{
					texmap = new AoMap(connectedObject);
					return Corona::ColorOrMap(rgbColor, texmap);
				}
				if (connectedNode.typeName() == "CoronaFrontBack")
				{
					texmap = new FrontBackMap(connectedObject);
					return Corona::ColorOrMap(rgbColor, texmap);
				}
				if (connectedNode.typeName() == "CoronaWire")
				{
					texmap = new WireMap(connectedObject);
					return Corona::ColorOrMap(rgbColor, texmap);
				}
			}
		}
		bool useOSL = true;
		if (connectedObject.hasFn(MFn::kFileTexture))
		{
			MFnDependencyNode fileTexNode(connectedObject);
			if (getBoolAttr("mtco_noOSL", fileTexNode, false))
			{
				useOSL = false;
				Logging::warning(MString("UseOSL is turned off for texture node : ") + fileTexNode.name() + " using plain corona texture.");
				MString texName = getStringAttr("fileTextureName", fileTexNode, "");
				if (textureFileSupported(texName))
				{
					mtco_MapLoader loader(connectedObject);
					texmap = loader.loadBitmap("");
				}
				else{
					Logging::error(MString("File texture format not supported: ") + texName);
				}
			}
		}
		if (useOSL)
		{
			texmap = getOslTexMap(attributeName, depFn, sn, oslRenderer);
		}
		else{
		}
	}
	else{
		if (getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_COLOR)
		{
			MColor col = getColorAttr(attributeName.asChar(), depFn);
			MString multiplierName = attributeName + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName, &stat);
			float multiplier = 1.0f;
			if (stat)
				multiplier = multiplierPlug.asFloat();
			Corona::Rgb offsetColor(0,0,0);
			rgbColor = Corona::Rgb(col.r * multiplier, col.g * multiplier, col.b * multiplier);
			rgbColor += offsetColor;
		}
		if (getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_FLOAT)
		{
			float f = getFloatAttr(attributeName.asChar(), depFn, 0.0f);
			MString multiplierName = attributeName + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName);
			if (!multiplierPlug.isNull())
				f *= multiplierPlug.asFloat();
			rgbColor = Corona::Rgb(f, f, f);
		}
	}
	return Corona::ColorOrMap(rgbColor, texmap);
}

Corona::SharedPtr<Corona::Abstract::Map> defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, OSL::OSLShadingNetworkRenderer *oslRenderer)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = nullptr;
	RoundCorners *rcmap = nullptr;

	if (isConnected("roundCorners", depFn, true, false))
	{
		MObject rcObj = getConnectedInNode(depFn.object(), "roundCorners");
		MFnDependencyNode rcNode(rcObj);
		if (rcNode.typeName() == "CoronaRoundCorners")
		{
			rcmap = new RoundCorners(rcObj);
			texmap = rcmap;
		}
	}
	if (isConnected("normalCamera", depFn, true, false))
	{
		MString normalCamName = "normalCamera";
		MObject inObj = getConnectedInNode(depFn.object(), "normalCamera");
		if (inObj != MObject::kNullObj)
		{
			MFnDependencyNode inNode(inObj);
			if (isConnected("bumpValue", inObj, true) || isConnected("normalMap", inObj, true))
			{
				MObject bumpInObj = getConnectedInNode(inObj, "bumpValue");
				if ( bumpInObj == MObject::kNullObj)
					bumpInObj = getConnectedInNode(inObj, "normalMap");
				if (bumpInObj.hasFn(MFn::kFileTexture))
				{
					MFnDependencyNode bumpInNode(bumpInObj);
					if (getBoolAttr("mtco_noOSL", bumpInNode, false))
					{
						MString texName = getStringAttr("fileTextureName", bumpInNode, "");
						if (textureFileSupported(texName))
						{
							mtco_MapLoader loader(bumpInObj);
							Corona::SharedPtr<Corona::Abstract::Map> bumtexmap = loader.loadBitmap("");
							if (rcmap != nullptr)
							{
								rcmap->normalCamera = Corona::ColorOrMap(0, bumtexmap);
								texmap = rcmap;
							}
							return texmap;
						}
						else{
							Logging::error(MString("File texture format not supported: ") + texName);
						}
					}
				}
			}
			if (rcmap != nullptr)
			{
				rcmap->normalCamera = defineAttribute(MString("normalCamera"), depFn.object(), oslRenderer);
				return texmap;
			}
			texmap = getOslTexMap(normalCamName, depFn, sn, oslRenderer);
			Logging::debug("Bump connected");
			return texmap;
		}
	}
	return texmap;
}


Corona::SharedPtr<Corona::IMaterial> defineDefaultMaterial()
{
	Corona::NativeMtlData data;
	data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
	return data.createMaterial();
}

Corona::SharedPtr<Corona::IMaterial> defineCoronaMaterial(MObject& materialNode, std::shared_ptr<MayaObject> obj, OSL::OSLShadingNetworkRenderer *oslRenderer, bool keepData)
{
	float globalScaleFactor = 1.0f;
	if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr != nullptr)
		globalScaleFactor = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->scaleFactor;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	if (materialNode == MObject::kNullObj)
		return defineDefaultMaterial();

	ShadingNetwork network(materialNode);

	if (network.shaderList.size() == 0)
		return defineDefaultMaterial();

	Logging::debug(MString("Defining corona material from node: ") + network.rootNodeName);

	MFnDependencyNode depFn(materialNode);

	// if we are in IPR mode, save all translated shading nodes to the interactive update list
	if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
	{
		if (mayaScene)
		{
			InteractiveElement iel;
			iel.mobj = materialNode;
			iel.obj = obj;
			iel.name = depFn.name();
			iel.node = materialNode;
			mayaScene->interactiveUpdateMap[mayaScene->interactiveUpdateMap.size()] = iel;

			if(MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING)
			{
				RenderQueueWorker::IPRUpdateCallbacks();
			}
		}
	}

	if(keepData)
	{
		for (size_t i = 0; i < dataList.size(); i++)
		{
			if (dataList[i].mtlName == depFn.name())
				return dataList[i].data.createMaterial();
		}
	}

	if (depFn.typeName() == "CoronaLayered")
	{
		Logging::debug(MString("Defining layered material: ") + depFn.name());
		MPlug baseMatPlug = depFn.findPlug("baseMaterial");
		if (baseMatPlug.isNull() || !baseMatPlug.isConnected())
			return defineDefaultMaterial();
		MPlugArray connected;
		baseMatPlug.connectedTo(connected, true, false);
		if (connected.length() == 0)
			return defineDefaultMaterial();
		MObject connectedMat = connected[0].node();
		MString connectedMatName = MFnDependencyNode(connectedMat).typeName();
		std::vector<std::string> validNames = {"CoronaSurface", "CoronaLight", "CoronaVolume"};
		bool found = false;
		for (auto n : validNames)
			if (connectedMatName == n.c_str())
				found = true;
		if (!found )
			return defineDefaultMaterial();

		Corona::NativeMtlData data;
		data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));

		Logging::debug(MString("Defining layered base Material: ") + connectedMatName);
		Corona::SharedPtr<Corona::IMaterial> connectedBaseMat = defineCoronaMaterial(connectedMat, obj, oslRenderer, keepData);
		Corona::LayeredMtlData layerData;
		layerData.baseMtl = connectedBaseMat;
		//Corona::LayeredMtlData::MtlEntry baseEntry;
		//baseEntry.material = connectedBaseMat;
		//baseEntry.amount = Corona::ColorOrMap(Corona::Rgb(1, 1, 1), nullptr);
		//layerData.layers.push(baseEntry);

		MPlug entryPlug = depFn.findPlug("materialEntry");
		MPlug mtlEntryPlug = depFn.findPlug("materialEntryMtl");
		MPlug amountEntryPlug = depFn.findPlug("materialEntryAmount");
		
		Logging::debug(MString("Defining layers: "));
		if (mtlEntryPlug.numElements() == amountEntryPlug.numElements())
		{
			Logging::debug(MString("Found ") + mtlEntryPlug.numElements() + " material entries.");
			for (uint pId = 0; pId < mtlEntryPlug.numElements(); pId++)
			{
				MPlug mPlug = mtlEntryPlug[pId];
				MPlug aPlug = amountEntryPlug[pId];
				if (!mPlug.isConnected())
					continue;

				MPlugArray inputs;
				mPlug.connectedTo(inputs, true, false);
				if (inputs.length() == 0)
					continue;

				// check for valid connection
				MString matType = MFnDependencyNode(inputs[0].node()).typeName();
				MString nodeName = MFnDependencyNode(inputs[0].node()).name();
				bool found = false;
				for (auto n : validNames)
					if (matType == n.c_str())
						found = true;
				if (!found)
					continue;

				Logging::debug(MString("Creating entry ") + pId + " Shader: " + nodeName);
				Corona::LayeredMtlData::MtlEntry entry;
				entry.material = defineCoronaMaterial(inputs[0].node(), obj, oslRenderer, keepData);
				MString attName = aPlug.name();
				MStringArray strArray;
				attName.split('.', strArray);
				if (strArray.length() > 1)
					attName = strArray[strArray.length()-1];
				entry.amount = defineAttribute(attName, materialNode, oslRenderer);
				layerData.layers.push(entry);
			}
		}

		return layerData.createMaterial();
	}

	if (depFn.typeName() == "CoronaSurface")
	{
		Corona::NativeMtlData data;

		data.components.diffuse = defineAttribute(MString("diffuse"), depFn, network, oslRenderer);
		data.components.translucency = defineAttribute(MString("translucency"), depFn, network, oslRenderer);
		data.translucencyLevel = defineAttribute(MString("translucencyFraction"), depFn, network, oslRenderer);
		//data.translucencyLevel = defineAttribute(MString("translucencyLevel"), depFn, network, oslRenderer);
		data.components.reflect = defineAttribute(MString("reflectivity"), depFn, network, oslRenderer);
		const Corona::BsdfLobeType bsdfType[] = { Corona::BSDF_ASHIKHMIN, Corona::BSDF_PHONG, Corona::BSDF_WARD };
		data.reflect.glossiness = defineAttribute(MString("reflectionGlossiness"), depFn, network, oslRenderer);
		data.reflect.fresnelIor = defineAttribute(MString("fresnelIor"), depFn, network, oslRenderer);
		data.reflect.anisotropy = defineAttribute(MString("anisotropy"), depFn, network, oslRenderer);
		data.reflect.anisoRotation = defineAttribute(MString("anisotropicRotation"), depFn, network, oslRenderer);
		data.components.refract = defineAttribute(MString("refractivity"), depFn, network, oslRenderer);
		data.refract.ior = defineAttribute(MString("refractionIndex"), depFn, network, oslRenderer);
		data.refract.glossiness = defineAttribute(MString("refractionGlossiness"), depFn, network, oslRenderer);

		int glassType = getEnumInt("glassType", depFn);
		Corona::GlassMode glassModes[] = { Corona::GLASS_HYBRID, Corona::GLASS_ONESIDED, Corona::GLASS_TWOSIDED };
		data.refract.glassMode = glassModes[glassType];

		// --- volume ----
		data.volume.attenuationColor = defineAttribute(MString("attenuationColor"), depFn, network, oslRenderer);
		data.volume.attenuationDist = defineFloat(MString("attenuationDist"), depFn);
		data.volume.attenuationDist *= globalScaleFactor;
		data.volume.emissionColor = defineColor(MString("volumeEmissionColor"), depFn);
		data.volume.emissionDist = defineFloat(MString("volumeEmissionDist"), depFn);

		//// -- volume sss --
		data.volume.meanCosine = getFloatAttr("volumeMeanCosine", depFn, 0.0f);
		data.volume.scatteringAlbedo = defineAttribute(MString("volumeScatteringAlbedo"), depFn, network, oslRenderer);
		data.volume.sssMode = getBoolAttr("volumeSSSMode", depFn, false);

		data.opacity = defineAttribute(MString("opacity"), depFn, network, oslRenderer);

		// ---- emission ----
		data.emission.color = defineAttribute(MString("emissionColor"), depFn, network, oslRenderer);
		data.emission.disableSampling = true; // always disable sampling because we use it only in light shaders

		// ---- alpha mode ----
		int alphaMode = 0;
		getEnum(MString("alphaMode"), depFn, alphaMode);
		Corona::AlphaInteraction alphaInteraction[] = { Corona::ALPHA_DEFAULT, Corona::ALPHA_SOLID, Corona::ALPHA_TRANSPARENT };
		data.alpha = alphaInteraction[alphaMode];

		data.bump = defineBump(MString("bump"), depFn, network, oslRenderer);

		// setup all object specific data which needs a mayaobject
		if (obj != nullptr)
		{
			// overrideColor is not yet reimplemented. It should be used by particle instancer
			MColor overrideColor(1, 1, 1);
			if (obj->attributes != nullptr)
				if (obj->attributes->hasColorOverride)
					overrideColor = obj->attributes->colorOverride;

			MFnDependencyNode geoFn(obj->mobject);

			// cast shadows is derived from geometry
			// this attribute is not exposed in the UI by default but a advanced user can use it if he wants to
			data.castsShadows = getBoolAttr("castsShadows", depFn, true);
			if (data.castsShadows)
			{
				if (!getBoolAttr("castsShadows", geoFn, true))
					data.castsShadows = false;
			}

			int shadowCatcherMode = getIntAttr("mtco_shadowCatcherMode", geoFn, 0);
			if (shadowCatcherMode > 0)
			{
				if (shadowCatcherMode == 1)
					data.shadowCatcherMode = Corona::SC_ENABLED_FINAL;
				if (shadowCatcherMode == 2)
					data.shadowCatcherMode = Corona::SC_ENABLED_COMPOSITE;
			}
			

			// ---- ies profiles -----
			MStatus stat;
			MPlug iesPlug = depFn.findPlug("iesProfile", &stat);
			if (stat)
			{
				//data.emission.gonioDiagram
				MString iesFile = iesPlug.asString();
				if (iesFile.length() > 4)
				{
					Corona::IesParser iesParser;
					Corona::FileReader input;
					Corona::String fn(iesFile.asChar());
					input.open(fn);
					if (!input.failed())
					{

						try {

							const double rm[4][4] = {
								{ 1.0, 0.0, 0.0, 0.0 },
								{ 0.0, 0.0, 1.0, 0.0 },
								{ 0.0, -1.0, 0.0, 0.0 },
								{ 0.0, 0.0, 0.0, 1.0 }
							};
							MMatrix zup(rm);

							MMatrix tm = zup * obj->transformMatrices[0];
							Corona::AnimatedAffineTm atm;
							setAnimatedTransformationMatrix(atm, tm);

							const Corona::IesLight light = iesParser.parseIesLight(input);
							data.emission.gonioDiagram = light.distribution;
							Corona::Matrix33 m(atm[0].extractRotation());

							data.emission.emissionFrame = Corona::AnimatedMatrix33(m);

						}
						catch (Corona::Exception& ex) {
							Logging::error(MString(ex.getMessage().cStr()));
						}
					}
					else{
						Logging::error(MString("Unable to read ies file .") + iesFile);
					}
				}
			}

			MFnDependencyNode globalsNode(objectFromName("coronaGlobals"));
			bool doDirect = getBoolAttr("useGlobalDirectOverride", globalsNode, false);
			bool doReflect = getBoolAttr("useGlobalReflectionOverride", globalsNode, false);
			bool doRefract = getBoolAttr("useGlobalRefractionOverride", globalsNode, false);
			if (doDirect || doReflect || doRefract)
			{
				Corona::ColorOrMap com;
				data.bgOverride = Corona::SharedPtr<Corona::RaySwitcherMap>(new Corona::RaySwitcherMap(com));
				if (doDirect)
				{
					com = defineAttribute(MString("globalDirectOverride"), globalsNode, network, oslRenderer);
					data.bgOverride->addOverride(com, Corona::RayType::RAY_DIRECT);
				}
				if (doReflect)
				{
					com = defineAttribute(MString("globalReflectionOverride"), globalsNode, network, oslRenderer);
					data.bgOverride->addOverride(com, Corona::RayType::RAY_REFLECTED);
				}
				if (doRefract)
				{
					com = defineAttribute(MString("globalRefractionOverride"), globalsNode, network, oslRenderer);
					data.bgOverride->addOverride(com, Corona::RayType::RAY_REFRACTED);
				}
			}
		}

		if (keepData)
		{
			mtlData md;
			md.data = data;
			md.mtlName = depFn.name();
			dataList.push_back(md);
		}
		return data.createMaterial();
	}

	if (depFn.typeName() == "CoronaLight")
	{
		Corona::NativeMtlData data;
		// ---- emission ----
		data.emission.color = defineAttribute(MString("emissionColor"), depFn, network, oslRenderer);
		data.emission.useTwoSidedEmission = getBoolAttr("doubleSidedEmission", depFn, false);
		//data.emission.color.setMap(new SidedMap);
		data.emission.emissionGlossiness = getFloatAttr("emissionGlossiness", depFn, 0.0f);
		//data.components.diffuse.setMap(new SidedMap);
		//bool disableSampling = false;
		//getBool("emissionDisableSampling", depFn, disableSampling);
		data.opacity = defineAttribute(MString("opacity"), depFn, network, oslRenderer);
		// if the CoronaLight shader is not supposed to emit light, it should be used as something like a self illuminated BG
		// so we turn off visible in GI and disableSampling. 
		if (getBoolAttr("emitLight", depFn, true))
		{
			data.emission.disableSampling = false;
			if (obj != nullptr)
			{
				if (!getBoolAttr("mtco_visibleInGI", depFn, true))
				{
					MFnDependencyNode geoDN(obj->mobject);
					MPlug giPlug = geoDN.findPlug("mtco_visibleInGI");
					giPlug.setBool(true);
				}
			}
		}
		else
		{
			data.emission.disableSampling = true;
			if (obj != nullptr)
			{
				//getBoolAttr("mtco_visibleInGI", depFn, true);
				MFnDependencyNode geoDN(obj->mobject);
				MPlug giPlug = geoDN.findPlug("mtco_visibleInGI");
				giPlug.setBool(false);
			}
		}
		// setup all object specific data which needs a mayaobject
		if (obj != nullptr)
		{
			bool sharpnessFake = false;
			getBool("emissionSharpnessFake", depFn, sharpnessFake);
			data.emission.sharpnessFake = sharpnessFake;
			//MVector point(0, 0, 0);
			MMatrix c = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->globalConversionMatrix;
			MMatrix centerM = obj->transformMatrices[0] * c;
			MPoint p(centerM[3][0], centerM[3][1], centerM[3][2]);
			//getPoint(MString("emissionSharpnessFakePoint"), depFn, point);
			data.emission.sharpnessFakePoint = Corona::AnimatedPos(Corona::Pos(p.x, p.y, p.z));
			
			// ---- ies profiles -----
			MStatus stat;
			MPlug iesPlug = depFn.findPlug("iesProfile", &stat);
			if (stat)
			{
				//data.emission.gonioDiagram
				MString iesFile = iesPlug.asString();
				if (iesFile.length() > 4)
				{
					std::ifstream iesFileObject;
					iesFileObject.open(iesFile.asChar());
					if (!(iesFileObject.is_open() && iesFileObject.good()))
					{
						iesFileObject.close();
					}
					else{
						iesFileObject.close();

						Corona::IesParser iesParser;
						Corona::FileReader input;
						Corona::String fn(iesFile.asChar());
						input.open(fn);
						if (!input.failed())
						{

							try {

								const double rm[4][4] = {
									{ 1.0, 0.0, 0.0, 0.0 },
									{ 0.0, 0.0, 1.0, 0.0 },
									{ 0.0, -1.0, 0.0, 0.0 },
									{ 0.0, 0.0, 0.0, 1.0 }
								};
								MMatrix zup(rm);

								MMatrix tm = zup * obj->transformMatrices[0];
								Corona::AnimatedAffineTm atm;
								setAnimatedTransformationMatrix(atm, tm);

								const Corona::IesLight light = iesParser.parseIesLight(input);
								data.emission.gonioDiagram = light.distribution;
								Corona::Matrix33 m(atm[0].extractRotation());

								data.emission.emissionFrame = Corona::AnimatedMatrix33(m);

							}
							catch (Corona::Exception& ex) {
								Logging::error(MString(ex.getMessage().cStr()));
							}
						}
						else{
							Logging::error(MString("Unable to read ies file .") + iesFile);
						}
					}
				}
			}
		}

		if (keepData)
		{
			mtlData md;
			md.data = data;
			md.mtlName = depFn.name();
			dataList.push_back(md);
		}
		return data.createMaterial();
	}

	if (depFn.typeName() == "CoronaVolume")
	{
		Corona::NativeMtlData data;
		// --- volume ----
		data.opacity = defineAttribute(MString("opacity"), depFn, network, oslRenderer);

		data.volume.attenuationColor = defineAttribute(MString("attenuationColor"), depFn, network, oslRenderer);
		data.volume.attenuationDist = defineFloat(MString("attenuationDist"), depFn);
		data.volume.attenuationDist *= globalScaleFactor;
		data.volume.emissionColor = defineColor(MString("volumeEmissionColor"), depFn);
		data.volume.emissionDist = defineFloat(MString("volumeEmissionDist"), depFn);

		//// -- volume sss --
		data.volume.meanCosine = getFloatAttr("volumeMeanCosine", depFn, 0.0f);
		data.volume.scatteringAlbedo = defineAttribute(MString("volumeScatteringAlbedo"), depFn, network, oslRenderer);
		data.volume.sssMode = getBoolAttr("volumeSSSMode", depFn, false);

		if (keepData)
		{
			mtlData md;
			md.data = data;
			md.mtlName = depFn.name();
			dataList.push_back(md);
		}
		return data.createMaterial();
	}

	if (depFn.typeName() == "lambert")
	{
		Corona::NativeMtlData data;
		MColor col = getColorAttr("color", depFn);
		data.components.diffuse.setColor(Corona::Rgb(col.r, col.g, col.b));
		data.components.reflect.setColor(0.0f);
		mtlData md;
		md.data = data;
		md.mtlName = depFn.name();
		dataList.push_back(md);
		return data.createMaterial();
	}

	return defineDefaultMaterial();
}