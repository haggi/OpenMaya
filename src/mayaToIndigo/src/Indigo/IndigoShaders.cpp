#include "Indigo.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "../mtin_common/mtin_mayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include <maya/MStringArray.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnNumericData.h>
#include <vector>

static Logging logger;

struct MaterialNode{
	MString name;
	Indigo::SceneNodeMaterialRef materialNode;
};

struct MediumNode{
	MString name;
	Indigo::SceneNodeMediumRef mediumNode;
};

struct IndigoTextureNode{
	ShadingNode shadingNode;
	Indigo::Texture texture;
	int id;
	IndigoTextureNode()
	{
		id = -1;
	};
};

std::vector<MaterialNode> materialNodes;
std::vector<IndigoTextureNode> textureNodes;
std::vector<MediumNode> mediumNodes;


void IndigoRenderer::createIndigoDefaultMaterial()
{
	this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
	Indigo::DiffuseMaterial* diffuse = new Indigo::DiffuseMaterial();
	diffuse->random_triangle_colours = false;
	diffuse->layer = 0;
	diffuse->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(.9,.8,.5), 2.2));
	this->currentSceneNodeMaterialRef->material = diffuse;
	sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
}

struct parameter{
	Reference<Indigo::WavelengthDependentParam> dependentParam;
	Reference<Indigo::WavelengthIndependentParam> independentParam;
	float f;
	int i;
	double d;
	MString s;
	MString name;
	enum PTYPE {
		DependentParam,
		IndependenParam,
		Float,
		Int,
		Double,
		String
	};
	PTYPE type;
};

// Indigo::ConstantWavelengthIndependentParam(value)
// Indigo::ShaderWavelengthIndependentParam(value)
// Indigo::TextureWavelengthIndependentParam(value)

// Indigo::ConstantWavelengthDependentParam(value)
// Indigo::ShaderWavelengthDependentParam(value)
// Indigo::TextureWavelengthDependentParam(value)

void IndigoRenderer::setIndependentParameter(Reference<Indigo::WavelengthIndependentParam>& p, MFnDependencyNode& depFn, MString attrName, MString type)
{
	MStatus stat;
	MPlug attrPlug = depFn.findPlug(attrName);

	MObject textureObj = getConnectedFileTextureObject(attrName, depFn);
	if(textureObj != MObject::kNullObj)
	{
		IndigoTextureNode *tNode = NULL;
		for( size_t txId(0); txId < textureNodes.size(); txId++)
		{	
			if( textureNodes[txId].shadingNode.mobject == textureObj )
			{
				tNode = &textureNodes[txId];
				logger.debug(MString("Found texture node ") +  textureNodes[txId].shadingNode.fullName );
			}
		}
		if( tNode )
		{
			if( tNode->id < 0)
			{
				this->currentMaterial->textures.push_back(tNode->texture);
				tNode->id = this->currentMaterial->textures.size() - 1;
			}
			p = Reference<Indigo::WavelengthIndependentParam>(new Indigo::TextureWavelengthIndependentParam(tNode->id));
		}else{
			logger.debug(MString("Could not find texture node ") +  getObjectName(textureObj) );
		}
	}else{
		float gamma = 2.2f;
		if( type == "color" )
		{
			MColor color(1,1,1);
			getColor(attrName, depFn, color);
			Indigo::RGBSpectrum *iColor = new Indigo::RGBSpectrum(Indigo::Vec3d(color.r,color.g,color.b), gamma);
			double mix = (color.r + color.g + color.b)/3.0;
			p = Reference<Indigo::WavelengthIndependentParam>(new Indigo::ConstantWavelengthIndependentParam(mix));
		}
		if( type == "float" )
		{
			float value;
			getFloat(attrName, depFn, value);
			p = Reference<Indigo::WavelengthIndependentParam>(new Indigo::ConstantWavelengthIndependentParam(value));
		}				
		if( type == "double" )
		{
			double value;
			getDouble(attrName, depFn, value);
			p = Reference<Indigo::WavelengthIndependentParam>(new Indigo::ConstantWavelengthIndependentParam(value));
		}				
	}
}

void IndigoRenderer::setDependentParameter(Reference<Indigo::WavelengthDependentParam>& p, MFnDependencyNode& depFn, MString attrName, MString type)
{
	MStatus stat;
	MPlug attrPlug = depFn.findPlug(attrName);

	MObject textureObj = getConnectedFileTextureObject(attrName, depFn);
	if(textureObj != MObject::kNullObj)
	{
		IndigoTextureNode *tNode = NULL;
		for( size_t txId(0); txId < textureNodes.size(); txId++)
		{	
			if( textureNodes[txId].shadingNode.mobject == textureObj )
			{
				tNode = &textureNodes[txId];
				logger.debug(MString("Found texture node ") +  textureNodes[txId].shadingNode.fullName );
			}
		}
		if( tNode )
		{
			if( tNode->id < 0)
			{
				this->currentMaterial->textures.push_back(tNode->texture);
				tNode->id = this->currentMaterial->textures.size() - 1;
			}
			p = Reference<Indigo::WavelengthDependentParam>(new Indigo::TextureWavelengthDependentParam(tNode->id));
		}else{
			logger.debug(MString("Could not find texture node ") +  getObjectName(textureObj) );
		}
	}else{
		float gamma = 2.2f;
		if( type == "color" )
		{			
			MColor color(1,1,1);
			getColor(attrName, depFn, color);
			Indigo::RGBSpectrum *iColor = new Indigo::RGBSpectrum(Indigo::Vec3d(color.r,color.g,color.b), gamma);
			p = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(iColor)));
		}
		if( type == "float" )
		{
			float value;
			getFloat(attrName, depFn, value);
			Indigo::RGBSpectrum *iColor = new Indigo::RGBSpectrum(Indigo::Vec3d(value,value,value), gamma);
			p = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(iColor)));
		}				
		if( type == "double" )
		{
			double value;
			getDouble(attrName, depFn, value);
			Indigo::RGBSpectrum *iColor = new Indigo::RGBSpectrum(Indigo::Vec3d(value,value,value), gamma);
			p = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(iColor)));
		}				
	}
}


void IndigoRenderer::createIndigoShadingNode(ShadingNode& snode)
{
	logger.debug(MString("createIndigoShadingNode: ") + snode.fullName);
	MFnDependencyNode depFn(snode.mobject);

	if( snode.typeName == "file")
	{
		Indigo::Texture texture;
		MString texturePath;
		getString(MString("fileTextureName"), depFn, texturePath);	
		texture.path = texturePath.asChar();
		texture.exponent = 1.0; // gamma
		texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::UVTexCoordGenerator());
		IndigoTextureNode tn;
		tn.shadingNode = snode;
		tn.texture = texture;
		textureNodes.push_back(tn);
	}

	if( snode.typeName == "inBlend")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::BlendMaterial* blend = new Indigo::BlendMaterial();
		this->currentSceneNodeMaterialRef->material = blend;
		this->currentMaterial = blend;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];

			if( sa.name == "a_name")
			{
				if(!sa.connected )
					logger.error(MString("GlossyTransparent shader needs a medium connection: ") + snode.fullName);

				MObject connectedMedium;
				getMsgObj("a_name", depFn, connectedMedium);
				if( connectedMedium != MObject::kNullObj)
				{
					MString connectedObjectName = getObjectName(connectedMedium);
					for( size_t mId(0); mId < materialNodes.size(); mId++)
					{
						if( materialNodes[mId].name == connectedObjectName)
						{
							logger.debug(MString("Found medium: ") + materialNodes[mId].name);
							blend->a_mat = materialNodes[mId].materialNode;
						}
					}
				}
			}
			if( sa.name == "b_name")
			{
				if(!sa.connected )
					logger.error(MString("GlossyTransparent shader needs a medium connection: ") + snode.fullName);

				MObject connectedMedium;
				getMsgObj("b_name", depFn, connectedMedium);
				if( connectedMedium != MObject::kNullObj)
				{
					MString connectedObjectName = getObjectName(connectedMedium);
					for( size_t mId(0); mId < materialNodes.size(); mId++)
					{
						if( materialNodes[mId].name == connectedObjectName)
						{
							logger.debug(MString("Found medium: ") + materialNodes[mId].name);
							blend->b_mat = materialNodes[mId].materialNode;
						}
					}
				}
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, blend->backface_emit);
			}
			if( sa.name == "step_blend")
			{
				getBool("step_blend", depFn, blend->step_blend);
			}
			if( sa.name == "blend")
			{
				double blendValue;
				getDouble(MString("blend"), depFn, blendValue);
				blend->blend = new Indigo::ConstantDisplacementParam(blendValue);				
			}
		}


		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

	if( snode.typeName == "inDiffuseTransmitter")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::DiffuseTransmitter* diftrans = new Indigo::DiffuseTransmitter();
		this->currentMaterial = diftrans;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, diftrans->backface_emit);
			}
		}
		
		this->currentSceneNodeMaterialRef->material = diftrans;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

	if( snode.typeName == "inNull")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::NullMaterial* null = new Indigo::NullMaterial();
		this->currentMaterial = null;
		this->currentMaterial->name = snode.fullName.asChar();
		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, null->backface_emit);
			}

		}
		this->currentSceneNodeMaterialRef->material = null;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

	if( snode.typeName == "inSpecular")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::SpecularMaterial* spec = new Indigo::SpecularMaterial();
		this->currentMaterial = spec;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "emission")
			{
				setDependentParameter(spec->emission, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, spec->emission);
				//else
				//	setColorDependentParameter(spec->emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "base_emission")
			{
				setDependentParameter(spec->base_emission, depFn, sa.name.c_str(), sa.type.c_str());
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, spec->backface_emit);
			}
			if( sa.name == "layer")
			{
				getUInt("layer", depFn, spec->layer);
			}
			if( sa.name == "transparent")
			{
				getBool("transparent", depFn, spec->transparent);
			}
			if( sa.name == "internal_medium_name")
			{
				if(!sa.connected )
					logger.error(MString("GlossyTransparent shader needs a medium connection: ") + snode.fullName);

				MObject connectedMedium;
				getMsgObj("internal_medium_name", depFn, connectedMedium);
				if( connectedMedium != MObject::kNullObj)
				{
					MString connectedObjectName = getObjectName(connectedMedium);
					for( size_t mId(0); mId < mediumNodes.size(); mId++)
					{
						if( mediumNodes[mId].name == connectedObjectName)
						{
							logger.debug(MString("Found medium: ") + mediumNodes[mId].name);
							spec->internal_medium = mediumNodes[mId].mediumNode;
						}
					}
				}
			}
		}	
		this->currentSceneNodeMaterialRef->material = spec;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);

	}

	if( snode.typeName == "inDiffuse")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::DiffuseMaterial *diffuse = new Indigo::DiffuseMaterial();	
		this->currentMaterial = diffuse;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "albedo")
			{
				setDependentParameter(diffuse->albedo, depFn, sa.name.c_str(), sa.type.c_str());
			}
			if( sa.name == "emission")
			{
				setDependentParameter(diffuse->emission, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, diffuse->emission);
				//else
				//	setColorDependentParameter(diffuse->emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "base_emission")
			{
				setDependentParameter(diffuse->base_emission, depFn, sa.name.c_str(), sa.type.c_str());
				//setColorDependentParameter(diffuse->base_emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, diffuse->backface_emit);
			}
			if( sa.name == "layer")
			{
				getUInt("layer", depFn, diffuse->layer);
			}
		}
					
		this->currentSceneNodeMaterialRef->material = diffuse;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

	if( snode.typeName == "inOrenNayar")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::OrenNayarMaterial *oren = new Indigo::OrenNayarMaterial();	
		this->currentMaterial = oren;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "sigma")
			{
				setIndependentParameter(oren->sigma, depFn, sa.name.c_str(), sa.type.c_str());

				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureIndependentParamter(sa.connectedMObject, oren->sigma);
				//else
				//	setColorIndependentParameter(oren->sigma, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "albedo")
			{
				setDependentParameter(oren->albedo, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, oren->albedo);
				//else
				//	setColorDependentParameter(oren->albedo, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "emission")
			{
				setDependentParameter(oren->emission, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, oren->emission);
				//else
				//	setColorDependentParameter(oren->emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "base_emission")
			{
				setDependentParameter(oren->base_emission, depFn, sa.name.c_str(), sa.type.c_str());
				//setColorDependentParameter(oren->base_emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, oren->backface_emit);
			}
			if( sa.name == "layer")
			{
				getUInt("layer", depFn, oren->layer);
			}

		}
					
		this->currentSceneNodeMaterialRef->material = oren;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}


	if( snode.typeName == "inPhong")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::PhongMaterial* phong = new Indigo::PhongMaterial();
		this->currentMaterial = phong;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "diffuse_albedo")
			{
				setDependentParameter(phong->diffuse_albedo, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, phong->diffuse_albedo);
				//else
				//	setColorDependentParameter(phong->diffuse_albedo, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "emission")
			{
				setDependentParameter(phong->emission, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, phong->emission);
				//else
				//	setColorDependentParameter(phong->emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "base_emission")
			{
				setDependentParameter(phong->base_emission, depFn, sa.name.c_str(), sa.type.c_str());
				//setColorDependentParameter(phong->base_emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "layer")
			{
				getUInt("layer", depFn, phong->layer);
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, phong->backface_emit);
			}
			if( sa.name == "exponent")
			{
				setIndependentParameter(phong->exponent, depFn, sa.name.c_str(), sa.type.c_str());
				//setFloatIndependentParameter(phong->exponent, depFn, MString("exponent"));
			}
			if( sa.name == "fresnelScale")
			{
				setIndependentParameter(phong->fresnel_scale, depFn, sa.name.c_str(), sa.type.c_str());
			}
			if( sa.name == "specular_reflectivity")
			{
				setDependentParameter(phong->specular_reflectivity, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, phong->specular_reflectivity);
				//else
				//	setColorDependentParameter(phong->specular_reflectivity, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "ior")
			{
				getDouble(MString("ior"), depFn, phong->ior);
			}
		}	
		this->currentSceneNodeMaterialRef->material = phong;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

	if( snode.typeName == "inMediumBasic")
	{
		Indigo::BasicMedium* medium = new Indigo::BasicMedium();
		Indigo::SceneNodeMediumRef mediumRef = new Indigo::SceneNodeMedium();
		mediumRef->medium = medium;
		this->sceneRootRef->addChildNode(mediumRef); // exception, mediums have to be added to the scene seperatly
		medium->name = snode.fullName.asChar();
		
		bool useSubsurfaceScatter = false;
		getBool("subsurface_scattering", depFn, useSubsurfaceScatter);

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];

			if( (sa.name == "scattering_coefficient_spectrum") && (useSubsurfaceScatter))
			{
				MColor scs;
				getColor("scattering_coefficient_spectrum", depFn, scs);
				Indigo::RGBSpectrum *color = new Indigo::RGBSpectrum(Indigo::Vec3d(scs.r,scs.g,scs.b), 2.2);
				medium->subsurface_scattering->scattering_coefficient = Reference<Indigo::VolumeParam>(new Indigo::ConstantVolumeParam(Reference<Indigo::Spectrum>(color)));
			}
			if( sa.name == "absorption_coefficient_spectrum")
			{
				MColor acs;
				getColor("absorption_coefficient_spectrum", depFn, acs);
				Indigo::RGBSpectrum *color = new Indigo::RGBSpectrum(Indigo::Vec3d(acs.r,acs.g,acs.b), 2.2);				
				medium->absorption_coefficient = Reference<Indigo::VolumeParam>(new Indigo::ConstantVolumeParam(Reference<Indigo::Spectrum>(color)));
			}
			if( sa.name == "ior")
			{
				getDouble(MString("ior"), depFn, medium->ior);
			}
			if( sa.name == "precedence")
			{
				getInt(MString("precedence"), depFn, medium->precedence);
			}
			if( sa.name == "phase_function_hgg")
			{}

			if( sa.name == "phase_function")
			{
				//MColor acs;
				//getColor("phase_function_hgg", depFn, acs);				
				//medium->subsurface_scattering->phase_function = Reference<Indigo::PhaseFunction>(new Indigo::HenyeyGreensteinPhaseFunction());
				//Indigo::RGBSpectrum *color = new Indigo::RGBSpectrum(Indigo::Vec3d(acs.r,acs.g,acs.b), 2.2);				
				//medium->absorption_coefficient = Reference<Indigo::VolumeParam>(new Indigo::ConstantVolumeParam(Reference<Indigo::Spectrum>(color)));
			}
			if( sa.name == "cauchy_b_coeff")
			{
			}
		}	
		MediumNode mn;
		mn.mediumNode = mediumRef;
		mn.name = snode.fullName.asChar();
		mediumNodes.push_back(mn);
	}

	if( snode.typeName == "inGlossyTransparent")
	{
		this->currentSceneNodeMaterialRef = new Indigo::SceneNodeMaterial();
		Indigo::GlossyTransparent* glossy = new Indigo::GlossyTransparent();
		this->currentMaterial = glossy;
		this->currentMaterial->name = snode.fullName.asChar();

		for( size_t atId(0); atId < snode.inputAttributes.size(); atId++)
		{
			ShaderAttribute sa = snode.inputAttributes[atId];
			if( sa.name == "internal_medium_name")
			{
				if(!sa.connected )
					logger.error(MString("GlossyTransparent shader needs a medium connection: ") + snode.fullName);

				MObject connectedMedium;
				getMsgObj("internal_medium_name", depFn, connectedMedium);
				if( connectedMedium != MObject::kNullObj)
				{
					MString connectedObjectName = getObjectName(connectedMedium);
					for( size_t mId(0); mId < mediumNodes.size(); mId++)
					{
						if( mediumNodes[mId].name == connectedObjectName)
						{
							logger.debug(MString("Found medium: ") + mediumNodes[mId].name);
							glossy->internal_medium = mediumNodes[mId].mediumNode;
						}
					}
				}
			}
			if( sa.name == "emission")
			{
				setDependentParameter(glossy->emission, depFn, sa.name.c_str(), sa.type.c_str());
				//if(sa.connected && sa.connectedMObject.hasFn(MFn::kFileTexture))
				//	setTextureDependentParamter(sa.connectedMObject, glossy->emission);
				//else
				//	setColorDependentParameter(glossy->emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "base_emission")
			{
				setDependentParameter(glossy->base_emission, depFn, sa.name.c_str(), sa.type.c_str());
				//setColorDependentParameter(glossy->base_emission, depFn, MString(sa.name.c_str()));
			}
			if( sa.name == "layer")
			{
				getUInt("layer", depFn, glossy->layer);
			}
			if( sa.name == "backface_emit")
			{
				getBool("backface_emit", depFn, glossy->backface_emit);
			}
			if( sa.name == "exponent")
			{
				setIndependentParameter(glossy->exponent, depFn, sa.name.c_str(), sa.type.c_str());
			}
		}	
		this->currentSceneNodeMaterialRef->material = glossy;
		sceneRootRef->addChildNode(this->currentSceneNodeMaterialRef);
		MaterialNode mn;
		mn.name = depFn.name();
		mn.materialNode = this->currentSceneNodeMaterialRef;
		materialNodes.push_back(mn);
	}

}

void IndigoRenderer::defineShadingNodes(mtin_MayaObject *obj)
{
	for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MObject shadingGroup = obj->shadingGroups[sgId];
		MString shadingGroupName = getObjectName(shadingGroup);
		logger.debug(MString("Define Material from shadingGroup: ") + shadingGroupName);
		this->createIndigoDefaultMaterial();
		
		Material material(shadingGroup);
		logger.debug(MString("Translating material for SG: ") + material.materialName);
		this->currentSceneNodeMaterialRef->setName( material.materialName.asChar());
		
		int numNodes = (int)material.surfaceShaderNet.shaderList.size();
		for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
		{
			ShadingNode sn = material.surfaceShaderNet.shaderList[shadingNodeId];
			logger.debug(MString("SNode Id: ") + shadingNodeId + " " + sn.fullName);
			this->createIndigoShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId]);
		}

		obj->matRef = this->currentSceneNodeMaterialRef;

		//bool found = false;
		//for( int shadingNodeId = numNodes - 1; shadingNodeId >= 0; shadingNodeId-- )
		//{
		//	for( size_t mnId = 0; mnId < definedMaterialNodes.size(); mnId++)
		//	{
		//		if( material.surfaceShaderNet.shaderList[shadingNodeId].fullName == definedMaterialNodes[mnId].name )
		//		{
		//			logger.debug(MString("Object ") + obj->shortName + " will receive material " + definedMaterialNodes[mnId].name);
		//			obj->matRef = definedMaterialNodes[mnId].materialNode;
		//			found = true;
		//			break;
		//		}
		//	}
		//	if( found )
		//		break;
		//}
	}
}