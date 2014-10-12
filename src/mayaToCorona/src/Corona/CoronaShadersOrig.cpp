#include "Corona.h"
#include <maya/MPlugArray.h>
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "shadingtools/shadingNode.h"
#include "CoronaOSLMap.h"
#include <maya/MFnAttribute.h>
#include "osl/oslUtils.h"
#include "CoronaMap.h"
#include "world.h"

static Logging logger;

//static std::vector<Corona::NativeMtlData> dataArray;
static std::vector<MObject> shaderArray;
Corona::Stack<Corona::NativeMtlData> dataArray;
Corona::Stack<Corona::SharedPtr<Corona::IMaterial>> coronaMaterialStack;

//static std::vector<Corona::IMaterial *> definedMaterials;


Corona::SharedPtr<Corona::Abstract::Map> CoronaRenderer::getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn)
{

	size_t numNodes = sn.shaderList.size();
	MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
	MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
	OSL::ShaderGroupRef shaderGroup = this->oslRenderer->shadingsys->ShaderGroupBegin(shaderGroupName.asChar());
	
	MObject thisMObject = depFn.object();
	MString outPlugName;
	MString connectedObjectName = getObjectName(getOtherSideSourceNode(attributeName, thisMObject, true, outPlugName));

	logger.debug(MString("getOslTexMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
	MPlug shaderPlug = depFn.findPlug(attributeName);

	MAYATO_OSL::createOSLProjectionNodes(shaderPlug);

	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = sn.shaderList[shadingNodeId];
		logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		MAYATO_OSL::createOSLHelperNodes(sn.shaderList[shadingNodeId]);
		MAYATO_OSL::createOSLShadingNode(sn.shaderList[shadingNodeId]);
		MAYATO_OSL::connectProjectionNodes(sn.shaderList[shadingNodeId].mobject);

		if (snode.fullName == connectedObjectName.asChar())
		{
			MAYATO_OSL::createOSLHelperNodes(sn.shaderList[sn.shaderList.size() - 1]);
			logger.debug(MString("connected node found: ") + snode.fullName + " search output attr.");
			
			for (size_t outId = 0; outId < snode.outputAttributes.size(); outId++)
			{
				ShaderAttribute& sa = snode.outputAttributes[outId];
				if (MString(sa.name.c_str()) == outPlugName)
				{
					logger.debug(MString("connected out attr found: ") + sa.name.c_str() + " ");
					MString destParam;
					MString sourceParam = outPlugName;
					MString sourceNode = connectedObjectName;
					if ((sa.type == "color") || (sa.type == "vector"))
					{
						// lets see if we have a color helper node
						MString helperNodeName = MAYATO_OSL::createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - color/vector attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (MAYATO_OSL::doesOSLNodeAlreadyExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
						}
						destParam = "inVector";
					}
					if (sa.type == "float")
					{
						destParam = "inFloat";
					}
					if (sa.type == "int")
					{
						destParam = "inInt";
					}
					if (sa.type == "bool")
					{
						destParam = "inBool";
					}

					if (sourceParam == "output")
						sourceParam = "outOutput";

					logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
					bool success = this->oslRenderer->shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
					logger.debug(MString("connecting ") + sourceNode + "." + sourceParam + " -> " + OSLInterfaceName + "." + destParam);
					success = this->oslRenderer->shadingsys->ConnectShaders(sourceNode.asChar(), sourceParam.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
					break;
				}
			}
			break;
		}
	}
	if (!this->oslRenderer->shadingsys->ShaderGroupEnd())
	{
		logger.debug("Problem finishing shader group");
	}
	std::string serialized;
	this->oslRenderer->shadingsys->getattribute(shaderGroup.get(), "pickle", serialized);
	logger.debug(MString("Serialized: ") + serialized.c_str());

	Corona::SharedPtr<Corona::Abstract::Map> oslMapp = new OSLMap;
	OSLMap *oslMap = (OSLMap *)oslMapp.getReference();

	//OSLMap *oslMap = new OSLMap();
	
	//oslMap->coronaRenderer = this;
	oslMap->oslRenderer = (OSL::OSLShadingNetworkRenderer *)getObjPtr("oslRenderer");
	oslMap->shaderGroup = shaderGroup;

	if (attributeName == "normalCamera")
	{
		oslMap->bumpType = OSLMap::NONE;
		// we only support direct bumpmap connections
		MPlug ncPlug = depFn.findPlug("normalCamera");
		if (ncPlug.isConnected())
		{
			MPlugArray pa;
			ncPlug.connectedTo(pa, true, false);
			if (pa.length() > 0)
			{
				for (uint pId = 0; pId < pa.length(); pId++)
				{
					if (pa[pId].node().hasFn(MFn::kBump) || pa[pId].node().hasFn(MFn::kBump3d))
					{
						MFnDependencyNode bumpFn(pa[pId].node());
						MPlug interpPlug = bumpFn.findPlug("bumpInterp");
						if (interpPlug.asInt() == 0)
							oslMap->bumpType = OSLMap::BUMP;
						if (interpPlug.asInt() == 1)
							oslMap->bumpType = OSLMap::NORMALTANGENT;
						if (interpPlug.asInt() == 2)
							oslMap->bumpType = OSLMap::NORMALOBJECT;
					}
				}
			}
		}
	}
	return oslMapp;
}

void CoronaRenderer::defineAttribute(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com, ShadingNetwork& sn)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = NULL;

	Corona::Rgb rgbColor(0.0);
	logger.debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if( isConnected(attributeName.asChar(), depFn, true, true))
	{
		logger.debug(MString("it is connected"));
		texmap = getOslTexMap(attributeName, depFn, sn);
	}else{
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_COLOR )
		{
			MColor col = getColorAttr(attributeName.asChar(), depFn);
			rgbColor = Corona::Rgb(col.r,col.g,col.b);		
		}
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_FLOAT )
		{
			float f = getFloatAttr(attributeName.asChar(), depFn, 0.0f);
			rgbColor = Corona::Rgb(f, f, f);		
		}
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::NativeMtlData& data)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = NULL;
	if( isConnected("normalCamera", depFn, true, false))
	{
		MString normalCamName = "normalCamera";
		logger.debug(MString("normal camera is connected"));
		texmap = getOslTexMap(normalCamName, depFn, sn);
		logger.debug("Bump connected");
		data.bump = texmap;
	}
}

void CoronaRenderer::defineColor(MString& attributeName, MFnDependencyNode& depFn, Corona::Rgb& com)
{
	MColor col(0,0,0);
	getColor(attributeName, depFn, col);
	Corona::Rgb rgbColor = Corona::Rgb(col.r,col.g,col.b);
	com = rgbColor;
}

void CoronaRenderer::defineFloat(MString& attributeName, MFnDependencyNode& depFn, float& com)
{
	float f = 0.0f;
	getFloat(attributeName, depFn, f);
	com = f;
}

bool CoronaRenderer::assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj)
{
	int index = -1;
	for( size_t i = 0; i < shaderArray.size(); i++)
	{
		logger.debug(MString("assingExistingMat search for ") + getObjectName(shadingGroup) + " current shaderArrayElement: " + getObjectName(shaderArray[i]));
		if( shaderArray[i] == shadingGroup)
		{
			index = i;
			break;
		}
	}
	if( index > -1)
	{
		logger.info(MString("Reusing material data from :") + getObjectName(shaderArray[index]));
		Corona::NativeMtlData data = dataArray[index];
		MFnDependencyNode depFn(obj->mobject);
		// this can be a problem because a user may turn off shadow casting for the shader itself what is possible but should not be used normally
		// the default case is that the user turnes shadowCasting on/off on the geometry.
		data.castsShadows = true;
		if (!getBoolAttr("castsShadows", depFn, true))
			data.castsShadows = false;
		data.shadowCatcherMode = Corona::SC_DISABLED;
		int shadowCatcherMode = getIntAttr("mtco_shadowCatcherMode", depFn, 0);
		if (shadowCatcherMode > 0)
		{
			if (shadowCatcherMode == 1)
				data.shadowCatcherMode = Corona::SC_ENABLED_FINAL;
			if (shadowCatcherMode == 2)
				data.shadowCatcherMode = Corona::SC_ENABLED_COMPOSITE;
		}

		Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
		Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
		setRenderStats(ms, obj);
		obj->instance->addMaterial(ms);
		return true;
	}
	return false;
}

void CoronaRenderer::clearMaterialLists()
{
	shaderArray.clear();
	dataArray.clear();
	//definedMaterials.clear();
}

void CoronaRenderer::defineDefaultMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{
	Corona::NativeMtlData data;
	data.components.diffuse.setColor(Corona::Rgb(.7,.7,.7));
	Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
	
	//shaderArray.push_back(MObject::kNullObj);
	//dataArray.push(data);
	obj->instance->addMaterial(Corona::IMaterialSet(mat));
}

void CoronaRenderer::defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{

	MAYATO_OSL::initOSLUtil();

	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
		
	if( obj->shadingGroups.length() > 0)
	{
		for (uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
		{
			MObject shadingGroup = obj->shadingGroups[sgId];
			logger.debug(MString("---------- Check shading group: ") + getObjectName(shadingGroup) + " for existence on object named " +  obj->fullName);
			if (assingExistingMat(shadingGroup, obj))
				return;

			Material mat(shadingGroup);
			size_t numNodes = mat.surfaceShaderNet.shaderList.size();

			if (numNodes > 0)
			{
				// give me the last node in the node list, this should be the surface shader
				ShadingNode ss = mat.surfaceShaderNet.shaderList.back();
				//ShadingNode ss = mat.surfaceShaderNet.shaderList[ mat.surfaceShaderNet.shaderList.size() - 1];
				logger.debug(MString("Found surface shader ") + ss.fullName);
				MColor colorVal;

				Corona::Rgb rgbColor;
				Corona::ColorOrMap com;

				MFnDependencyNode depFn(ss.mobject);
				if (ss.typeName == "CoronaSurface")
				{
					Corona::NativeMtlData data;

					MColor overrideColor(1, 1, 1);
					if (obj->attributes != NULL)
						if (obj->attributes->hasColorOverride)
							overrideColor = obj->attributes->colorOverride;

					this->defineAttribute(MString("diffuse"), depFn, data.components.diffuse, mat.surfaceShaderNet);
					this->defineAttribute(MString("translucency"), depFn, data.components.translucency, mat.surfaceShaderNet);
					this->defineAttribute(MString("reflectivity"), depFn, data.components.reflect, mat.surfaceShaderNet);

					const Corona::BsdfLobeType bsdfType[] = { Corona::BSDF_ASHIKHMIN, Corona::BSDF_PHONG, Corona::BSDF_WARD };
					int id;
					getEnum(MString("brdfType"), depFn, id);
					data.reflect.bsdfType = bsdfType[id];

					//this->defineFloatOrMap(MString("reflectionGlossiness"), depFn, data.reflect.glossiness);
					this->defineAttribute(MString("reflectionGlossiness"), depFn, data.reflect.glossiness, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("fresnelIor"), depFn, data.reflect.fresnelIor);	
					this->defineAttribute(MString("fresnelIor"), depFn, data.reflect.fresnelIor, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("anisotropy"), depFn, data.reflect.anisotropy);	
					this->defineAttribute(MString("anisotropy"), depFn, data.reflect.anisotropy, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation);	
					this->defineAttribute(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation, mat.surfaceShaderNet);

					this->defineAttribute(MString("refractivity"), depFn, data.components.refract, mat.surfaceShaderNet);
					this->defineAttribute(MString("refractionIndex"), depFn, data.refract.ior, mat.surfaceShaderNet);
					this->defineAttribute(MString("refractionGlossiness"), depFn, data.refract.glossiness, mat.surfaceShaderNet);
					
					// -- round corners -- 
					float rcRadius = 0.0001;
					getFloat(MString("roundCornersRadius"), depFn, rcRadius);
					data.roundedCorners.radius = rcRadius * this->mtco_renderGlobals->scaleFactor;
					getInt(MString("roundCornersSamples"), depFn, data.roundedCorners.samples);

					int glassMode = 0;
					getEnum(MString("glassMode"), depFn, glassMode);
					Corona::GlassMode glassModes[] = { Corona::GLASS_ONESIDED, Corona::GLASS_TWOSIDED, Corona::GLASS_HYBRID };
					data.refract.glassMode = glassModes[glassMode];

					// --- volume ----
					this->defineAttribute(MString("attenuationColor"), depFn, data.volume.attenuationColor, mat.surfaceShaderNet);
					this->defineFloat(MString("attenuationDist"), depFn, data.volume.attenuationDist);
					data.volume.attenuationDist *= this->mtco_renderGlobals->scaleFactor;
					this->defineColor(MString("volumeEmissionColor"), depFn, data.volume.emissionColor);
					this->defineFloat(MString("volumeEmissionDist"), depFn, data.volume.emissionDist);

					// -- volume sss --
					data.volume.meanCosine = getFloatAttr("volumeMeanCosine", depFn, 0.0f);
					this->defineAttribute(MString("volumeScatteringAlbedo"), depFn, data.volume.scatteringAlbedo, mat.surfaceShaderNet);
					data.volume.sssMode = getBoolAttr("volumeSSSMode", depFn, false);

					this->defineAttribute(MString("opacity"), depFn, data.opacity, mat.surfaceShaderNet);

					// ---- alpha mode ----
					int alphaMode = 0;
					getEnum(MString("alphaMode"), depFn, alphaMode);
					Corona::AlphaInteraction alphaInteraction[] = { Corona::ALPHA_DEFAULT, Corona::ALPHA_SOLID, Corona::ALPHA_TRANSPARENT};
					data.alpha = alphaInteraction[alphaMode];

					// ---- emission ----
					this->defineAttribute(MString("emissionColor"), depFn, data.emission.color, mat.surfaceShaderNet);

					// ---- ies profiles -----
					MStatus stat;
					MPlug iesPlug = depFn.findPlug("mtco_mat_iesProfile", &stat);
					if (stat)
					{
						//data.emission.gonioDiagram
						MString iesFile = iesPlug.asString();
						if (iesFile.length() > 4)
						{
							Corona::IesParser iesParser;
							//std::ifstream input(iesFile.asChar());
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
									logger.error(MString(ex.getMessage().cStr()));
								}
							}
							else{
								logger.error(MString("Unable to read ies file .") + iesFile);
							}
						}
					}

					bool disableSampling = false;
					getBool("emissionDisableSampling", depFn, disableSampling);
					data.emission.disableSampling = disableSampling;

					bool sharpnessFake = false;
					getBool("emissionSharpnessFake", depFn, sharpnessFake);
					data.emission.sharpnessFake = sharpnessFake;

					MVector point(0, 0, 0);
					getPoint(MString("emissionSharpnessFakePoint"), depFn, point);
					data.emission.sharpnessFakePoint = Corona::AnimatedPos(Corona::Pos(point.x, point.y, point.z));
					
					defineBump(MString("bump"), depFn, mat.surfaceShaderNet, data);

					MFnDependencyNode geoFn(obj->mobject);
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
					//shaderArray.push_back(shadingGroup);
					//dataArray.push(data);
					Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
					Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
					setRenderStats(ms, obj);
					obj->instance->addMaterial(ms);
				}
				else if (ss.typeName == "lambert"){
					getColor("color", depFn, colorVal);
					Corona::NativeMtlData data;
					data.components.diffuse.setColor(Corona::Rgb(colorVal.r, colorVal.g, colorVal.b));
					//shaderArray.push_back(shadingGroup);
					//dataArray.push(data);
					Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
					Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
					setRenderStats(ms, obj);
					obj->instance->addMaterial(ms);
				}
				else if (ss.typeName == "CoronaLayeredMaterial"){
					defineDefaultMaterial(instance, obj);
					Corona::LayeredMtlData data;
					Corona::LayeredMtlData::MtlEntry entry;
				}
				else{
					defineDefaultMaterial(instance, obj);
				}
			}
			else{
				defineDefaultMaterial(instance, obj);
			}

			MFnDependencyNode sn;
		}
	}else{
		defineDefaultMaterial(instance, obj);
	}		
}

void CoronaRenderer::setRenderStats(Corona::IMaterialSet& ms, mtco_MayaObject *obj)
{
	MFnDependencyNode depFn(obj->mobject);

	if (!getBoolAttr("primaryVisibility", depFn, true))
		ms.visibility.direct = false;

	if (!getBoolAttr("visibleInReflections", depFn, true))
		ms.visibility.reflect = false;

	if (!getBoolAttr("visibleInRefractions", depFn, true))
		ms.visibility.refract = false;

	if (!getBoolAttr("mtco_visibleInGI", depFn, true))
		ms.visibility.normal = false;
}

