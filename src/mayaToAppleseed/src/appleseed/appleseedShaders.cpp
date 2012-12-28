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
	if( this->scenePtr->colors().get_by_name("gray") == NULL)
	{
		this->scenePtr->colors().insert(
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
	asr::Texture *texture = this->scenePtr->textures().get_by_name(textureName.asChar());
	if( texture != NULL)
		this->scenePtr->textures().remove(texture);

	asr::TextureInstance *textureInstance = this->scenePtr->texture_instances().get_by_name(textureInstanceName.asChar());
	if( textureInstance != NULL)
		this->scenePtr->texture_instances().remove(textureInstance);

	MString colorProfileName;
	int profileId = 0;
	getEnum(MString("colorProfile"), fileTextureNode, profileId);
	logger.debug(MString("Color profile from fileNode: ") + profileId);
	
	MStringArray colorProfiles;
	colorProfiles.append("srgb"); //0 == none == default == sRGB
	colorProfiles.append("srgb"); //1 == undefined == default == sRGB
	colorProfiles.append("linear_rgb"); //2 == linear_rgb
	colorProfiles.append("srgb"); //3 == sRGB
	colorProfiles.append("linear_rgb"); //4 == linear_rec_709
	colorProfiles.append("linear_rgb"); //5 == hdtv_rec_709
	MString colorProfile = colorProfiles[profileId];
	
	asr::ParamArray params;
	logger.debug(MString("Now inserting file name: ") + fileTextureName);
	params.insert("filename", fileTextureName.asChar());      // OpenEXR only for now. The param is called filename but it can be a path
	params.insert("color_space", colorProfile.asChar());

    asf::auto_release_ptr<asr::Texture> textureElement(
        asr::DiskTexture2dFactory().create(
	    textureName.asChar(),
            params,
            this->project->get_search_paths()));    // the project holds a set of search paths to find textures and other assets
	this->scenePtr->textures().insert(textureElement);

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
	
	this->scenePtr->texture_instances().insert(tinst);

	textureDefinition = textureInstanceName;

}

#define isBlack(x) ((x.r + x.g + x.b) <= 0.0f)

void AppleseedRenderer::defineSmokeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	logger.debug(MString("SmokeShader: ") + getObjectName(shadingGroup) + " not yet implmented.");
}

void AppleseedRenderer::defineSmokeShader(asr::Assembly *assembly, MObject& shaderNode, MString& shaderName)
{
	logger.debug(MString("SmokeShader: ") + getObjectName(shaderNode) + " not yet implmented.");
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

void AppleseedRenderer::defineAoShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
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

void AppleseedRenderer::defineAoVoxelShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
	logger.debug(MString("Translating aoVoxelShader: ") + shaderNode.name());

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
	asrSurfaceShader = asr::VoxelAOSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray());
	assembly->surface_shaders().insert(asrSurfaceShader);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", shaderName.asChar());
}

void AppleseedRenderer::defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();
	MFnDependencyNode shaderNode(surfaceShader);
	MString shaderName = shaderNode.name();
	logger.debug(MString("Translating constantShader: ") + shaderNode.name());

	MColor color(1,1,1);
	getColor(MString("color"), shaderNode, color);

	MString colName = shaderName + "_color";
	this->defineColor(colName, color);

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
	asrSurfaceShader = asr::ConstantSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
		.insert("color", colName.asChar())
		);
	assembly->surface_shaders().insert(asrSurfaceShader);

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", shaderName.asChar());

	assembly->materials().insert(
			asr::MaterialFactory::create(
			materialName.asChar(),
			materialParams));

}

void AppleseedRenderer::defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup, MString& shaderName)
{
	MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
	logger.debug(MString("Translating constantShader: ") + shaderNode.name());

	MColor color(1,1,1);
	getColor(MString("color"), shaderNode, color);

	MString colName = shaderName + "_color";
	this->defineColor(colName, color);

	asf::auto_release_ptr<asr::SurfaceShader> asrSurfaceShader;
	asrSurfaceShader = asr::ConstantSurfaceShaderFactory().create(
		shaderName.asChar(),
		asr::ParamArray()
		.insert("color", colName.asChar())
		);
	assembly->surface_shaders().insert(asrSurfaceShader);
}

void AppleseedRenderer::defineCollectionShader(asr::Assembly *assembly, MObject& shadingGroup)
{

	//MObject surfaceShader = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	//MFnDependencyNode shadingGroupNode(shadingGroup);
	//MString materialName = shadingGroupNode.name();
	//MFnDependencyNode shaderNode(surfaceShader);
	//MString shaderName = shaderNode.name();
	//logger.debug(MString("Translating constantShader: ") + shaderNode.name());
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

// define surface shader only, no material
void AppleseedRenderer::defineDiagnosticShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{	
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
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
		this->defineColor(albedoName, albedo);

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

void AppleseedRenderer::defineFastSSSShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
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
		this->defineColor(albedoName, albedo);

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
}

void AppleseedRenderer::defineWireframeShader(asr::Assembly *assembly, MObject& shadingGroup)
{
}

void AppleseedRenderer::defineWireframeShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
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
	this->defineColor(matteRefName, color);
	MString origCName = matteRefName;
	this->defineTexture(shaderNode, MString("color"), matteRefName);
	bool hasMap = false;
	if(origCName != matteRefName)
		hasMap = true;

	MColor incandescence;
	getColor(MString("incandescence"), shaderNode, incandescence);
	MString incandName = shaderNode.name() + "_incandescence";
	this->defineColor(incandName, incandescence);
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

void AppleseedRenderer::defineMayaLambertShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
	logger.debug(MString("Translating mayaLambertShader: ") + shaderNode.name());

	MColor color(1.0f,1.0f,1.0f);
	getColor(MString("color"), shaderNode, color);
	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, color);
	MString origCName = matteRefName;
	this->defineTexture(shaderNode, MString("color"), matteRefName);
	bool hasMap = false;
	if(origCName != matteRefName)
		hasMap = true;

	MColor incandescence;
	getColor(MString("incandescence"), shaderNode, incandescence);
	MString incandName = shaderNode.name() + "_incandescence";
	this->defineColor(incandName, incandescence);
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
	shaderName = surfaceShaderNode;
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
	this->defineColor(matteRefName, color);

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

void AppleseedRenderer::defineMayaPhongShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
	logger.debug(MString("Translating mayaPhongShader: ") + shaderNode.name());

	MColor color(1.0f,1.0f,1.0f);
	getColor(MString("color"), shaderNode, color);

	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, color);

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

	MColor specReflectance(1.0f,1.0f,1.0f);
	getColor(MString("specular_reflectance"), shaderNode, specReflectance);

	float specular_reflectance_multiplier = 1.0f;
	getFloat(MString("specular_reflectance_multiplier"), shaderNode, specular_reflectance_multiplier);

	float shinyU = 1000.0f;
	float shinyV = 1000.0f;
	getFloat(MString("shininess_u"), shaderNode, shinyU);
	getFloat(MString("shininess_v"), shaderNode, shinyV);

	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, matteReflectance);
	this->defineTexture(shaderNode, MString("matte_reflectance"), matteRefName);

	float matte_reflectance_multiplier = 1.0f;
	getFloat(MString("matte_reflectance_multiplier"), shaderNode, matte_reflectance_multiplier);
	
	MString specRefName = shaderNode.name() + "_specRefl";
	this->defineColor(specRefName, specReflectance);
	this->defineTexture(shaderNode, MString("specular_reflectance"), specRefName);

	float roughness = .5f;
	getFloat(MString("roughness"), shaderNode, roughness);

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
		bsdf = asr::LambertianBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", matteRefName.asChar())
				.insert("reflectance_multiplier", matte_reflectance_multiplier)
				);
		break;
	case 1: // Ashikhmin
		bsdf = asr::AshikhminBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("diffuse_reflectance", matteRefName.asChar())
				.insert("diffuse_reflectance_multiplier", matte_reflectance_multiplier)
				.insert("glossy_reflectance", specRefName.asChar())
				.insert("glossy_reflectance_multiplier", specular_reflectance_multiplier)
				.insert("shininess_u", (MString("") + shinyU).asChar())
				.insert("shininess_v", (MString("") + shinyV).asChar())
				);
		break;
	case 2: // Kelemen
		bsdf = asr::KelemenBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("matte_reflectance", matteRefName.asChar())
				.insert("matte_reflectance_multiplier", matte_reflectance_multiplier)				
				.insert("specular_reflectance", specRefName.asChar())
				.insert("specular_reflectance_multiplier", specular_reflectance_multiplier)
				.insert("roughness", roughness)
				);
		break;
	case 3: // Specular			
		getColor(MString("transmittance"), shaderNode, transmittance);
		defineColor(transmittanceName, transmittance);
		defineTexture(shaderNode, MString("transmittance"), transmittanceName);
		getFloat(MString("transmittance_multiplier"), shaderNode, transmittanceMultiplier);
			
		getFloat(MString("from_ior"), shaderNode, from_ior);
			
		getFloat(MString("to_ior"), shaderNode, to_ior);
		// if transmittance > 0 then create a specular_btdf, else a specular brdf
		if( (transmittance.r + transmittance.g + transmittance.b) > 0.0f)
		{
			//doubleSided = true;
			bsdf = asr::SpecularBTDFFactory().create(
				(shaderName).asChar(),
				asr::ParamArray()
					.insert("reflectance",  specRefName.asChar())
					.insert("reflectance_multiplier",specular_reflectance_multiplier)
					.insert("from_ior", (MString("") + from_ior).asChar())
					.insert("to_ior", (MString("") + to_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
			bsdfBack = asr::SpecularBTDFFactory().create(
				(shaderName + "_back").asChar(),
				asr::ParamArray()
					.insert("reflectance", specRefName.asChar())
					.insert("reflectance_multiplier", specular_reflectance_multiplier)
					.insert("from_ior", (MString("") + to_ior).asChar())
					.insert("to_ior", (MString("") + from_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
		}else{
			bsdf = asr::SpecularBRDFFactory().create(
				shaderName.asChar(),
				asr::ParamArray()
					.insert("reflectance", specRefName.asChar())
					);
		}
		break;
	case 4: // Phong
		bsdf = asr::SpecularBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", specRefName.asChar()));
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
		defineColor(exitanceName, exitance);
		defineTexture(shaderNode, MString("exitance"), exitanceName);
		edfName = shaderNode.name() + "_exitance_edf";
		asr::ParamArray params;
		params.insert("exitance", exitanceName.asChar());
		edfName = shaderNode.name() + "_edf";
			
		assembly->edfs().insert(
			asr::DiffuseEDFFactory().create(edfName.asChar(), params));
	}		

	MString surfaceShaderName = shaderName + "phys_surf";
	asf::auto_release_ptr<asr::SurfaceShader> physSurfaceShader;
	physSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
			surfaceShaderName.asChar(),
			asr::ParamArray());
	assembly->surface_shaders().insert(physSurfaceShader);

	if(this->defineAOVShaders(assembly, surfaceShaderName))
	{
		logger.debug(MString("Successfully created an aov shader called: ") + surfaceShaderName);
	}

	asr::ParamArray materialParams;
	materialParams.insert("surface_shader", surfaceShaderName.asChar());
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
		materialParams.insert("surface_shader", surfaceShaderName.asChar());
		materialParams.insert("bsdf", (shaderName + "_back").asChar());
		if( edfName != "")
			materialParams.insert("edf", edfName.asChar());
		assembly->materials().insert(
				asr::MaterialFactory::create(
				(materialName + "_back").asChar(),
				materialParams));
	}
}

void AppleseedRenderer::definePhysSurfShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName)
{
	MFnDependencyNode shaderNode(surfaceShader);
	shaderName = shaderNode.name();
	logger.debug(MString("Translating physicalSurfaceShader: ") + shaderNode.name());

	MColor matteReflectance(1.0f,1.0f,1.0f);
	getColor(MString("matte_reflectance"), shaderNode, matteReflectance);

	MColor specReflectance(1.0f,1.0f,1.0f);
	getColor(MString("specular_reflectance"), shaderNode, specReflectance);

	float specular_reflectance_multiplier = 1.0f;
	getFloat(MString("specular_reflectance_multiplier"), shaderNode, specular_reflectance_multiplier);

	float shinyU = 1000.0f;
	float shinyV = 1000.0f;
	getFloat(MString("shininess_u"), shaderNode, shinyU);
	getFloat(MString("shininess_v"), shaderNode, shinyV);

	MString matteRefName = shaderNode.name() + "_matteRefl";
	this->defineColor(matteRefName, matteReflectance);
	this->defineTexture(shaderNode, MString("matte_reflectance"), matteRefName);

	float matte_reflectance_multiplier = 1.0f;
	getFloat(MString("matte_reflectance_multiplier"), shaderNode, matte_reflectance_multiplier);
	
	MString specRefName = shaderNode.name() + "_specRefl";
	this->defineColor(specRefName, specReflectance);
	this->defineTexture(shaderNode, MString("specular_reflectance"), specRefName);

	float roughness = .5f;
	getFloat(MString("roughness"), shaderNode, roughness);

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
		bsdf = asr::LambertianBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", matteRefName.asChar())
				.insert("reflectance_multiplier", matte_reflectance_multiplier)
				);
		break;
	case 1: // Ashikhmin
		bsdf = asr::AshikhminBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("diffuse_reflectance", matteRefName.asChar())
				.insert("diffuse_reflectance_multiplier", matte_reflectance_multiplier)
				.insert("glossy_reflectance", specRefName.asChar())
				.insert("glossy_reflectance_multiplier", specular_reflectance_multiplier)
				.insert("shininess_u", (MString("") + shinyU).asChar())
				.insert("shininess_v", (MString("") + shinyV).asChar())
				);
		break;
	case 2: // Kelemen
		bsdf = asr::KelemenBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("matte_reflectance", matteRefName.asChar())
				.insert("matte_reflectance_multiplier", matte_reflectance_multiplier)				
				.insert("specular_reflectance", specRefName.asChar())
				.insert("specular_reflectance_multiplier", specular_reflectance_multiplier)
				.insert("roughness", roughness)
				);
		break;
	case 3: // Specular			
		getColor(MString("transmittance"), shaderNode, transmittance);
		defineColor(transmittanceName, transmittance);
		defineTexture(shaderNode, MString("transmittance"), transmittanceName);
		getFloat(MString("transmittance_multiplier"), shaderNode, transmittanceMultiplier);
			
		getFloat(MString("from_ior"), shaderNode, from_ior);
			
		getFloat(MString("to_ior"), shaderNode, to_ior);
		// if transmittance > 0 then create a specular_btdf, else a specular brdf
		if( (transmittance.r + transmittance.g + transmittance.b) > 0.0f)
		{
			//doubleSided = true;
			bsdf = asr::SpecularBTDFFactory().create(
				(shaderName).asChar(),
				asr::ParamArray()
					.insert("reflectance",  specRefName.asChar())
					.insert("reflectance_multiplier",specular_reflectance_multiplier)
					.insert("from_ior", (MString("") + from_ior).asChar())
					.insert("to_ior", (MString("") + to_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
			bsdfBack = asr::SpecularBTDFFactory().create(
				(shaderName + "_back").asChar(),
				asr::ParamArray()
					.insert("reflectance", specRefName.asChar())
					.insert("reflectance_multiplier", specular_reflectance_multiplier)
					.insert("from_ior", (MString("") + to_ior).asChar())
					.insert("to_ior", (MString("") + from_ior).asChar())
					.insert("transmittance", transmittanceName.asChar())
					.insert("transmittance_multiplier", (MString("") + transmittanceMultiplier).asChar())
					);
		}else{
			bsdf = asr::SpecularBRDFFactory().create(
				shaderName.asChar(),
				asr::ParamArray()
					.insert("reflectance", specRefName.asChar())
					);
		}
		break;
	case 4: // Phong
		bsdf = asr::SpecularBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", specRefName.asChar()));
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
		defineColor(exitanceName, exitance);
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

bool AppleseedRenderer::defineAOVShaders(asr::Assembly *assembly, MString& physSurfaceShaderName)
{
	if( assembly->surface_shaders().get_by_name("aovs") != NULL)
	{
		logger.debug("AOV collection shader already defined.");
		physSurfaceShaderName = "aovs";
		return true;
	}
	MStatus stat;
	MPlugArray plugArray;
	MObject globalsObj = objectFromName(MString("appleseedGlobals"));
	MFnDependencyNode depFn(globalsObj);
	MPlug AOVs = depFn.findPlug(MString("AOVs"), &stat);
	if( !stat )
	{
		logger.debug("Unable to find AOVs plug.");
		return false;
	}
	if( AOVs.numElements() == 0)
	{
		logger.debug("No elements in AOVs.");
		return false;
	}

	asr::ParamArray params;
	params.insert((MString("surface_shader1")).asChar(), physSurfaceShaderName);

    for(unsigned int i = 0; i < AOVs.numElements (); i++)
    {
		MPlug elementPlug = AOVs[i];
		elementPlug.connectedTo(plugArray, true, false, &stat);
		if( plugArray.length() == 0)
		{
			logger.debug(MString("No aovs connected to ") + elementPlug.name());
			continue;
		}
		MObject shaderNode = plugArray[0].node();
		MString nodeName = getObjectName(shaderNode);
		logger.debug(elementPlug.name() + " connected to: " + nodeName);
		MString shaderName;
		MFn::Type shaderType = shaderNode.apiType();
		MFnDependencyNode shaderFn(shaderNode);
		int type_id = shaderFn.typeId().id();
		if( type_id ==  SMOKE_SHADER)
		{
			this->defineSmokeShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  DIAGNOSTIC_SHADER)
		{
			this->defineDiagnosticShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  CONST_SHADER)
		{
			this->defineConstantShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  FASTSSS_SHADER)
		{
			this->defineFastSSSShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  AOVOXEL_SHADER)
		{		
			this->defineAoVoxelShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  AO_SHADER)
		{
			this->defineAoShader(assembly, shaderNode, shaderName);
		}
		if( type_id ==  PHYSICAL_SURFACE_SHADER)
		{
			this->definePhysSurfShader(assembly, shaderNode, shaderName);
		}
		if( shaderType == MFn::kLambert)
		{
			this->defineMayaLambertShader(assembly, shaderNode, shaderName);
		}
		if( shaderType == MFn::kPhong)
		{
			this->defineMayaPhongShader(assembly, shaderNode, shaderName);
		}	
		logger.debug(MString("Inserting aov shader into array: ") + shaderName);
		params.insert((MString("surface_shader")+(i+2)).asChar(), shaderName);
    }
	if( params.size() == 0)
	{
		logger.debug("No connections found in AOVs.");
		return false;
	}
	
	this->hasAOVs = true;

	MString surfaceShaderNode = "aovs";
	asf::auto_release_ptr<asr::SurfaceShader> physSurfaceShader;
	physSurfaceShader = asr::SurfaceShaderCollectionFactory().create(
			surfaceShaderNode.asChar(),
			params);
	assembly->surface_shaders().insert(physSurfaceShader);
	physSurfaceShaderName = surfaceShaderNode;
	return true;
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
