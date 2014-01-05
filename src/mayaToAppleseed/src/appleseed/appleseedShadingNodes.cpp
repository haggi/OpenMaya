#include "appleseed.h"
#include "renderer/api/edf.h"
#include "renderer/api/texture.h"
#include "renderer/modeling/surfaceshader/fastsubsurfacescatteringsurfaceshader.h"
#include <maya/MColor.h>

#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"


static Logging logger;

using namespace AppleRender;

void AppleseedRenderer::defineColor(MString attrName, MObject& mobject)
{
	logger.feature(MString("Define color: ") + attrName);
	MColor color;
	MFnDependencyNode dn(mobject);
	getColor(attrName, dn, color);
	float col[3];
	float alpha;
	mayaColorToFloat(color, col, &alpha);
	MString colorSpace = "srgb";
	MString colorName = getObjectName(mobject) + "." + attrName;
	
	logger.feature(MString("Creating Color object : ") + colorName);

	removeColorEntityIfItExists(colorName);

	this->scenePtr->colors().insert(asr::ColorEntityFactory::create(
				colorName.asChar(),	
				asr::ParamArray()
					.insert("color_space", colorSpace.asChar()),
				asr::ColorValueArray(3, col),
				asr::ColorValueArray(1, &alpha)));

}

void AppleseedRenderer::addNodeParameters(ShadingNode& sn, asr::ParamArray& pa)
{
	logger.feature(MString("addNodeParameters for node: ") + sn.fullName);
	MFnDependencyNode depFn(sn.mobject);

	for( size_t inputAttId = 0; inputAttId < sn.inputAttributes.size(); inputAttId++)
	{
		ShaderAttribute sa = sn.inputAttributes[inputAttId];
		logger.feature(MString("Translating nodeAtt : ") + sa.name.c_str() + " type: " + sa.type.c_str() + " connected " + sa.connected);
		MString parameterValue; 
		// colors needs to be defined as color entity in appleseed. So we define them and use the defined name as parameter.
		if( sa.connected || MString(sa.type.c_str()) == "color" || MString(sa.type.c_str()) == "varying_color" || MString(sa.type.c_str()) == "uniform_color")
		{
			logger.feature(MString("Connected or color."));
			if( sa.connected )
				parameterValue = getObjectName(sa.connectedMObject).asChar();
			else{
				parameterValue = sn.fullName + "." + MString(sa.name.c_str());
				defineColor(sa.name.c_str(), sn.mobject);
			}
		}
		else if( MString(sa.type.c_str()) == "uniform_float" || MString(sa.type.c_str()) == "uniform_scalar" || MString(sa.type.c_str()) == "float")
		{
			float f = 0.0f;
			getFloat(sa.name.c_str(), depFn, f);
			parameterValue = MString("") + f; 
		}
		else if( MString(sa.type.c_str()) == "int")
		{
			int in = 0;
			getInt(sa.name.c_str(), depFn, in);
			parameterValue = MString("") + in; 
		}
		else if( MString(sa.type.c_str()) == "enum")
		{
			MString enumValue="";
			int enumInt = 0;
			getEnum(sa.name.c_str(), depFn, enumInt, enumValue);
			parameterValue = enumValue; 
		}
		else if( MString(sa.type.c_str()) == "boolean")
		{
			bool value = false;
			MString boolValue = "false";
			getBool(sa.name.c_str(), depFn, value);
			if( value )
				boolValue = "true";
			parameterValue = boolValue; 
		}

		if( parameterValue.length() > 0)
			pa.insert(sa.name.c_str(), parameterValue.asChar());
	}
	
}

void AppleseedRenderer::createMayaShadingNode(ShadingNode& sn, asr::Assembly* localAssembly)
{
	logger.feature(MString("Create maya shading node: ") + sn.fullName + " maya NodeType name " + sn.typeName);
	MFnDependencyNode depFn(sn.mobject);

	if( sn.typeName == "file")
	{
		defineTexture(sn.mobject); 
	}
}

void AppleseedRenderer::createAppleseedShadingNode(ShadingNode& sn, asr::Assembly* localAssembly)
{
	if( localAssembly == NULL )
	{	
		logger.error("createAppleseedShadingNode::Local assembly is NULL.");
		return;
	}

	// not ideal, try to get plugin node list 
	MString appleName = pystring::slice(sn.typeName.asChar(), 2).c_str();
	
	MFnDependencyNode depFn(sn.mobject);
	MString classification = depFn.classification(sn.typeName);
	if( pystring::find(classification.asChar(), "appleseed") == -1)
	{
		logger.feature(MString("Node is not a appleseed node: Trying to create maya node: ") + sn.typeName);
		createMayaShadingNode(sn, localAssembly);
		return;
	}

	logger.feature(MString("Create appleseed shading node: ") + sn.fullName + " appleseed Node name " + appleName);

	bool nodeFound = false;

//Surface_shader_collection
//Bsdf_mix
//Lambertian_brdf
//Specular_brdf
//Constant_surface_shader
//Diffuse_btdf
//Microfacet_brdf
//Ashikhmin_brdf 
//Bsdf_blend
//Diagnostic_surface_shader
//Fast_sss_surface_shader
//Specular_btdf
//Material
//Kelemen_brdf
//Ao_surface_shader
//Physical_surface_shader
//Cone_edf
//Diffuse_edf

	asr::Assembly *assembly = localAssembly;
	
	// textures and colors are placed in the master assembly
	//if( pystring::find(appleName.asChar(), "file") > -1)
	//	assembly = this->masterAssembly;

	asr::Entity *entity = NULL;

	if( appleName == "Diffuse_edf")
	{
		entity = assembly->edfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->edfs().insert(asr::DiffuseEDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->edfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Cone_edf")
	{
		entity = assembly->edfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->edfs().insert(asr::ConeEDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->edfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Ao_surface_shader")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::AOSurfaceShaderFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Specular_btdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::SpecularBTDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Fast_sss_surface_shader")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::FastSubSurfaceScatteringSurfaceShaderFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Diagnostic_surface_shader")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::DiagnosticSurfaceShaderFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Bsdf_blend")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::BSDFBlendFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Ashikhmin_brdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::AshikhminBRDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Microfacet_brdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::MicrofacetBRDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Diffuse_btdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::DiffuseBTDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Constant_surface_shader")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::ConstantSurfaceShaderFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Specular_brdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::SpecularBRDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Bsdf_mix")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::BSDFMixFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Surface_shader_collection")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::SurfaceShaderCollectionFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}

	// in appleseed a material represents what in maya a shading engine means.
	// To handle the material <-> geometry connections better, I use the shading engine name here
	if( appleName == "Material")
	{
		MObject shadingEngine = getConnectedShadingEngine(sn.mobject);
		if( shadingEngine != MObject::kNullObj )
		{
			MString shadingEngineName = getObjectName(shadingEngine);
			logger.debug(MString("Defining appleseed material with shading engine name: ") + shadingEngineName);
			entity = assembly->materials().get_by_name(shadingEngineName.asChar());
			if( entity == NULL)
				assembly->materials().insert(asr::MaterialFactory::create(shadingEngineName.asChar(), asr::ParamArray()));
			entity = assembly->materials().get_by_name(shadingEngineName.asChar());
			nodeFound = true;
		}else{
			logger.warning(MString("Could not find shading engine connection of material node: ") + sn.fullName);
		}
	}
	if( appleName == "Lambertian_brdf")
	{
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->bsdfs().insert(asr::LambertianBRDFFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->bsdfs().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}
	if( appleName == "Physical_surface_shader")
	{
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		if( entity == NULL)
			assembly->surface_shaders().insert(asr::PhysicalSurfaceShaderFactory().create(sn.fullName.asChar(), asr::ParamArray()));
		entity = assembly->surface_shaders().get_by_name(sn.fullName.asChar());
		nodeFound = true;
	}

	if( nodeFound )
	{
		asr::ParamArray& pa = entity->get_parameters();
		addNodeParameters(sn, pa);
	}else{
		logger.feature(MString("Node not an Appleseed node."));
	}

}

void AppleseedRenderer::defineShadingNodes(mtap_MayaObject *obj)
{
	for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MObject shadingGroup = obj->shadingGroups[sgId];
		logger.feature(MString("Define Material from shadingGroup: ") + getObjectName(shadingGroup));
		Material material(shadingGroup);

		logger.feature(MString("Translating material for SG: ") + material.materialName);
		
		int numNodes = (int)material.surfaceShaderNet.shaderList.size();
		for( int shadingNodeId = numNodes - 1; shadingNodeId >= 0; shadingNodeId--)
		{
			ShadingNode sn = material.surfaceShaderNet.shaderList[shadingNodeId];
			logger.feature(MString("SNode Id: ") + shadingNodeId + " " + sn.fullName);
			createAppleseedShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId], getObjectAssembly(obj));
		}
	}

}