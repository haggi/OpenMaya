#include "appleseed.h"

#include "renderer/api/edf.h"
#include "renderer/modeling/edf/coneedf.h"
#include "renderer/api/texture.h"

#include <maya/MPlugArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>

#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
//#include "../mtap_common/mtap_shadingNode.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"


static Logging logger;

using namespace AppleRender;

struct ShaderAssemblyAssignment
{
	MObject shaderObject;
	MObject shadingGroup;
	std::vector<asr::Assembly *> assemblyList;
};

static std::vector<ShaderAssemblyAssignment> ShaderAssemblyAssignments;

void AppleseedRenderer::defineMaterial(MObject shadingGroup)
{
}

void AppleseedRenderer::addDefaultMaterial(asr::Assembly *assembly)
{
 //   // Create a color called "gray" and insert it into the assembly.
    static const float GrayReflectance[] = { 0.8f, 0.8f, 0.8f };
	if( this->scenePtr->colors().get_by_name("gray") == nullptr)
	{
		this->scenePtr->colors().insert(
			asr::ColorEntityFactory::create(
				"gray",
				asr::ParamArray()
					.insert("color_space", "srgb"),
				asr::ColorValueArray(3, GrayReflectance)));
	}
	if( assembly->bsdfs().get_by_name("diffuse_gray_brdf") == nullptr)
	{
		// Create a BRDF called "diffuse_gray_brdf" and insert it into the assembly.
		assembly->bsdfs().insert(
			asr::LambertianBRDFFactory().create(
				"diffuse_gray_brdf",
				asr::ParamArray()
					.insert("reflectance", "gray")));

		// Create a physical surface shader and insert it into the assembly.
		assembly->surface_shaders().insert(
			asr::PhysicalSurfaceShaderFactory().create(
				"physical_surface_shader",
				asr::ParamArray()));

		// Create a material called "gray_material" and insert it into the assembly.
		assembly->materials().insert(
			asr::GenericMaterialFactory().create(
				"gray_material",
				asr::ParamArray()
					.insert("surface_shader", "physical_surface_shader")
					.insert("bsdf", "diffuse_gray_brdf")));
	}
}

void AppleseedRenderer::defineSmokeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineAoShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineAoVoxelShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineCollectionShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineDiagnosticShader(asr::Assembly *assembly, MObject& shadingGroup)
{	
}


void AppleseedRenderer::defineFastSSSShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}


void AppleseedRenderer::defineWireframeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}


void AppleseedRenderer::defineMayaLambertShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	Logging::debug(MString("Translating mayaLambertShader: ") + shaderNode.name());

	MString colorAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("color"));
	MString incandescenceAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("incandescence"));
	MColor incandescence;
	getColor(MString("incandescence"), shaderNode, incandescence);

	MString edf_name = "";
	// if incandescence is not black we think this is a self illuminated materials, we we create an edf
	if( !isBlack(incandescence))
	{		
		edf_name = shaderNode.name() + "_incandescence_edf";
		asr::ParamArray params;
		params.insert("radiance", incandescenceAttributeDefinition.asChar());
		assembly->edfs().insert(
			asr::DiffuseEDFFactory().create(edf_name.asChar(), params));
	}

	asf::auto_release_ptr<asr::BSDF> lambertShader;
		
	asr::Entity *entity = assembly->bsdfs().get_by_name(shaderName.asChar());
	if( entity != nullptr)
		assembly->bsdfs().remove(entity);

	lambertShader = asr::LambertianBRDFFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
			.insert("reflectance", colorAttributeDefinition.asChar()));
		
	assembly->bsdfs().insert(lambertShader);

	MString surfaceShaderNode = shaderName + "phys_surf";
	entity = assembly->surface_shaders().get_by_name(surfaceShaderNode.asChar());
	if( entity != nullptr)
		assembly->surface_shaders().remove(entity);

	asf::auto_release_ptr<asr::SurfaceShader> lambertSurfaceShader;
	lambertSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
			surfaceShaderNode.asChar(),
			asr::ParamArray()
			.insert("alpha_source", "material"));

	assembly->surface_shaders().insert(lambertSurfaceShader);

	entity = assembly->materials().get_by_name(materialName.asChar());
	if( entity != nullptr)
		assembly->materials().remove(entity);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", surfaceShaderNode.asChar());
	materialParams.insert("bsdf", shaderName.asChar());
	if( edf_name.length() > 0)
		materialParams.insert("edf", edf_name.asChar());

	materialParams.insert("alpha_map", colorAttributeDefinition.asChar());

	assembly->materials().insert(
		asr::GenericMaterialFactory().create(
				materialName.asChar(),
				materialParams));

	addShaderAssemblyAssignment(surfaceShader, shadingGroup, assembly);
}


void AppleseedRenderer::defineMayaPhongShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	//MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	//MFnDependencyNode shadingGroupNode(shadingGroup);
	//MString materialName = shadingGroupNode.name();
	//MFnDependencyNode shaderNode(surfaceShader);
	//MString shaderName = shaderNode.name();
	//Logging::debug(MString("Translating mayaPhongShader: ") + shaderNode.name());

	//MString colorAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("color"));

	//asf::auto_release_ptr<asr::BSDF> phongShader;
	//phongShader = asr::SpecularBRDFFactory().create(
	//	shaderName.asChar(),
	//	asr::ParamArray()
	//		.insert("reflectance", colorAttributeDefinition.asChar()));

	//assembly->bsdfs().insert(phongShader);

	//MString surfaceShaderName = shaderName + "phys_surf";
	//asf::auto_release_ptr<asr::SurfaceShader> phongSurfaceShader;
	//phongSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
	//		surfaceShaderName.asChar(),
	//		asr::ParamArray());

	//assembly->surface_shaders().insert(phongSurfaceShader);

	//assembly->materials().insert(
	//	asr::MaterialFactory::create(
	//		materialName.asChar(),
	//		asr::ParamArray()
	//			.insert("surface_shader", surfaceShaderName.asChar())
	//			.insert("bsdf", shaderName.asChar())));
}


void AppleseedRenderer::definePhysSurfShader(asr::Assembly *assembly, MObject& shadingGroup, bool update = false)
{
	//MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	//MFnDependencyNode shadingGroupNode(shadingGroup);
	//MString materialName = shadingGroupNode.name();
	//MFnDependencyNode shaderNode(surfaceShader);
	//MString shaderName = shaderNode.name();
	//Logging::debug(MString("Translating physicalSurfaceShader: ") + shaderNode.name());

	//MString matteReflAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("matte_reflectance"));
	//MString specReflAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("specular_reflectance"));

	//float specular_reflectance_multiplier = 1.0f;
	//getFloat(MString("specular_reflectance_multiplier"), shaderNode, specular_reflectance_multiplier);

	//float shinyU = 1000.0f;
	//float shinyV = 1000.0f;
	//getFloat(MString("shininess_u"), shaderNode, shinyU);
	//getFloat(MString("shininess_v"), shaderNode, shinyV);

	//float matte_reflectance_multiplier = 1.0f;
	//getFloat(MString("matte_reflectance_multiplier"), shaderNode, matte_reflectance_multiplier);
	//
	//float fresnel_multiplier = 1.0f;
	//getFloat(MString("fresnel_multiplier"), shaderNode, fresnel_multiplier);

	//float roughness = .5f;
	//getFloat(MString("roughness"), shaderNode, roughness);

	//int shaderType = 0;
	//getInt(MString("bsdf"), shaderNode, shaderType);		
	//float transmittanceMultiplier = 1.0f;
	//float from_ior = 1.0f;
	//float to_ior = 1.0f;
	//MColor transmittance(1.0f,1.0f,1.0f);
	//bool doubleSided = false;

	//asf::auto_release_ptr<asr::BSDF> bsdf;
	//asf::auto_release_ptr<asr::BSDF> bsdfBack;

	//switch(shaderType)
	//{
	//case 0: // lambert
	//	if( update )
	//	{
	//		assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//				.insert("reflectance", matteReflAttributeDefinition.asChar())
	//				.insert("reflectance_multiplier", matte_reflectance_multiplier);
	//		return;
	//	}else{
	//		bsdf = asr::LambertianBRDFFactory().create(
	//			shaderName.asChar(),
	//			asr::ParamArray()
	//				.insert("reflectance", matteReflAttributeDefinition.asChar())
	//				.insert("reflectance_multiplier", matte_reflectance_multiplier)
	//				);
	//	}
	//	break;
	//case 1: // Ashikhmin
	//	if( update )
	//	{
	//		assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//				.insert("diffuse_reflectance", matteReflAttributeDefinition.asChar())
	//				.insert("diffuse_reflectance_multiplier", matte_reflectance_multiplier)
	//				.insert("glossy_reflectance", specReflAttributeDefinition.asChar())
	//				.insert("glossy_reflectance_multiplier", specular_reflectance_multiplier)
	//				.insert("shininess_u", shinyU)
	//				.insert("shininess_v", shinyV)
	//				.insert("fresnel_multiplier", fresnel_multiplier)
	//				;
	//		return;
	//	}else{
	//		bsdf = asr::AshikhminBRDFFactory().create(
	//			shaderName.asChar(),
	//			asr::ParamArray()
	//				.insert("diffuse_reflectance", matteReflAttributeDefinition.asChar())
	//				.insert("diffuse_reflectance_multiplier", matte_reflectance_multiplier)
	//				.insert("glossy_reflectance", specReflAttributeDefinition.asChar())
	//				.insert("glossy_reflectance_multiplier", specular_reflectance_multiplier)
	//				.insert("shininess_u", shinyU)
	//				.insert("shininess_v", shinyV)
	//				.insert("fresnel_multiplier", fresnel_multiplier)
	//				);
	//	}
	//	break;
	//case 2: // Kelemen
	//	if( update )
	//	{
	//		assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//			.insert("matte_reflectance", matteReflAttributeDefinition.asChar())
	//			.insert("matte_reflectance_multiplier", matte_reflectance_multiplier)				
	//			.insert("specular_reflectance", specReflAttributeDefinition.asChar())
	//			.insert("specular_reflectance_multiplier", specular_reflectance_multiplier)
	//			.insert("roughness", roughness)
	//			;
	//		return;
	//	}else{
	//		bsdf = asr::KelemenBRDFFactory().create(
	//		shaderName.asChar(),
	//		asr::ParamArray()
	//			.insert("matte_reflectance", matteReflAttributeDefinition.asChar())
	//			.insert("matte_reflectance_multiplier", matte_reflectance_multiplier)				
	//			.insert("specular_reflectance", specReflAttributeDefinition.asChar())
	//			.insert("specular_reflectance_multiplier", specular_reflectance_multiplier)
	//			.insert("roughness", roughness)
	//			);
	//	}
	//	break;
	//case 3: // Specular		
	//	{
	//		MString transmittanceAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("transmittance"));
	//		getFloat(MString("transmittance_multiplier"), shaderNode, transmittanceMultiplier);
	//		getFloat(MString("from_ior"), shaderNode, from_ior);
	//		getFloat(MString("to_ior"), shaderNode, to_ior);
	//		// if transmittance > 0 then create a specular_btdf, else a specular brdf
	//		bool isTransparent = (transmittance.r + transmittance.g + transmittance.b) > 0.0f;
	//		doubleSided = true;
	//		if( isTransparent )
	//		{
	//			if( update )
	//			{
	//				assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//						.insert("reflectance",  specReflAttributeDefinition.asChar())
	//						.insert("reflectance_multiplier",specular_reflectance_multiplier)
	//						.insert("from_ior", (MString("") + from_ior).asChar())
	//						.insert("to_ior", (MString("") + to_ior).asChar())
	//						.insert("transmittance", transmittanceAttributeDefinition.asChar())
	//						.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
	//					;
	//				assembly->bsdfs().get_by_name((shaderName + "_back").asChar())->get_parameters()			
	//						.insert("reflectance",  specReflAttributeDefinition.asChar())
	//						.insert("reflectance_multiplier",specular_reflectance_multiplier)
	//						.insert("from_ior", (MString("") + to_ior).asChar())
	//						.insert("to_ior", (MString("") + from_ior).asChar())
	//						.insert("transmittance", transmittanceAttributeDefinition.asChar())
	//						.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
	//					;
	//				return;
	//			}else{
	//				bsdf = asr::SpecularBTDFFactory().create(
	//					(shaderName).asChar(),
	//					asr::ParamArray()
	//						.insert("reflectance",  specReflAttributeDefinition.asChar())
	//						.insert("reflectance_multiplier",specular_reflectance_multiplier)
	//						.insert("from_ior", (MString("") + from_ior).asChar())
	//						.insert("to_ior", (MString("") + to_ior).asChar())
	//						.insert("transmittance", transmittanceAttributeDefinition.asChar())
	//						.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
	//						);
	//				bsdfBack = asr::SpecularBTDFFactory().create(
	//					(shaderName + "_back").asChar(),
	//					asr::ParamArray()
	//						.insert("reflectance", specReflAttributeDefinition.asChar())
	//						.insert("reflectance_multiplier", specular_reflectance_multiplier)
	//						.insert("from_ior", (MString("") + to_ior).asChar())
	//						.insert("to_ior", (MString("") + from_ior).asChar())
	//						.insert("transmittance", transmittanceAttributeDefinition.asChar())
	//						.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
	//						);
	//			}
	//		}else{
	//			if( update )
	//			{
	//				assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//						.insert("reflectance", specReflAttributeDefinition.asChar())
	//					;
	//				return;
	//			}else{
	//				bsdf = asr::SpecularBRDFFactory().create(
	//					shaderName.asChar(),
	//					asr::ParamArray()
	//						.insert("reflectance", specReflAttributeDefinition.asChar())
	//						);
	//			}
	//		}
	//	}
	//	break;
	//case 4: // Phong
	//	if( update )
	//	{
	//		assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//				.insert("reflectance", specReflAttributeDefinition.asChar());
	//			;
	//		return;
	//	}else{
	//		bsdf = asr::SpecularBRDFFactory().create(
	//			shaderName.asChar(),
	//			asr::ParamArray()
	//				.insert("reflectance", specReflAttributeDefinition.asChar()));
	//	}
	//	break;
	//case 5: // microfacet
	//	{
	//		float mdf_param = 1.0f;
	//		getFloat(MString("mdf_parameter"), shaderNode, mdf_param);
	//		int id = 0;
	//		MString mdf("blinn");
	//		getEnum(MString("mdf"), shaderNode, id, mdf);
	//		mdf = pystring::lower(mdf.asChar()).c_str();
	//		if( update )
	//		{
	//			assembly->bsdfs().get_by_name(shaderName.asChar())->get_parameters()			
	//					.insert("mdf", mdf.asChar())
	//					.insert("mdf_parameter", mdf_param)
	//					.insert("reflectance", specReflAttributeDefinition.asChar())
	//					.insert("reflectance_multiplier", specular_reflectance_multiplier)
	//					.insert("fresnel_multiplier",fresnel_multiplier)
	//				;
	//			return;
	//		}else{
	//			bsdf = asr::MicrofacetBRDFFactory().create(
	//				shaderName.asChar(),
	//				asr::ParamArray()
	//					.insert("mdf", mdf.asChar())
	//					.insert("mdf_parameter", mdf_param)
	//					.insert("reflectance", specReflAttributeDefinition.asChar())
	//					.insert("reflectance_multiplier", specular_reflectance_multiplier)
	//					.insert("fresnel_multiplier",fresnel_multiplier)
	//					);
	//		}
	//	}
	//	break;
	//}

	//assembly->bsdfs().insert(bsdf);
	//	
	//// we have a front/back side shading
	//if( doubleSided )
	//{
	//	assembly->bsdfs().insert(bsdfBack);
	//}

	//// EDF
	//bool emitLight = false;
	//getBool(MString("emitLight"), shaderNode, emitLight);
	//MString edfName = "";
	//if( emitLight )
	//{
	//	MString exitanceAttributeDefinition = this->defineColorAttributeWithTexture(shaderNode, MString("transmittance"));
	//	edfName = shaderNode.name() + "_exitance_edf";
	//	asr::ParamArray params;
	//	params.insert("radiance", exitanceAttributeDefinition.asChar());
	//	edfName = shaderNode.name() + "_edf";

	//	MString edfType = "diffuse";
	//	int id = 0;
	//	getEnum("emitLightType", shaderNode, id, edfType);
	//	if( edfType == "cone")
	//	{
	//		float angle = 90.0f;
	//		getFloat("angle", shaderNode, angle);
	//		params.insert("angle", angle);
	//		assembly->edfs().insert(
	//			asr::ConeEDFFactory().create(edfName.asChar(), params));
	//	}else{
	//		assembly->edfs().insert(
	//			asr::DiffuseEDFFactory().create(edfName.asChar(), params));
	//	}

	//}		

	//MString surfaceShaderName = shaderName + "phys_surf";
	//asf::auto_release_ptr<asr::SurfaceShader> physSurfaceShader;

	//asr::ParamArray physSurfaceParams;
	//float tranclucency = 0.0f;
	//getFloat(MString("translucency"), shaderNode, tranclucency);
	//if( tranclucency > 0.0f)
	//	physSurfaceParams.insert("translucency", tranclucency);
	//{
	//	bool useAerialPerspective = false;
	//	getBool("useAerialPerspective", shaderNode, useAerialPerspective);
	//	float aerial_persp_distance = 1000.0f;
	//	getFloat("aerial_persp_distance", shaderNode, aerial_persp_distance); 
	//	float aerial_persp_intensity = 0.001f;
	//	getFloat("aerial_persp_intensity", shaderNode, aerial_persp_intensity); 
	//	MString aerial_persp_mode = "none";
	//	int id = 0;
	//	getEnum("aerial_persp_mode", shaderNode, id, aerial_persp_mode); 
	//	if(useAerialPerspective)
	//	{
	//		physSurfaceParams.insert("aerial_persp_distance", aerial_persp_distance);
	//		physSurfaceParams.insert("aerial_persp_intensity", aerial_persp_intensity);
	//		physSurfaceParams.insert("aerial_persp_mode", aerial_persp_mode.asChar());
	//	}
	//}
	//float alpha_multiplier = 1.0f;
	//getFloat("alpha_multiplier", shaderNode, alpha_multiplier); 
	//physSurfaceParams.insert("alpha_multiplier", alpha_multiplier);

	//float color_multiplier = 1.0f;
	//getFloat("color_multiplier", shaderNode, color_multiplier); 
	//physSurfaceParams.insert("color_multiplier", color_multiplier);

	//int front_lighting_samples = 1;
	//getInt("front_lighting_samples", shaderNode, front_lighting_samples);
	//physSurfaceParams.insert("front_lighting_samples", front_lighting_samples);

	//int back_lighting_samples = 1;
	//getInt("back_lighting_samples", shaderNode, back_lighting_samples);
	//physSurfaceParams.insert("back_lighting_samples", back_lighting_samples);

	//physSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
	//		surfaceShaderName.asChar(),
	//		physSurfaceParams);
	//assembly->surface_shaders().insert(physSurfaceShader);

	//if(this->defineAOVShaders(assembly, surfaceShaderName))
	//{
	//	Logging::debug(MString("Successfully created an aov shader called: ") + surfaceShaderName);
	//}

	//asr::ParamArray materialParams;
	//materialParams.insert("surface_shader", surfaceShaderName.asChar());
	//materialParams.insert("bsdf", shaderName.asChar());
	//if( edfName != "")
	//	materialParams.insert("edf", edfName.asChar());

	//defineBumpMap(materialParams, surfaceShader);

	//assembly->materials().insert(
	//		asr::MaterialFactory::create(
	//		materialName.asChar(),
	//		materialParams));

	//// this is not a good way to implement the double sided shading, have to find a better one
	//if( doubleSided )
	//{
	//	asr::ParamArray materialParams;
	//	materialParams.insert("surface_shader", surfaceShaderName.asChar());
	//	materialParams.insert("bsdf", (shaderName + "_back").asChar());
	//	if( edfName != "")
	//		materialParams.insert("edf", edfName.asChar());
	//	assembly->materials().insert(
	//			asr::MaterialFactory::create(
	//			(materialName + "_back").asChar(),
	//			materialParams));
	//}

	//addShaderAssemblyAssignment(surfaceShader, shadingGroup, assembly);
}


void AppleseedRenderer::defineBumpMap(asr::ParamArray& materialParams, MObject& surfaceShader)
{
	MFnDependencyNode shaderNode(surfaceShader);

	MObject bumpObj = getOtherSideNode(MString("normalCamera"), surfaceShader);
	if( bumpObj == MObject::kNullObj)
	{
		Logging::debug(MString("No normalCamera input found."));
		return;
	}
	if( !bumpObj.hasFn(MFn::kBump))
	{
		Logging::debug(MString("Found cameraNormal input for shader: ") + shaderNode.name() + " : " + getObjectName(bumpObj) + " but is NO bump2d node");
		return;
	}
	Logging::debug(MString("Found bump input for shader: ") + shaderNode.name() + " : " + getObjectName(bumpObj));
	MFnDependencyNode bumpNode(bumpObj);
	MObject fileObj = getOtherSideNode(MString("bumpValue"), bumpObj);
	if( fileObj == MObject::kNullObj)
	{
		Logging::debug(MString("No bump file node found."));
		return;
	}	

	MString textureAttributeDefinition = this->defineScalarAttributeWithTexture(bumpNode, MString("bumpValue"));
	if( textureAttributeDefinition == "")
	{
		Logging::debug(MString("BumpValue texture definition == empty --> invalid."));
		return;
	}
	int bumpInterp = 0;
	getEnum(MString("bumpInterp"), bumpNode, bumpInterp);
	float bumpDepth = 1.0f;
	getFloat(MString("bumpDepth"), bumpNode, bumpDepth);

	materialParams.insert("bump_amplitude", bumpDepth);
	materialParams.insert("displacement_map", textureAttributeDefinition.asChar());
	if( bumpInterp == 0) // bump
		materialParams.insert("displacement_method", "bump");
	if( bumpInterp > 0) // normal
		materialParams.insert("displacement_method", "normal");
}

bool AppleseedRenderer::defineAOVShaders(asr::Assembly *assembly, MString& physSurfaceShaderName)
{
	//if( assembly->surface_shaders().get_by_name("aovs") != nullptr)
	//{
	//	Logging::debug("AOV collection shader already defined.");
	//	physSurfaceShaderName = "aovs";
	//	return true;
	//}
	//MStatus stat;
	//MPlugArray plugArray;
	//MObject globalsObj = objectFromName(MString("appleseedGlobals"));
	//MFnDependencyNode depFn(globalsObj);
	//MPlug AOVs = depFn.findPlug(MString("AOVs"), &stat);
	//if( !stat )
	//{
	//	Logging::debug("Unable to find AOVs plug.");
	//	return false;
	//}
	//if( AOVs.numElements() == 0)
	//{
	//	Logging::debug("No elements in AOVs.");
	//	return false;
	//}

	//asr::ParamArray params;
	//params.insert((MString("surface_shader1")).asChar(), physSurfaceShaderName);

 //   for(unsigned int i = 0; i < AOVs.numElements (); i++)
 //   {
	//	MPlug elementPlug = AOVs[i];
	//	elementPlug.connectedTo(plugArray, true, false, &stat);
	//	if( plugArray.length() == 0)
	//	{
	//		Logging::debug(MString("No aovs connected to ") + elementPlug.name());
	//		continue;
	//	}
	//	MObject shaderNode = plugArray[0].node();
	//	MString nodeName = getObjectName(shaderNode);
	//	Logging::debug(elementPlug.name() + " connected to: " + nodeName);
	//	MString shaderName;
	//	MFn::Type shaderType = shaderNode.apiType();
	//	MFnDependencyNode shaderFn(shaderNode);
	//	int type_id = shaderFn.typeId().id();
	//	if( type_id ==  SMOKE_SHADER)
	//	{
	//		this->defineSmokeShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  DIAGNOSTIC_SHADER)
	//	{
	//		this->defineDiagnosticShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  CONST_SHADER)
	//	{
	//		this->defineConstantShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  FASTSSS_SHADER)
	//	{
	//		this->defineFastSSSShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  AOVOXEL_SHADER)
	//	{		
	//		this->defineAoVoxelShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  AO_SHADER)
	//	{
	//		this->defineAoShader(assembly, shaderNode, shaderName);
	//	}
	//	if( type_id ==  PHYSICAL_SURFACE_SHADER)
	//	{
	//		this->definePhysSurfShader(assembly, shaderNode, shaderName);
	//	}
	//	if( shaderType == MFn::kLambert)
	//	{
	//		this->defineMayaLambertShader(assembly, shaderNode, shaderName);
	//	}
	//	if( shaderType == MFn::kPhong)
	//	{
	//		this->defineMayaPhongShader(assembly, shaderNode, shaderName);
	//	}	
	//	Logging::debug(MString("Inserting aov shader into array: ") + shaderName);
	//	params.insert((MString("surface_shader")+(i+2)).asChar(), shaderName);
 //   }
	//if( params.size() == 0)
	//{
	//	Logging::debug("No connections found in AOVs.");
	//	return false;
	//}
	//
	//this->hasAOVs = true;

	//MString surfaceShaderNode = "aovs";
	//asf::auto_release_ptr<asr::SurfaceShader> physSurfaceShader;
	//physSurfaceShader = asr::SurfaceShaderCollectionFactory().create(
	//		surfaceShaderNode.asChar(),
	//		params);
	//assembly->surface_shaders().insert(physSurfaceShader);
	//physSurfaceShaderName = surfaceShaderNode;
	return true;
}

// translate shaders here, will be seperated later if we have a real shading language
void AppleseedRenderer::defineObjectMaterial(mtap_RenderGlobals *renderGlobals, std::shared_ptr<MayaObject> obj, asf::StringArray& materialNames)
{
	asr::Assembly *assembly = getObjectAssembly(obj);

	// get per face shadingGroups
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups, true);

	MObject shadingGroup;

	defineShadingNodes(obj);

	for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		shadingGroup = obj->shadingGroups[sgId];

		MObject surfaceShaderNode = getOtherSideNode(MString("surfaceShader"), shadingGroup);
		if( surfaceShaderNode == MObject::kNullObj)
			continue;

		MFnDependencyNode shadingGroupNode(shadingGroup);
		MString materialName = shadingGroupNode.name();


		// here I reuse the shader if it already exists in the assembly
		if( assembly->materials().get_by_name(materialName.asChar()) != nullptr)
		{
			materialNames.push_back(materialName.asChar());
			continue;		
		}else{
			Logging::debug(MString("----MaterialName: ") + materialName + " does not exist in assembly: " + assembly->get_name());
		}

		if (surfaceShaderNode.hasFn(MFn::kLambert))
		{
			this->defineMayaLambertShader(assembly, shadingGroup);
			materialNames.push_back(materialName.asChar());
			continue;
		}

		// if we have double sided shading, we have a xx_back material 
		if( assembly->materials().get_by_name((materialName + "_back").asChar()) != nullptr)
		{
			materialNames.push_back((materialName + "_back").asChar());
		}
	}
}

void AppleseedRenderer::updateShader( MObject shadingNode)
{
	size_t assignmentId = 0;
	bool found = false;
	for( size_t i = 0; i < ShaderAssemblyAssignments.size(); i++)
	{
		if( ShaderAssemblyAssignments[i].shaderObject == shadingNode )
		{
			assignmentId = i;
			found = true;
			break;
		}
	}
	if(found)
	{
		ShaderAssemblyAssignment saa = ShaderAssemblyAssignments[assignmentId];
		Logging::debug(MString("Found shader assingments for shading node: ") + getObjectName(shadingNode));		
		for( size_t assId = 0; assId < saa.assemblyList.size(); assId++)
			definePhysSurfShader(saa.assemblyList[assId], saa.shadingGroup, true);
	}
}

void AppleseedRenderer::clearShaderAssemblyAssignments()
{
	ShaderAssemblyAssignments.clear();
}

void AppleseedRenderer::addShaderAssemblyAssignment(MObject shadingNode, MObject shadingGroup, asr::Assembly *assembly)
{
	size_t assignmentId = 0;
	bool found = false;
	for( size_t i = 0; i < ShaderAssemblyAssignments.size(); i++)
	{
		if( ShaderAssemblyAssignments[i].shaderObject == shadingNode )
		{
			assignmentId = i;
			found = true;
			break;
		}
	}

	if( found )
	{
		ShaderAssemblyAssignments[assignmentId].assemblyList.push_back(assembly);
	}else{
		ShaderAssemblyAssignment assignment;
		assignment.shaderObject = shadingNode;
		assignment.shadingGroup = shadingGroup;
		assignment.assemblyList.push_back(assembly);
		ShaderAssemblyAssignments.push_back(assignment);
	}
}