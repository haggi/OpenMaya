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
#include "TheaXMLUtil.h"

static Logging logger;

static ShadingNodeMap theaShadingMap;

#define XBSDFBASICPTR (TheaSDK::XML::BasicBSDF *)
#define XMATPTR (TheaSDK::XML::Material *)
#define XTEXPTR (TheaSDK::XML::BitmapTexture *)

static std::vector<Object> objList;

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
	objList.clear();
}

void TheaRenderer::setTextureOrColor(const char *attrName, MObject& shaderNode, void *theaTextureNode)
{}

void TheaRenderer::setTextureOrFloat(const char *attrName, MObject& shaderNode, void *theaTextureNode)
{}

void TheaRenderer::assignObjParameters(ShadingNode& sn, Object& o)
{
	MFnDependencyNode depFn(sn.mobject);
	MColor c;
	float f;
	int i;
	MString s;
	double d;
	bool b;

	if (o.identifier == "Physically Layered Material")
	{
		MPlug layersPlug = depFn.findPlug("layers");
		// children 0=weight 1=texture 2=shader
		for (uint lId = 0; lId < layersPlug.numElements(); lId++)
		{
			MPlug thisLayer = layersPlug[lId];
			float weight = thisLayer.child(0).asFloat();
			MObject connectedTexture = getConnectedInNode(thisLayer.child(1));
			MObject connectedShader = getConnectedInNode(thisLayer.child(2));
			MString shaderName = getObjectName(connectedShader);
			MString textureName = getObjectName(connectedTexture);
			for (size_t oId = 0; oId < objList.size(); oId++)
			{
				Object co = objList[oId];
				if (co.mayaName == shaderName)
				{
					logger.debug(MString("Found connected layer shader: ") + shaderName + " in layer " + lId);
					co.identifier = MString("./Layer #") + lId + "/" + co.mayaName;
					//o.childObjectList.push_back(co);
					o.LayerList.push_back(co);
				}
				if (connectedTexture != MObject::kNullObj)
				{
					if (co.mayaName == textureName)
					{
						logger.debug(MString("Found connected layer textureName: ") + textureName + " in layer " + lId);
						co.identifier = MString("./Weight Map #") + lId + "/" + co.mayaName;
						//o.childObjectList.push_back(co);	
						o.LayerTextureList.push_back(co);
					}
				}
				else{
					Object dummy("Dummy", "Dummy", "Dummy", "Dummy", "Dummy");
					o.LayerTextureList.push_back(dummy);
				}
			}
			Parameter p(MString("./Weight #") + lId + "/Weight", weight / 100.0f);
			o.LayerWeightList.push_back(p);
		}
	}


	for( int idx = 0; idx < sn.inputAttributes.size(); idx++)
	{
		ShaderAttribute sa = sn.inputAttributes[idx];

		if (!isConnected(sa.name.c_str(), sn.mobject, true))
		{
			if( sa.type == "color" )
			{
				getColor(sa.name.c_str(), depFn, c);
				if( getTexMap(sa.name.c_str()) != "" )
				{
					o.childObjectList.push_back(colorTextureObject(sa.name.c_str(), c));
				}else{
					Parameter p(sa.name.c_str(), c);
					o.parameterList.push_back(p);
				}
			}
			if( sa.type == "float" )
			{
				getFloat(sa.name.c_str(), depFn, f);
				Parameter p(sa.name.c_str(), f);
				o.parameterList.push_back(p);
			}
			if( sa.type == "int" )
			{
				getInt(sa.name.c_str(), depFn, i);
				Parameter p(sa.name.c_str(), i);
				o.parameterList.push_back(p);
			}
			if( sa.type == "bool" )
			{
				getBool(sa.name.c_str(), depFn, b);
				Parameter p(sa.name.c_str(), b);
				o.parameterList.push_back(p);
			}
			if( sa.type == "string" )
			{
				getString(MString(sa.name.c_str()), depFn, s);
				Parameter p(sa.name.c_str(), s);
				if( sn.typeName == "file" )
				{
					if( sa.name == "fileTextureName")
						p.type = "file";
				}
				o.parameterList.push_back(p);
			}
			if( sa.type == "enum" )
			{
				int id;
				getEnum(MString(sa.name.c_str()), depFn, id, s);
				Parameter p(sa.name.c_str(), s);
				o.parameterList.push_back(p);
			}
		}else{

			MPlug directPlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
			MString nodeName = getObjectName(directPlug.node());
			logger.debug(MString("Search for connected obj ") + nodeName);
			for( size_t oId = 0; oId < objList.size(); oId++)
			{
				Object co = objList[oId];
				if (co.mayaName == nodeName)
				{
					logger.debug(MString("Found connected obj ") + co.mayaName);
					if(co.type == "Texture" )
					{
						if( sa.type == "color" )
						{
							//"./Diffuse/Bitmap Texture"
							logger.debug(MString("found bitmap texture ") + sa.name.c_str());
							co.identifier = MString("./") + getTexMap(sa.name) + "/" + co.identifier;
						}else{
							//"./Roughness Map/Bitmap Texture"
							co.identifier = MString("./") + getParamName(sa.name) + " Map/" + co.identifier;
						}
					}
					if ((o.LayerList.size() > 0) && (sa.name == "bsdf"))
					{
						o.LayerList.push_back(co);
						Parameter p(MString("./Weight #") + (o.LayerList.size() - 1) + "/Weight", 1.0f);
						o.LayerWeightList.push_back(p);
						Object dummy("Dummy", "Dummy", "Dummy", "Dummy", "Dummy");
						o.LayerTextureList.push_back(dummy);
					}
					else{
						o.childObjectList.push_back(co);
					}
				}
			}
		}
	}
}


void TheaRenderer::createTheaShadingNode(ShadingNode& sn, mtth_MayaObject *obj)
{
	logger.debug("createTheaShadingNode");
	MFnDependencyNode depFn(sn.mobject);
	initMaps();


	if (sn.typeName == "TheaMaterial")
	{
		logger.debug(MString("Defining theaMaterial: ") + sn.fullName);

		if (this->mtth_renderGlobals->exportSceneFile)
		{
			// reuse already defined materials
			for (size_t oId = 0; oId < objList.size(); oId++)
			{
				if (objList[oId].mayaName == sn.fullName)
				{
					logger.debug(MString("Reusing material ") + sn.fullName);
					obj->xmlModel->materialName = sn.fullName.asChar();
					return;
				}
			}

			TheaSDK::XML::Material *material = new TheaSDK::XML::Material;
			material->setName(sn.fullName.asChar());
			TheaSDK::RawXMLObject xml = material->convertToRawXML();
			std::string la = xml.getDescription();

			Object o(sn.fullName, sn.fullName, "Material", "Physically Layered Material", "Physically Layered Material");
			assignObjParameters(sn, o);
			o.integrate = true;
			logger.debug(o.get());
			objList.push_back(o);

			material->setRawXMLDescription(o.get().asChar());

			sceneXML.addMaterial(*material);
			obj->xmlModel->materialName = material->getName();
		}
	}
	else if (sn.typeName == "BasicBSDF"){
		logger.debug(MString("Defining BasicBSDF: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Material", "Basic Material", "Basic Material");
			assignObjParameters(sn, o);
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "DiffuseLight"){
		logger.debug(MString("Defining Diffuse emitter: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Emittance", "Diffuse Light", "Diffuse Light");
			assignObjParameters(sn, o);
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "GlossyBSDF"){
		logger.debug(MString("Defining GlossyBSDF: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Material", "Glossy Material", "Glossy Material");
			assignObjParameters(sn, o);
			objList.push_back(o);

		}
	}
	else if (sn.typeName == "file"){
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Bitmap Texture", "Bitmap Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaBlackBody"){
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "BlackBody Texture", "BlackBody Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaChecker")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Checker Texture", "Checker Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaConcentric")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Concentric Texture", "Concentric Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaCurl")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Curl Texture", "Curl Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaGradient")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Gradient Texture", "Gradient Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaMarble")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Marble Texture", "Marble Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaPerlin")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Perlin Texture", "Perlin Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaVoronoi")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Voronoi Texture", "Voronoi Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaWindy")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Windy Texture", "Windy Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaWireframe")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Wireframe Texture", "Wireframe Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "TheaWood")
	{
		logger.debug(MString("Defining file: ") + sn.fullName);
		if (this->mtth_renderGlobals->exportSceneFile)
		{
			Object o(sn.fullName, sn.fullName, "Texture", "Wood Texture", "Wood Texture");
			assignObjParameters(sn, o);
			logger.debug(o.get());
			objList.push_back(o);
		}
	}
	else if (sn.typeName == "lambert"){
		if (this->mtth_renderGlobals->exportSceneFile)
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
	}
	else{
		if( this->mtth_renderGlobals->exportSceneFile )
		{
			obj->xmlModel->materialName="DefaultMaterial";
		}
	}
}

void TheaRenderer::defineShader(mtth_MayaObject *obj)
{
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups, true);
		
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