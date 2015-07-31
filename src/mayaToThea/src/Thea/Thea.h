#ifndef THEA_RENDER_H
#define THEA_RENDER_H

#include <vector>
#include "rendering/renderer.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"

#include <maya/MObject.h>
#include <maya/MMatrix.h>
#include <maya/MColor.h>
#include <maya/MString.h>

#include <SDK/Integration/sdk.h>
#include <SDK/Integration/sdk.plus.h>

#include <SDK/XML/sdk.xml.importer.h>

#define THEASDK_OOP_DESIGN
#ifdef THEASDK_OOP_DESIGN
	#include <SDK/Integration/sdk.scene.h>
#endif

#include "TheaXMLUtil.h"

class mtth_MayaObject;

class TheaShadingNode
{
public:
	enum ShaderType { UNDEF=0, BASIC_BSDF, BSSDF_BSDF, COATING_BSDF, THINFILM_BSDF, GLOSSY_BSDF, MATERIAL, FILETEXTURE, CHECHERTEXTURE, CURLTEXTURE, MARBLETEXTURE};
	void *data;
	ShaderType type;

	TheaShadingNode()
	{
		data = NULL;
		type = UNDEF;
	}
	~TheaShadingNode()
	{
		if( data != NULL )
		{}
	}
};

class ShadingNodeMap
{
public:
	std::vector<std::string> names;
	std::vector<TheaShadingNode> shadingNodes;
	void add(std::string name, TheaShadingNode& sn)
	{
		TheaShadingNode tmpsn;
		if(!this->find(name, tmpsn))
		{
			names.push_back(name);
			shadingNodes.push_back(sn);
		}
	}
	bool find(std::string name, TheaShadingNode& sn)
	{
		for( size_t i = 0; i < names.size(); i++)
		{
			if( names[i] == name)
			{
				sn = shadingNodes[i];
				return true;
			}
		}
		return false;
	}
	void clear()
	{
		names.clear();
		shadingNodes.clear();
	}
};


class TheaRenderer : public MayaTo::Renderer
{
public:

	TheaSDK::Scene scene;
	TheaSDK::XML::Scene sceneXML;
	bool isGood;

	//std::vector<std::shared_ptr<MayaObject>> interactiveUpdateList;
	//std::vector<MObject> interactiveUpdateMOList;
	virtual void interactiveFbCallback();

	TheaRenderer();
	~TheaRenderer();
	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineLights();
	void defineSettings();
	TheaSDK::Normal3D getSunDirection();

	virtual void render();

	virtual void initializeRenderer();
	virtual void unInitializeRenderer();
	virtual void updateShape(std::shared_ptr<MayaObject> obj);
	virtual void updateTransform(std::shared_ptr<MayaObject> obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();

	void defineMesh(mtth_MayaObject *obj);
	static void frameBufferCallback();
	static void renderEndCallback( void *ptr );
	void doTestRender();
	void matrixToTransform(MMatrix& m, TheaSDK::Transform& t);
	void defineShader(mtth_MayaObject *obj);
	void defineIBLNode(TheaSDK::XML::EnvironmentOptions::IBLMap&, const char *attName);
	void setTextureOrColor(const char *attrName, MObject& shaderNode, void *theaTextureNode);
	void setTextureOrFloat(const char *attrName, MObject& shaderNode, void *theaTextureNode);
	void createDefaultMaterial();
	bool assignDefinedShader(mtth_MayaObject *obj, Material& mat);
	void clearMaterialLists();
	void createTheaShadingNode(ShadingNode& sn, mtth_MayaObject *obj);
	void assignParameters(ShadingNode& sn, TheaShadingNode& tsn);
	void assignObjParameters(ShadingNode& sn, Object& o);
};

#endif