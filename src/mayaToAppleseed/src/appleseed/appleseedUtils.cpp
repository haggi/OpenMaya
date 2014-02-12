#include "appleseed.h"

#include "renderer/api/texture.h"

#include <maya/MColor.h>
#include <maya/MPlugArray.h>
#include "../mtap_common/mtap_mayaScene.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"

static Logging logger;

using namespace AppleRender;

// 
// If the object has an assembly itself, there should be an assembly with the object name in the database.
// If the object does not have an assembly, the objAttributes should contain the parent assembly node.

asr::Assembly *AppleseedRenderer::getAssemblyFromMayaObject(mtap_MayaObject *obj)
{
	asr::Assembly *assembly = NULL;

	// in ipr mode, the geometry is always placed into the assembly of the parent transform node
	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		if( obj->parent != NULL)
		{
			mtap_MayaObject *parentObject = (mtap_MayaObject *)obj->parent;
			if( parentObject->objectAssembly != NULL)
				assembly = parentObject->objectAssembly;
		}
	}else{
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
		if( att == NULL)
			logger.debug("Error");
		if( att->needsOwnAssembly )
			return obj->objectAssembly;

		return att->assemblyObject->objectAssembly;
	}
	return assembly;
}


// we have a very flat hierarchy, what means we do not have hierarchies of assemblies.
// all assemblies are placed in the world, as well as all assemblyInstances
asr::AssemblyInstance *AppleseedRenderer::getAssemblyInstFromMayaObject(mtap_MayaObject *obj)
{
	asr::AssemblyInstance *assemblyInst = NULL;
	MString assInstName = obj->getAssemblyInstName();
	logger.debug(MString("Searching assembly instance in world: ") + assInstName);
	assemblyInst = this->masterAssembly->assembly_instances().get_by_name(assInstName.asChar());
	return assemblyInst;
}


//
// colors are defined in the scene scope, makes handling easier
//

void AppleseedRenderer::mayaColorToFloat(MColor& col, float *floatCol, float *alpha)
{
	floatCol[0] = col.r;
	floatCol[1] = col.g;
	floatCol[2] = col.b;
	*alpha = col.a;
}

void AppleseedRenderer::removeColorEntityIfItExists(MString& colorName)
{
	asr::ColorEntity *entity = this->scenePtr->colors().get_by_name(colorName.asChar());
	if( entity != NULL)
	{
		this->scenePtr->colors().remove(entity);
	}
}

void AppleseedRenderer::defineColor(MString& name, MColor& color, float intensity, MString colorSpace)
{
	float colorDef[3];
	float alpha = color.a;
	mayaColorToFloat(color, colorDef, &alpha);
	removeColorEntityIfItExists(name);

	asf::auto_release_ptr<asr::ColorEntity> colorEntity = asr::ColorEntityFactory::create(
				name.asChar(),
				asr::ParamArray()
					.insert("color_space", colorSpace.asChar())
					.insert("multiplier", intensity),
				asr::ColorValueArray(3, colorDef),
				asr::ColorValueArray(1, &alpha));

	this->scenePtr->colors().insert(colorEntity);
}

MString AppleseedRenderer::defineColor(MFnDependencyNode& shader, MString& attributeName, MString colorSpace = "srgb", float intensity = 1.0f)
{
	MColor col(0,0,0);
	if(!getColor(attributeName, shader, col))
	{
		logger.error(MString("Unable to get color values from node: ") + shader.name());
		return "";
	}
	MString colorName = shader.name() + "_" + attributeName;
	defineColor(colorName, col, intensity, colorSpace);
	return colorName;
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

void AppleseedRenderer::removeTextureEntityIfItExists(MString& textureName)
{
	MString textureInstanceName = textureName + "_texInst";
	asr::Texture *texture = this->scenePtr->textures().get_by_name(textureName.asChar());
	if( texture != NULL)
		this->scenePtr->textures().remove(texture);

	asr::TextureInstance *textureInstance = this->scenePtr->texture_instances().get_by_name(textureInstanceName.asChar());
	if( textureInstance != NULL)
		this->scenePtr->texture_instances().remove(textureInstance);
}

MString AppleseedRenderer::getTextureColorProfile(MFnDependencyNode& fileTextureNode)
{
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
	return colorProfiles[profileId];
}

void AppleseedRenderer::defineTexture(MObject& fileTextureObj)
{
	MStatus stat;
	MString textureDefinition("");

	MFnDependencyNode fileTextureNode(fileTextureObj, &stat);
	MString textureName = fileTextureNode.name() + "_diskTexture";
	MString fileTextureName = "";
	getString(MString("fileTextureName"), fileTextureNode, fileTextureName);
	if((!pystring::endswith(fileTextureName.asChar(), ".exr") && (!pystring::endswith(fileTextureName.asChar(), ".png"))) || (fileTextureName.length() == 0))
	{
		if( fileTextureName.length() == 0)
			logger.warning(MString("FileTextureName has no content."));
		else
			logger.warning(MString("FileTextureName does not have an .exr extension. Other filetypes are not yet supported, sorry."));
		return;
	}

	removeTextureEntityIfItExists(textureName);

	MString colorProfile = getTextureColorProfile(fileTextureNode);
	
	asr::ParamArray params;
	logger.detail(MString("Now inserting file name: ") + fileTextureName);
	params.insert("filename", fileTextureName.asChar()); 
	params.insert("color_space", colorProfile.asChar());
	
    asf::auto_release_ptr<asr::Texture> textureElement(
        asr::DiskTexture2dFactory().create(
	    textureName.asChar(),
            params,
            this->project->search_paths()));    // the project holds a set of search paths to find textures and other assets
	this->scenePtr->textures().insert(textureElement);

	bool alphaIsLuminance = false;
	getBool(MString("alphaIsLuminance"), fileTextureNode, alphaIsLuminance);
	asr::ParamArray tInstParams;
	tInstParams.insert("addressing_mode", "clamp");
	//tInstParams.insert("addressing_mode", "wrap");
	tInstParams.insert("filtering_mode", "bilinear");
	if( alphaIsLuminance )
		tInstParams.insert("alpha_mode", "luminance");

	MString textureInstanceName = fileTextureNode.name() ;
	asf::auto_release_ptr<asr::TextureInstance> tinst = asr::TextureInstanceFactory().create(
	   textureInstanceName.asChar(),
	   tInstParams,
	   textureName.asChar());
	
	this->scenePtr->texture_instances().insert(tinst);

}

MString AppleseedRenderer::defineTexture(MFnDependencyNode& shader, MString& attributeName)
{
	MStatus stat;
	MString textureDefinition("");

	MPlug plug = shader.findPlug(attributeName, &stat);	
	if( stat != MStatus::kSuccess)
		return textureDefinition;
	if( !plug.isConnected() )
		return textureDefinition;

	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);
	if( stat != MStatus::kSuccess) 
		return textureDefinition;

	if( plugArray.length() == 0)
		return textureDefinition;

	MPlug otherSidePlug = plugArray[0];
	MObject inputNode = otherSidePlug.node();
	
	if( !inputNode.hasFn(MFn::kFileTexture))
		return textureDefinition;

	MFnDependencyNode fileTextureNode(inputNode, &stat);
	MString textureName = fileTextureNode.name() + "_texture";

	logger.info(MString("Found fileTextureNode: ") + fileTextureNode.name());
	MString fileTextureName = "";
	getString(MString("fileTextureName"), fileTextureNode, fileTextureName);
	logger.info(MString("Found filename: ") + fileTextureName);
	if( !pystring::endswith(fileTextureName.asChar(), ".exr") || (fileTextureName.length() == 0))
	{
		if( fileTextureName.length() == 0)
			logger.warning(MString("FileTextureName has no content."));
		else
			logger.warning(MString("FileTextureName does not have an .exr extension. Other filetypes are not yet supported, sorry."));
		return textureDefinition;
	}

	removeTextureEntityIfItExists(textureName);

	MString colorProfile = getTextureColorProfile(fileTextureNode);
	
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
	//tInstParams.insert("addressing_mode", "wrap");
	tInstParams.insert("filtering_mode", "bilinear");
	if( alphaIsLuminance )
		tInstParams.insert("alpha_mode", "luminance");

	MString textureInstanceName = textureName + "_texInst";
	asf::auto_release_ptr<asr::TextureInstance> tinst = asr::TextureInstanceFactory().create(
	   textureInstanceName.asChar(),
	   tInstParams,
	   textureName.asChar());
	
	this->scenePtr->texture_instances().insert(tinst);

	return textureInstanceName;
}


MString AppleseedRenderer::defineColorAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName)
{
	MString definition = defineTexture(shaderNode, attributeName);
	if( definition.length() == 0)
		definition = defineColor(shaderNode, attributeName);
	return definition;
}

MString AppleseedRenderer::defineColorAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName, float intensity = 1.0f)
{
	MString definition = defineTexture(shaderNode, attributeName);
	if( definition.length() == 0)
		definition = defineColor(shaderNode, attributeName, "srgb", intensity);
	return definition;
}

MString AppleseedRenderer::defineScalarAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName)
{
	return defineTexture(shaderNode, attributeName);
}


void AppleseedRenderer::fillTransformMatices(MMatrix matrix, asr::AssemblyInstance *assInstance)
{
	assInstance->transform_sequence().clear();
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = matrix;
	this->MMatrixToAMatrix(colMatrix, appMatrix);
	assInstance->transform_sequence().set_transform(
			0.0f,
			asf::Transformd::from_local_to_parent(appMatrix));
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::AssemblyInstance *assInstance, MMatrix correctorMatrix)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	asf::Matrix4d appMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		colMatrix *= correctorMatrix;
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::AssemblyInstance *assInstance)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	asf::Matrix4d appMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::Camera *assInstance)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	// here only the transform will be scaled because a scaled camera will result in different renderings (e.g. dof)

	asf::Matrix4d appMatrix;
	MMatrix transformMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		transformMatrix.setToIdentity();
		transformMatrix.matrix[3][0] = obj->transformMatrices[matrixId].matrix[3][0];
		transformMatrix.matrix[3][1] = obj->transformMatrices[matrixId].matrix[3][1];
		transformMatrix.matrix[3][2] = obj->transformMatrices[matrixId].matrix[3][2];
		transformMatrix *= this->renderGlobals->globalConversionMatrix;
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		colMatrix.matrix[3][0] = transformMatrix.matrix[3][0];
		colMatrix.matrix[3][1] = transformMatrix.matrix[3][1];
		colMatrix.matrix[3][2] = transformMatrix.matrix[3][2];
		this->MMatrixToAMatrix(colMatrix, appMatrix);
		logger.trace(MString("cam mat ") + colMatrix.matrix[3][0] + " " + colMatrix.matrix[3][1] + " " + colMatrix.matrix[3][2]);
		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::Light *light)
{
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = obj->transformMatrices[0];
	colMatrix *=  this->renderGlobals->globalConversionMatrix;
	this->MMatrixToAMatrix(colMatrix, appMatrix);
	light->set_transform(asf::Transformd::from_local_to_parent(appMatrix));	
}

void AppleseedRenderer::MMatrixToAMatrix(MMatrix& mayaMatrix, asf::Matrix4d& appleMatrix)
{
	MMatrix rowMatrix = mayaMatrix.transpose();
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
}
