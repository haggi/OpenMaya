#include "Thea.h"
#include <map>
#include <SDK/Integration/sdk.basics.h>
#include "../mtth_common/mtth_renderGlobals.h"
#include "../mtth_common/mtth_mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"

static Logging logger;

static ShadingNodeMap theaShadingMap;

#define XBSDFBASICPTR (TheaSDK::XML::BasicBSDF *)
#define XMATPTR (TheaSDK::XML::Material *)
#define XTEXPTR (TheaSDK::XML::BitmapTexture *)

void TheaRenderer::createDefaultMaterial()
{
	if( this->mtth_renderGlobals->exportSceneFile )
	{
		TheaSDK::XML::BasicBSDF grayBsdf;
		grayBsdf.setDiffuseTexture(TheaSDK::XML::RgbTexture(1.0f,0.3f,0.7f));
		TheaSDK::XML::Material grayMat;
		grayMat.setName("DefaultMaterial");
		grayMat.setBSDF(grayBsdf);
		sceneXML.addMaterial(grayMat);
	}
}

bool TheaRenderer::assignDefinedShader(mtth_MayaObject *obj, Material& mat)
{
	logger.debug(MString("assignDefinedShader - search for materials surface shader ") + mat.materialName);
	if( mat.surfaceShaderNet.shaderList.size() > 0 )
	{
		ShadingNode surfaceShader = mat.surfaceShaderNet.shaderList.back();
		logger.debug(MString("search for surface shader ") + surfaceShader.fullName);
		TheaShadingNode tsn;
		if( theaShadingMap.find(surfaceShader.fullName.asChar(), tsn))
		{
			if( tsn.type == TheaShadingNode::MATERIAL )
			{
				logger.debug(MString("Found existing surface shader: ") + surfaceShader.fullName);
				obj->xmlModel->materialName = (XMATPTR tsn.data)->getName();
				return true;
			}
		}
	}

	return false;
}

void TheaRenderer::clearMaterialLists()
{
	theaShadingMap.clear();
}

void TheaRenderer::setTextureOrColor(const char *attrName, MObject& shaderNode, void *theaTextureNode)
{}

void TheaRenderer::setTextureOrFloat(const char *attrName, MObject& shaderNode, void *theaTextureNode)
{}

void TheaRenderer::assignParameters(ShadingNode& sn, TheaShadingNode& tsn)
{
	MFnDependencyNode depFn(sn.mobject);

	for( int i = 0; i < sn.inputAttributes.size(); i++)
	{
		logger.debug(MString("Node: ") + sn.fullName + " attr " + sn.inputAttributes[i].name.c_str());
		if( sn.inputAttributes[i].connected )
		{
			TheaShadingNode tmpNode;
			if(theaShadingMap.find(sn.inputAttributes[i].connectedNodeName, tmpNode))
			{
				MString attName = sn.inputAttributes[i].name.c_str();
				logger.debug(MString("\tFound connected attribute in shadingMap: ") + sn.inputAttributes[i].connectedNodeName.c_str() + " to " + sn.inputAttributes[i].name.c_str());
				if( tsn.type == TheaShadingNode::MATERIAL)
				{
					if( attName == "bsdf")
					{
						logger.debug(MString("\tconnecting bsdf"));
						if( tmpNode.type == TheaShadingNode::BASIC_BSDF )
							(XMATPTR(tsn.data))->setBSDF(*(XBSDFBASICPTR tmpNode.data));
					}
				}
				if( tsn.type == TheaShadingNode::BASIC_BSDF)
				{
					if( attName == "diffuseColor")
					{
						logger.debug(MString("\tconnecting diffuseColor"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setDiffuseTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "translucentColor")
					{
						logger.debug(MString("\tconnecting translucentColor"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setTranslucentTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "reflectanceColor")
					{
						logger.debug(MString("\tconnecting reflectanceColor"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setReflectanceTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "sigma")
					{
						logger.debug(MString("\tconnecting sigma"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setSigmaTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "roughness")
					{
						logger.debug(MString("\tconnecting roughness"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setRoughnessTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "anisotropy")
					{
						logger.debug(MString("\tconnecting anisotropy"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setAnisotropyTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "rotation")
					{
						logger.debug(MString("\tconnecting rotation"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setRotationTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "bump")
					{
						logger.debug(MString("\tconnecting bump"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setBumpTexture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "reflectance90Deg")
					{
						logger.debug(MString("\tconnecting reflectance90Deg"));
						if( tmpNode.type == TheaShadingNode::FILETEXTURE )
							(XBSDFBASICPTR(tsn.data))->setReflectance90Texture(*(XTEXPTR tmpNode.data));
					}
					if( attName == "normalMapping")
					{
						logger.debug(MString("\tnormalMapping connection not supported, using current value"));
						bool value;
						getBool("normalMapping", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setNormalMapping(value);
					}
					if( attName == "absorptionColor")
					{
						logger.debug(MString("\t absorptionColor connection not supported, using current value"));
						MColor value;
						getColor("absorptionColor", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setAbsorptionColor(TheaSDK::XML::RgbTexture(value.r, value.g, value.b));
					}
					if( attName == "absorptionDensity")
					{
						logger.debug(MString("\t absorptionDensity connection not supported, using current value"));
						float value;
						getFloat("absorptionDensity", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setAbsorptionDensity(value);
					}
					if( attName == "ior")
					{
						logger.debug(MString("\t ior connection not supported, using current value"));
						float value;
						getFloat("ior", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setIndexOfRefraction(value);
					}
					if( attName == "microRoughnessHeight")
					{
						logger.debug(MString("\t microRoughnessHeight not supported, using current value"));
						float value;
						getFloat("microRoughnessHeight", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setMicroRoughnessHeight(value);
					}
					if( attName == "microRoughnessWidth")
					{
						logger.debug(MString("\t microRoughnessWidth not supported, using current value"));
						float value;
						getFloat("microRoughnessWidth", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setMicroRoughnessWidth(value);
					}
					if( attName == "microRoughness")
					{
						logger.debug(MString("\t microRoughness not supported, using current value"));
						bool value;
						getBool("microRoughness", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setMicroRoughness(value);
					}
					if( attName == "kappa")
					{
						logger.debug(MString("\t kappa not supported, using current value"));
						float value;
						getFloat("kappa", depFn, value);
						(XBSDFBASICPTR(tsn.data))->setExtinctionCoefficient(value);
					}
				}
			}else{
				logger.debug(MString("\tattribute is connected but not found in list -> strange."));
			}

		}else{
			logger.debug(MString("\tdefine unconnected parameter: ") + sn.inputAttributes[i].name.c_str());
			MString attName = sn.inputAttributes[i].name.c_str();
			if( tsn.type == TheaShadingNode::BASIC_BSDF)
			{
				if( attName == "diffuseColor")
				{
					logger.debug(MString("\tdefining diffuseColor"));
					MColor c;
					getColor("diffuseColor", depFn, c);
					(XBSDFBASICPTR(tsn.data))->setDiffuseTexture(TheaSDK::XML::RgbTexture(c.r, c.g, c.b));
				}
				if( attName == "translucentColor")
				{
					logger.debug(MString("\tdefining translucentColor"));
					MColor c;
					getColor("translucentColor", depFn, c);
					(XBSDFBASICPTR(tsn.data))->setTranslucentTexture(TheaSDK::XML::RgbTexture(c.r, c.g, c.b));
				}
				if( attName == "reflectanceColor")
				{
					logger.debug(MString("\tdefining reflectanceColor"));
					MColor c;
					getColor("reflectanceColor", depFn, c);
					(XBSDFBASICPTR(tsn.data))->setReflectanceTexture(TheaSDK::XML::RgbTexture(c.r, c.g, c.b));
				}
				if( attName == "sigma")
				{
					logger.debug(MString("\tdefining sigma"));
					float value;
					getFloat("sigma", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setSigma(value);
				}
				if( attName == "roughness")
				{
					logger.debug(MString("\tdefining roughness"));
					float value;
					getFloat("roughness", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setRoughness(value);
				}
				if( attName == "anisotropy")
				{
					logger.debug(MString("\tdefining anisotropy"));
					float value;
					getFloat("anisotropy", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setAnisotropy(value);
				}
				if( attName == "rotation")
				{
					logger.debug(MString("\tdefining rotation"));
					float value;
					getFloat("rotation", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setRotation(value);
				}
				if( attName == "bump")
				{
					logger.debug(MString("\tdefining bump"));
					float value;
					getFloat("bump", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setBump(value);
				}
				if( attName == "reflectance90Deg")
				{
					logger.debug(MString("\tdefining reflectance90Deg"));
					MColor value;
					getColor("reflectance90Deg", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setReflectance90Texture(TheaSDK::XML::RgbTexture(value.r, value.g, value.b));
				}
				if( attName == "normalMapping")
				{
					logger.debug(MString("\tdefining normalMapping"));
					bool value;
					getBool("normalMapping", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setNormalMapping(value);
				}
				if( attName == "absorptionColor")
				{
					logger.debug(MString("\t defining absorptionColor"));
					MColor value;
					getColor("absorptionColor", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setAbsorptionColor(TheaSDK::XML::RgbTexture(value.r, value.g, value.b));
				}
				if( attName == "absorptionDensity")
				{
					logger.debug(MString("\t defining absorptionDensity"));
					float value;
					getFloat("absorptionDensity", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setAbsorptionDensity(value);
				}
				if( attName == "ior")
				{
					logger.debug(MString("\t defining ior"));
					float value;
					getFloat("ior", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setIndexOfRefraction(value);
				}
				if( attName == "microRoughnessHeight")
				{
					logger.debug(MString("\t defining microRoughnessHeight"));
					float value;
					getFloat("microRoughnessHeight", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setMicroRoughnessHeight(value);
				}
				if( attName == "microRoughnessWidth")
				{
					logger.debug(MString("\t defining microRoughnessWidth"));
					float value;
					getFloat("microRoughnessWidth", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setMicroRoughnessWidth(value);
				}
				if( attName == "microRoughness")
				{
					logger.debug(MString("\t defining microRoughness"));
					bool value;
					getBool("microRoughness", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setMicroRoughness(value);
				}
				if( attName == "kappa")
				{
					logger.debug(MString("\t defining kappa"));
					float value;
					getFloat("kappa", depFn, value);
					(XBSDFBASICPTR(tsn.data))->setExtinctionCoefficient(value);
				}
			}
			if( tsn.type == TheaShadingNode::FILETEXTURE)
			{
				if( attName == "fileTextureName")
				{
					MString fileTextureName;
					getString(MString("fileTextureName"), depFn, fileTextureName);
					logger.debug(MString("setting fileTextureName to ") + fileTextureName);
					((TheaSDK::XML::BitmapTexture *) tsn.data)->setFilename(fileTextureName.asChar());	
					//((TheaSDK::XML::BitmapTexture *) tsn.data)->setInterpolation();	
					//((TheaSDK::XML::BitmapTexture *) tsn.data)->setProjection(place2DTexture);
					
				}
			}
		}
	}
}

void TheaRenderer::createTheaShadingNode(ShadingNode& sn, mtth_MayaObject *obj)
{			
	logger.debug("createTheaShadingNode");
	MFnDependencyNode depFn(sn.mobject);

	if( sn.typeName == "TheaMaterial")
	{
		logger.debug(MString("Defining theaMaterial: ") + sn.fullName);
				
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::Material *material = new TheaSDK::XML::Material;
			material->setName(sn.fullName.asChar());
			TheaShadingNode tsn;
			tsn.type = TheaShadingNode::MATERIAL;
			tsn.data = material;

			//material->setGlobalEmitter(true);
			//material->setPassiveEmitter(false);

			assignParameters(sn, tsn);
			theaShadingMap.add(sn.fullName.asChar(), tsn);
			sceneXML.addMaterial(*material);
			obj->xmlModel->materialName = material->getName();

			TheaSDK::RawXMLObject xml = material->convertToRawXML();
			std::string la = xml.getDescription();
			logger.debug(MString("mat desc: ") + la.c_str());
			la = xml.getLabel();
			logger.debug(MString("mat label: ") + la.c_str());
			la = xml.getName();
			logger.debug(MString("mat name: ") + la.c_str());
			la = xml.getType();
			logger.debug(MString("mat type: ") + la.c_str());

		}
	}else if ( sn.typeName == "BasicBSDF"){
		logger.debug(MString("Defining BasicBSDF: ") + sn.fullName);
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::BasicBSDF *basicBsdf = new TheaSDK::XML::BasicBSDF;
			
			TheaShadingNode tsn;
			tsn.type = TheaShadingNode::BASIC_BSDF;
			tsn.data = basicBsdf;
			
			assignParameters(sn, tsn);
			theaShadingMap.add(sn.fullName.asChar(), tsn);
		}
	}else if ( sn.typeName == "file"){
		logger.debug(MString("Defining file: ") + sn.fullName);
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::BitmapTexture *texture = new TheaSDK::XML::BitmapTexture;

			TheaShadingNode tsn;
			tsn.type = TheaShadingNode::FILETEXTURE;
			tsn.data = texture;

			assignParameters(sn, tsn);
			theaShadingMap.add(sn.fullName.asChar(), tsn);
		}
	}else if ( sn.typeName == "lambert"){
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::Material *lambMat = new TheaSDK::XML::Material;

			TheaShadingNode tsn;
			tsn.type = TheaShadingNode::MATERIAL;
			tsn.data = lambMat;

			TheaSDK::XML::BasicBSDF lambBsdf;
			MColor color;
			getColor("color", depFn, color);
			TheaSDK::XML::RgbTexture t(color.r, color.g, color.b);
			lambBsdf.setDiffuseTexture(t);
			lambMat->setName((sn.fullName + "_LambertMat").asChar());
			lambMat->setBSDF(lambBsdf);
			this->sceneXML.addMaterial(*lambMat);
			obj->xmlModel->materialName = lambMat->getName();

			theaShadingMap.add(sn.fullName.asChar(), tsn);
		}
	}else{
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			obj->xmlModel->materialName="DefaultMaterial";
		}
	}
}

void TheaRenderer::defineShader(mtth_MayaObject *obj)
{
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
		
	if( obj->shadingGroups.length() > 0)
	{		
		Material mat(obj->shadingGroups[0]);

		if( assignDefinedShader(obj, mat) )
			return;

		int numNodes = (int)mat.surfaceShaderNet.shaderList.size();
		for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
		{
			ShadingNode sn = mat.surfaceShaderNet.shaderList[shadingNodeId];
			logger.debug(MString("SNode Id: ") + shadingNodeId + " " + sn.fullName);
			this->createTheaShadingNode(mat.surfaceShaderNet.shaderList[shadingNodeId], obj);
		}

	}
}