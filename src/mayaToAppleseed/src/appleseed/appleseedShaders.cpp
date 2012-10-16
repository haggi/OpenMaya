#include "appleseed.h"

#include "renderer/api/edf.h"
#include "renderer/api/texture.h"
#include "renderer/modeling/surfaceshader/fastsubsurfacescatteringsurfaceshader.h"

#include <maya/MPlugArray.h>

#include "shadingtools/material.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"


static Logging logger;

using namespace AppleRender;

void AppleseedRenderer::addDefaultMaterial(asr::Assembly *assembly)
{
    // Create a color called "gray" and insert it into the assembly.
    static const float GrayReflectance[] = { 0.8f, 0.8f, 0.8f };
	if( this->scene->colors().get_by_name("gray") == NULL)
	{
		this->scene->colors().insert(
			asr::ColorEntityFactory::create(
				"gray",
				asr::ParamArray()
					.insert("color_space", "srgb"),
				asr::ColorValueArray(3, GrayReflectance)));
	}
	if( assembly->bsdfs().get_by_name("diffuse_gray_brdf") == NULL)
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
			asr::MaterialFactory::create(
				"gray_material",
				asr::ParamArray()
					.insert("surface_shader", "physical_surface_shader")
					.insert("bsdf", "diffuse_gray_brdf")));
	}
}

//
// if a connection exists at the other side of the attribute name then:
//		- read the texture fileName
//		- if it is not a exr file convert it(?) 
//		- create a texture definition and
//		- put the textureFileDefintion string into textureDefinition string
//	the textureDefinition contains the current color definition, if no texture is found, the string remains unchanged
//  otherwise it will receive the texture definition and will used instead of the color
//

void AppleseedRenderer::defineTexture(MFnDependencyNode& shader, MString& attributeName, MString& textureDefinition)
{
	MStatus stat;
	// check attribute for connections
	MPlug plug = shader.findPlug(attributeName, &stat);	if( stat != MStatus::kSuccess)return;
	if( !plug.isConnected() )
		return;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return;
	if( plugArray.length() == 0)
		return;
	MPlug otherSidePlug = plugArray[0];
	MObject inputNode = otherSidePlug.node();
	
	// no file texture connected to the attribute
	if( !inputNode.hasFn(MFn::kFileTexture))
		return;

	MFnDependencyNode fileTextureNode(inputNode, &stat);
	MString textureName = fileTextureNode.name() + "_texture";

	logger.info(MString("Found fileTextureNode: ") + fileTextureNode.name());
	MString fileTextureName = "";
	getString(MString("fileTextureName"), fileTextureNode, fileTextureName);
	logger.info(MString("Found filename: ") + fileTextureName);
	if( !pystring::endswith(fileTextureName.asChar(), ".exr"))
	{
		logger.warning(MString("FileTextureName does not have an .exr extension. Other filetypes are not yet supported, sorry."));
		return;
	}

	MString textureInstanceName = textureName + "_texInst";
	asr::Texture *texture = this->scene->textures().get_by_name(textureName.asChar());
	if( texture != NULL)
		this->scene->textures().remove(texture);

	asr::TextureInstance *textureInstance = this->scene->texture_instances().get_by_name(textureInstanceName.asChar());
	if( textureInstance != NULL)
		this->scene->texture_instances().remove(textureInstance);

	MString colorProfileName;
	int profileId = 0;
	getEnum(MString("colorProfile"), fileTextureNode, profileId);
	logger.debug(MString("Color profile from fileNode: ") + profileId);
	asr::ParamArray params;
	logger.debug(MString("Now inserting file name: ") + fileTextureName);
	params.insert("filename", fileTextureName.asChar());      // OpenEXR only for now. The param is called filename but it can be a path
    params.insert("color_space", "srgb");					  // the color space the texture is in, often it's sRGB, have to change this

    asf::auto_release_ptr<asr::Texture> textureElement(
        asr::DiskTexture2dFactory().create(
	    textureName.asChar(),
            params,
            this->project->get_search_paths()));    // the project holds a set of search paths to find textures and other assets
	this->scene->textures().insert(textureElement);

	bool alphaIsLuminance = false;
	getBool(MString("alphaIsLuminance"), fileTextureNode, alphaIsLuminance);
	asr::ParamArray tInstParams;
	tInstParams.insert("addressing_mode", "clamp");
	tInstParams.insert("filtering_mode", "bilinear");
	if( alphaIsLuminance )
		tInstParams.insert("alpha_mode", "luminance");

	asf::auto_release_ptr<asr::TextureInstance> tinst = asr::TextureInstanceFactory().create(
	   textureInstanceName.asChar(),
	   tInstParams,
	   textureName.asChar());
	
	this->scene->texture_instances().insert(tinst);

	textureDefinition = textureInstanceName;

}

#define isBlack(x) ((x.r + x.g + x.b) <= 0.0f)

void AppleseedRenderer::defineSmokeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	logger.debug(MString("SmokeShader: ") + getObjectName(shadingGroup) + " not yet implmented.");
}

void AppleseedRenderer::defineAoShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating aoShader: ") + shaderNode.name());

	std::vector<MString> samplingTypes;
	samplingTypes.push_back("uniform");
	samplingTypes.push_back("cosine");
	int samples = 16;
	float max_dist = 1.0f;
	int samplingMethod = 0;
	getInt(MString("samples"), shaderNode, samples);
	getFloat(MString("maxDistance"), shaderNode, max_dist);
	getEnum(MString("samplingMethod"), shaderNode, samplingMethod);

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
	asrSurfaceShader = asr::AOSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
		.insert("sampling_method", samplingTypes[samplingMethod].asChar())
		.insert("samples", (MString("") + samples).asChar())
			.insert("max_distance", (MString("") + max_dist).asChar()));

	assembly->surface_shaders().insert(asrSurfaceShader);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", shaderName.asChar());

	assembly->materials().insert(
			asr::MaterialFactory::create(
			materialName.asChar(),
			materialParams));
}

void AppleseedRenderer::defineAoVoxelShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating aoVoxelShader: ") + shaderNode.name());

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
	asrSurfaceShader = asr::VoxelAOSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray());
	assembly->surface_shaders().insert(asrSurfaceShader);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", shaderName.asChar());

	assembly->materials().insert(
			asr::MaterialFactory::create(
			materialName.asChar(),
			materialParams));

}

void AppleseedRenderer::defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating constantShader: ") + shaderNode.name());
}

void AppleseedRenderer::defineDiagnosticShader(asr::Assembly *assembly, MObject& shadingGroup)
{	
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating diagnosticShader: ") + shaderNode.name());
	
	int mode = 0;
	getEnum(MString("mode"), shaderNode, mode);
	std::vector<MString> diagnosticModeNames;
	diagnosticModeNames.push_back("ambient_occlusion");
	diagnosticModeNames.push_back("assembly_instances");
	diagnosticModeNames.push_back("barycentric");
	diagnosticModeNames.push_back("bitangent");
	diagnosticModeNames.push_back("coverage");
	diagnosticModeNames.push_back("depth");
	diagnosticModeNames.push_back("geometric_normal");
	diagnosticModeNames.push_back("materials");
	diagnosticModeNames.push_back("object_instances");
	diagnosticModeNames.push_back("original_shading_normal");
	diagnosticModeNames.push_back("regions");
	diagnosticModeNames.push_back("shading_normal");
	diagnosticModeNames.push_back("sides");
	diagnosticModeNames.push_back("tangent");
	diagnosticModeNames.push_back("triangles");
	diagnosticModeNames.push_back("uv");
	diagnosticModeNames.push_back("wireframe");

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader = asr::DiagnosticSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
		.insert("mode", diagnosticModeNames[mode].asChar()));
	assembly->surface_shaders().insert(asrSurfaceShader);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", shaderName.asChar());

	assembly->materials().insert(
			asr::MaterialFactory::create(
			materialName.asChar(),
			materialParams));
}

void AppleseedRenderer::defineFastSSSShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating diagnosticShader: ") + shaderNode.name());

		logger.debug(MString("Translating fastSSSShader: ") + shaderNode.name());
		int light_samples = 1, occlusion_samples = 1;
		float scale = 1.0f, ambient_sss, view_dep_sss, diffuse, power, distortion;
		MColor albedo;
		getInt(MString("light_samples"), shaderNode, light_samples);
		getInt(MString("occlusion_samples"), shaderNode, occlusion_samples);
		getFloat(MString("scale"), shaderNode, scale);
		getFloat(MString("ambient_sss"), shaderNode, ambient_sss);
		getFloat(MString("view_dep_sss"), shaderNode, view_dep_sss);
		getFloat(MString("diffuse"), shaderNode, diffuse);
		getFloat(MString("power"), shaderNode, power);
		getFloat(MString("distortion"), shaderNode, distortion);
		getColor(MString("albedo"), shaderNode, albedo);
		MString albedoName = shaderNode.name() + "_albedo";
		this->defineColor(albedoName, albedo, assembly);

		asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
		asrSurfaceShader = asr::FastSubSurfaceScatteringSurfaceShaderFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
			.insert("light_samples", (MString("") + light_samples).asChar())
			.insert("occlusion_samples", (MString("") + occlusion_samples).asChar())
			.insert("scale", (MString("") + scale).asChar())
			.insert("ambient_sss", (MString("") + ambient_sss).asChar())
			.insert("view_dep_sss", (MString("") + view_dep_sss).asChar())
			.insert("diffuse", (MString("") + diffuse).asChar())
			.insert("power", (MString("") + power).asChar())
			.insert("distortion", (MString("") + distortion).asChar())
			.insert("albedo", albedoName.asChar())
			);
		assembly->surface_shaders().insert(asrSurfaceShader);

		asr::ParamArray materialParams;
		materialParams.insert("surface_shader", shaderName.asChar());

		assembly->materials().insert(
				asr::MaterialFactory::create(
				materialName.asChar(),
				materialParams));

}

void AppleseedRenderer::defineWireframeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating wireframeShader: ") + shaderNode.name());
}

void AppleseedRenderer::defineMayaLambertShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating mayaLambertShader: ") + shaderNode.name());

	MColor color(1.0f,1.0f,1.0f);
	getColor(MString("color"), shaderNode, color);
	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, color, assembly);
	MString origCName = matteRefName;
	this->defineTexture(shaderNode, MString("color"), matteRefName);
	bool hasMap = false;
	if(origCName != matteRefName)
		hasMap = true;

	MColor incandescence;
	getColor(MString("incandescence"), shaderNode, incandescence);
	MString incandName = shaderNode.name() + "_incandescence";
	this->defineColor(incandName, incandescence, assembly);
	this->defineTexture(shaderNode, MString("color"), incandName);

	MString edf_name = "";
	if( !isBlack(incandescence))
	{		
		// Create a new EDF.
		edf_name = shaderNode.name() + "_incandescence_edf";
		asr::ParamArray params;
		params.insert("exitance", incandName.asChar());
		assembly->edfs().insert(
			asr::DiffuseEDFFactory().create(edf_name.asChar(), params));
	}

	asf::auto_release_ptr<asr::BSDF> lambertShader;
		
	asr::Entity *entity = assembly->bsdfs().get_by_name(shaderName.asChar());
	if( entity != NULL)
		assembly->bsdfs().remove(entity);

	lambertShader = asr::LambertianBRDFFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
			.insert("reflectance", matteRefName.asChar()));
		
	assembly->bsdfs().insert(lambertShader);

	MString surfaceShaderNode = shaderName + "phys_surf";
	entity = assembly->surface_shaders().get_by_name(surfaceShaderNode.asChar());
	if( entity != NULL)
		assembly->surface_shaders().remove(entity);

	asf::auto_release_ptr<asr::SurfaceShader> lambertSurfaceShader;
	lambertSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
			surfaceShaderNode.asChar(),
			asr::ParamArray()
			.insert("alpha_source", "material"));

	assembly->surface_shaders().insert(lambertSurfaceShader);

	entity = assembly->materials().get_by_name(materialName.asChar());
	if( entity != NULL)
		assembly->materials().remove(entity);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", surfaceShaderNode.asChar());
	materialParams.insert("bsdf", shaderName.asChar());
	if( edf_name.length() > 0)
		materialParams.insert("edf", edf_name.asChar());
	if(hasMap)
		materialParams.insert("alpha_map", matteRefName.asChar());

	assembly->materials().insert(
		asr::MaterialFactory::create(
				materialName.asChar(),
				materialParams));
}

void AppleseedRenderer::defineMayaPhongShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating mayaPhongShader: ") + shaderNode.name());

	MColor color(1.0f,1.0f,1.0f);
	getColor(MString("color"), shaderNode, color);

	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, color, assembly);

	asf::auto_release_ptr<asr::BSDF> phongShader;
	phongShader = asr::SpecularBRDFFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
			.insert("reflectance", matteRefName.asChar()));

	assembly->bsdfs().insert(phongShader);

	MString surfaceShaderName = shaderName + "phys_surf";
	asf::auto_release_ptr<asr::SurfaceShader> phongSurfaceShader;
	phongSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
			surfaceShaderName.asChar(),
			asr::ParamArray());

	assembly->surface_shaders().insert(phongSurfaceShader);

	assembly->materials().insert(
		asr::MaterialFactory::create(
			materialName.asChar(),
			asr::ParamArray()
				.insert("surface_shader", surfaceShaderName.asChar())
				.insert("bsdf", shaderName.asChar())));
}

void AppleseedRenderer::definePhysSurfShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating physicalSurfaceShader: ") + shaderNode.name());

	MColor matteReflectance(1.0f,1.0f,1.0f);
	getColor(MString("matte_reflectance"), shaderNode, matteReflectance);
	float matteMultiplier = 1.0f;
	getFloat(MString("matte_reflectance_multiplier"), shaderNode, matteMultiplier);
	MColor specReflectance(1.0f,1.0f,1.0f);
	getColor(MString("specular_reflectance"), shaderNode, specReflectance);
	float specMultiplier = 1.0f;
	getFloat(MString("specular_reflectance_multiplier"), shaderNode, specMultiplier);
	float shinyU = 1000.0f;
	float shinyV = 1000.0f;
	getFloat(MString("shininess_u"), shaderNode, shinyU);
	getFloat(MString("shininess_v"), shaderNode, shinyV);
	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, matteReflectance, assembly);
	this->defineTexture(shaderNode, MString("matte_reflectance"), matteRefName);
	MString specRefName = shaderNode.name() + "_specRefl";
	this->defineColor(specRefName, specReflectance, assembly);
	this->defineTexture(shaderNode, MString("specular_reflectance"), specRefName);
	int shaderType = 0;
	getInt(MString("bsdf"), shaderNode, shaderType);		
	float transmittanceMultiplier = 1.0f;
	float from_ior = 1.0f;
	float to_ior = 1.0f;
	MColor transmittance(1.0f,1.0f,1.0f);
	MString transmittanceName = shaderNode.name() + "_transmittance";
	bool doubleSided = false;

	asf::auto_release_ptr<asr::BSDF> bsdf;
	asf::auto_release_ptr<asr::BSDF> bsdfBack;

	switch(shaderType)
	{
	case 0: // lambert
		break;
	case 1: // Ashikhmin
		bsdf = asr::AshikhminBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("diffuse_reflectance", matteRefName.asChar())
				.insert("glossy_reflectance", specRefName.asChar())
				.insert("shininess_u", (MString("") + shinyU).asChar())
				.insert("shininess_v", (MString("") + shinyV).asChar())
				);
		break;
	case 2: // Kelemen
		break;
	case 3: // Specular
			
		getColor(MString("transmittance"), shaderNode, transmittance);
		defineColor(transmittanceName, transmittance, assembly);
		defineTexture(shaderNode, MString("transmittance"), transmittanceName);
		getFloat(MString("transmittance_multiplier"), shaderNode, transmittanceMultiplier);
			
		getFloat(MString("from_ior"), shaderNode, from_ior);
			
		getFloat(MString("to_ior"), shaderNode, to_ior);
		// if transmittance > 0 then create a specular_btdf, else a specular brdf
		if( (transmittance.r + transmittance.g + transmittance.b) > 0.0f)
		{
			doubleSided = true;
			bsdf = asr::SpecularBTDFFactory().create(
				(shaderName).asChar(),
				asr::ParamArray()
					.insert("reflectance", matteRefName.asChar())
					.insert("reflectance_multiplier", (MString("") + matteMultiplier).asChar())
					.insert("from_ior", (MString("") + from_ior).asChar())
					.insert("to_ior", (MString("") + to_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
			bsdfBack = asr::SpecularBTDFFactory().create(
				(shaderName + "_back").asChar(),
				asr::ParamArray()
					.insert("reflectance", matteRefName.asChar())
					.insert("reflectance_multiplier", (MString("") + matteMultiplier).asChar())
					.insert("from_ior", (MString("") + to_ior).asChar())
					.insert("to_ior", (MString("") + from_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
		}else{
			bsdf = asr::SpecularBRDFFactory().create(
				shaderName.asChar(),
				asr::ParamArray()
					.insert("reflectance", (matteRefName).asChar())
					);
		}
		break;
	case 4: // Phong
		bsdf = asr::SpecularBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", matteRefName.asChar()));
		break;
	}
		
	assembly->bsdfs().insert(bsdf);
		
	// we have a front/back side shading
	if( doubleSided )
	{
		assembly->bsdfs().insert(bsdfBack);
	}

	// EDF
	bool emitLight = false;
	getBool(MString("emitLight"), shaderNode, emitLight);
	MString edfName = "";
	if( emitLight )
	{
		MColor exitance(1.0, 1.0, 1.0);
		getColor(MString("exitance"), shaderNode, exitance);
		MString exitanceName = shaderNode.name() + "_exitance";
		defineColor(exitanceName, exitance, assembly);
		defineTexture(shaderNode, MString("exitance"), exitanceName);
		edfName = shaderNode.name() + "_exitance_edf";
		asr::ParamArray params;
		params.insert("exitance", exitanceName.asChar());
		edfName = shaderNode.name() + "_edf";
			
		assembly->edfs().insert(
			asr::DiffuseEDFFactory().create(edfName.asChar(), params));
	}		

	MString surfaceShaderNode = shaderName + "phys_surf";

	asf::auto_release_ptr<asr::SurfaceShader> physSurfaceShader;
	physSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
			surfaceShaderNode.asChar(),
			asr::ParamArray());
	assembly->surface_shaders().insert(physSurfaceShader);


	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", surfaceShaderNode.asChar());
	materialParams.insert("bsdf", shaderName.asChar());
	if( edfName != "")
		materialParams.insert("edf", edfName.asChar());

	defineBumpMap(materialParams, surfaceShader);

	assembly->materials().insert(
			asr::MaterialFactory::create(
			materialName.asChar(),
			materialParams));

	// this is not a good way to implement the double sided shading, have to find a better one
	if( doubleSided )
	{
		asr::ParamArray materialParams;
		materialParams.insert("surface_shader", surfaceShaderNode.asChar());
		materialParams.insert("bsdf", (shaderName + "_back").asChar());
		if( edfName != "")
			materialParams.insert("edf", edfName.asChar());
		assembly->materials().insert(
				asr::MaterialFactory::create(
				(materialName + "_back").asChar(),
				materialParams));
	}
}

void AppleseedRenderer::defineBumpMap(asr::ParamArray& materialParams, MObject& surfaceShader)
{
	MFnDependencyNode shaderNode(surfaceShader);

	MObject bumpObj = getOtherSideNode(MString("normalCamera"), surfaceShader);
	if( bumpObj == MObject::kNullObj)
	{
		logger.debug(MString("No normalCamera input found."));
		return;
	}
	if( !bumpObj.hasFn(MFn::kBump))
	{
		logger.debug(MString("Found cameraNormal input for shader: ") + shaderNode.name() + " : " + getObjectName(bumpObj) + " but is NO bump2d node");
		return;
	}
	logger.debug(MString("Found bump input for shader: ") + shaderNode.name() + " : " + getObjectName(bumpObj));
	MFnDependencyNode bumpNode(bumpObj);
	MObject fileObj = getOtherSideNode(MString("bumpValue"), bumpObj);
	if( fileObj == MObject::kNullObj)
	{
		logger.debug(MString("No bump file node found."));
		return;
	}	

	MString textureDefinition = "";
	this->defineTexture(bumpNode, MString("bumpValue"), textureDefinition);
	if( textureDefinition == "")
	{
		logger.debug(MString("BumpValue texture definition == empty --> invalid."));
		return;
	}
	int bumpInterp = 0;
	getEnum(MString("bumpInterp"), bumpNode, bumpInterp);
	float bumpDepth = 1.0f;
	getFloat(MString("bumpDepth"), bumpNode, bumpDepth);

	materialParams.insert("bump_amplitude", bumpDepth);
	materialParams.insert("displacement_map", textureDefinition);
	if( bumpInterp == 0) // bump
		materialParams.insert("displacement_method", "bump");
	if( bumpInterp > 0) // normal
		materialParams.insert("displacement_method", "normal");
}

//<material name="white_material" model="generic_material">
//        <parameter name="bsdf" value="white_material_brdf" />
//        <parameter name="bump_amplitude" value="0.1" />
//        <parameter name="displacement_map" value="bump_inst" />
//        <parameter name="displacement_method" value="bump" />
//        <parameter name="surface_shader" value="physical_shader" />
//    </material>
//

// translate shaders here, will be seperated later if we have a real shading language
void AppleseedRenderer::defineObjectMaterial(mtap_RenderGlobals *renderGlobals, mtap_MayaObject *obj, asf::StringArray& materialNames)
{

	asr::Assembly *assembly = getAssemblyFromMayaObject(obj);

	MObject shadingGroup;
	getObjectShadingGroups(obj->dagPath, shadingGroup);
	
	MObject surfaceShaderNode = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	if( surfaceShaderNode == MObject::kNullObj)
		return;

	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();

	// here I reuse the shader if it already exists in the assembly
	if( assembly->materials().get_by_name(materialName.asChar()) != NULL)
	{
		materialNames.push_back(materialName.asChar());
		return;		
	}

	MFn::Type shaderType = surfaceShaderNode.apiType();
	MFnDependencyNode depFn(surfaceShaderNode);

	MFnDependencyNode shaderNode(surfaceShaderNode);
	MString shaderName = shaderNode.name();

	asf::auto_release_ptr<asr::BSDF> bsdf, bsdfFront, bsdfBack;

	int type_id = depFn.typeId().id();
	if( type_id ==  SMOKE_SHADER)
	{
		this->defineSmokeShader(assembly, shadingGroup);
	}
	if( type_id ==  DIAGNOSTIC_SHADER)
	{
		this->defineDiagnosticShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( type_id ==  CONST_SHADER)
	{
		this->defineConstantShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( type_id ==  FASTSSS_SHADER)
	{
		this->defineFastSSSShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( type_id ==  AOVOXEL_SHADER)
	{		
		this->defineAoVoxelShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( type_id ==  AO_SHADER)
	{
		this->defineAoShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( type_id ==  PHYSICAL_SURFACE_SHADER)
	{
		this->definePhysSurfShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
		if( assembly->materials().get_by_name((materialName + "_back").asChar()) != NULL)
		{
			materialNames.push_back((materialName + "_back").asChar());
		}
	}
	if( shaderType == MFn::kLambert)
	{
		this->defineMayaLambertShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}
	if( shaderType == MFn::kPhong)
	{
		this->defineMayaPhongShader(assembly, shadingGroup);
		materialNames.push_back(materialName.asChar());
	}	

}
