#include "appleseedUtils.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "renderGlobals.h"
#include "appleseed.h"
#include "utilities/pystring.h"
#include "world.h"

#include "renderer/api/color.h"
#include "renderer/api/material.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/bsdf.h"
#include "renderer/api/texture.h"

void defineDefaultMaterial(asr::Project *project)
{
	asr::Assembly *assembly = getMasterAssemblyFromProject(project);
	// Create a color called "gray" and insert it into the assembly.
	static const float GrayReflectance[] = { 0.5f, 0.5f, 0.5f };
	assembly->colors().insert(
		asr::ColorEntityFactory::create(
		"gray",
		asr::ParamArray()
		.insert("color_space", "srgb"),
		asr::ColorValueArray(3, GrayReflectance)));

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
		"default",
		asr::ParamArray()
		.insert("surface_shader", "physical_surface_shader")
		.insert("bsdf", "diffuse_gray_brdf")));
}

MString getAssemblyInstanceName(MayaObject *obj)
{
	MString assemblyName = getAssemblyName(obj);
	if (obj->instancerParticleId > -1)
		assemblyName = assemblyName + "_" + obj->instancerParticleId;
	if (obj->instanceNumber > 0)
		assemblyName = assemblyName + "_" + obj->instanceNumber;

	return assemblyName + "_assInst";
}

MString getObjectInstanceName(MayaObject *obj)
{
	return obj->fullNiceName + "_objInst";
}

MString getAssemblyName(MayaObject *obj)
{
	// if an obj is an instanced object, get the assembly name of the original object.
	if ((obj->instanceNumber > 0) || (obj->instancerParticleId > -1))
	{
		if (obj->origObject)
		{
			std::shared_ptr<mtap_MayaObject> orig = std::static_pointer_cast<mtap_MayaObject>(obj->origObject);
			return getAssemblyName(orig.get());
		}
		else{
			Logging::error(MString("Object is instanced but has no orig object --> Problem: ") + obj->fullName);
			return "";
		}
	}

	// We always use the transform name as assemblyName, if we have a shape node,
	// go up one element to the transform node.
	MDagPath path = obj->dagPath;
	MStatus stat;
	while (!path.node().hasFn(MFn::kTransform) && !path.node().hasFn(MFn::kWorld))
	{
		MFn::Type pt = path.apiType();
		MFn::Type nt = path.node().apiType();
		stat = path.pop();
		if (!stat)
			break;
	}
	if (path.node().hasFn(MFn::kWorld))
		return "world";
	else
		return path.fullPathName() + "_ass";
}

void defineScene(asr::Project *project)
{
	asr::Scene *scenePtr = project->get_scene();
	if (scenePtr == nullptr)
	{
		asf::auto_release_ptr<asr::Scene> scene(asr::SceneFactory::create());
		project->set_scene(scene);
	}
}

asr::Scene *getSceneFromProject(asr::Project *project)
{
	defineScene(project);
	return project->get_scene();
}


// we first define an assembly which contains the world assembly. This "uberMaster" contains the global transformation.
void defineMasterAssembly(asr::Project *project)
{

	MMatrix conversionMatrix;
	conversionMatrix.setToIdentity();
	MayaTo::MayaToWorld *world = MayaTo::getWorldPtr();
	if (world != nullptr)
	{
		RenderGlobals *rg = world->worldRenderGlobalsPtr.get();
		if (rg != nullptr)
			conversionMatrix = rg->globalConversionMatrix;
	}
	if (getSceneFromProject(project)->assemblies().get_by_name("world") == nullptr)
	{
		asf::auto_release_ptr<asr::Assembly> assembly(asr::AssemblyFactory().create("world", asr::ParamArray()));
		getSceneFromProject(project)->assemblies().insert(assembly);
		asf::Matrix4d appMatrix;
		MMatrix transformMatrix;
		transformMatrix.setToIdentity();
		transformMatrix *= conversionMatrix;
		asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create("world_Inst", asr::ParamArray(), "world");
		MMatrixToAMatrix(transformMatrix, appMatrix);
		assemblyInstance->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
		getSceneFromProject(project)->assembly_instances().insert(assemblyInstance);
	}
}

asr::Assembly *getMasterAssemblyFromProject(asr::Project *project)
{
	defineMasterAssembly(project);
	return getSceneFromProject(project)->assemblies().get_by_name("world");
}

// return the maya object above which has it's own assembly
MayaObject *getAssemblyMayaObject(MayaObject *mobj)
{
	MayaObject *obj = mobj;
	if (obj->instanceNumber > 0)
		obj = mobj->origObject.get();

	if (obj->attributes)
	{
		std::shared_ptr<mtap_ObjectAttributes> att = std::static_pointer_cast<mtap_ObjectAttributes>(obj->attributes);
		return att->assemblyObject;
	}
	return nullptr; // only happens if obj is world
}

asr::Assembly *getCreateObjectAssembly(std::shared_ptr<MayaObject> obj)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	MayaObject *assemblyObject = getAssemblyMayaObject(obj.get());
	if (assemblyObject == nullptr)
	{
		Logging::debug("create mesh assemblyPtr == null");
		return nullptr;
	}
	MString assemblyName = getAssemblyName(assemblyObject);
	MString assemblyInstanceName = getAssemblyInstanceName(assemblyObject);
	
	asr::Assembly *master = getMasterAssemblyFromProject(appleRenderer->getProjectPtr());
	if (obj->isLight() && (!obj->mobject.hasFn(MFn::kAreaLight)))
	{
		return master;
	}
	asr::Assembly *ass = master->assemblies().get_by_name(assemblyName.asChar());
	if (assemblyName == "world")
		ass = master;

	if (ass == nullptr)
	{
		asf::auto_release_ptr<asr::Assembly> assembly(asr::AssemblyFactory().create(assemblyName.asChar(), asr::ParamArray()));
		master->assemblies().insert(assembly);
		ass = master->assemblies().get_by_name(assemblyName.asChar());

		asf::auto_release_ptr<asr::AssemblyInstance> assInst(asr::AssemblyInstanceFactory().create(assemblyInstanceName.asChar(), asr::ParamArray(), assemblyName.asChar()));

		fillMatrices(obj, assInst->transform_sequence());
		master->assembly_instances().insert(assInst);
	}
	return ass;
}

asr::AssemblyInstance *getExistingObjectAssemblyInstance(MayaObject *obj)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	MayaObject *assemblyObject = getAssemblyMayaObject(obj);
	if (assemblyObject == nullptr)
	{
		Logging::debug("create mesh assemblyPtr == null");
		return nullptr;
	}
	MString assemblyName = getAssemblyName(obj);
	MString assemblyInstanceName = getAssemblyInstanceName(obj);
	asr::Assembly *ass = getMasterAssemblyFromProject(appleRenderer->getProjectPtr());
	if (assemblyName == "world")
		ass = getMasterAssemblyFromProject(appleRenderer->getProjectPtr());
	
	if (ass == nullptr)
		return nullptr;
	return ass->assembly_instances().get_by_name(assemblyInstanceName.asChar());
}


//
// colors are defined in the scene scope, makes handling easier
//
void mayaColorToFloat(MColor& col, float *floatCol, float *alpha)
{
	floatCol[0] = col.r;
	floatCol[1] = col.g;
	floatCol[2] = col.b;
	*alpha = col.a;
}

void removeColorEntityIfItExists(MString& colorName)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	assert(appleRenderer != nullptr);
	asr::Scene *scene = getSceneFromProject(appleRenderer->getProjectPtr());
	asr::ColorEntity *entity = scene->colors().get_by_name(colorName.asChar());
	if( entity != nullptr)
	{
		scene->colors().remove(entity);
	}
}

void defineColor(MString& name, MColor& color, float intensity, MString colorSpace)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	assert(appleRenderer != nullptr);

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

	asr::Scene *scene = getSceneFromProject(appleRenderer->getProjectPtr());
	scene->colors().insert(colorEntity);
}

void defineColor(asr::Project *project, const char *name, MColor color, float intensity, MString colorSpace)
{
	asr::Scene *scene = project->get_scene();
	asr::ColorEntity *col = scene->colors().get_by_name(name);
	if (col != nullptr)
		scene->colors().remove(col);

	float colorDef[3];
	float alpha = color.a;
	mayaColorToFloat(color, colorDef, &alpha);
	asf::auto_release_ptr<asr::ColorEntity> colorEntity = asr::ColorEntityFactory::create(
		name,
		asr::ParamArray()
		.insert("color_space", colorSpace.asChar())
		.insert("multiplier", intensity),
		asr::ColorValueArray(3, colorDef),
		asr::ColorValueArray(1, &alpha));

	scene->colors().insert(colorEntity);
}

// check if a texture is connected to this attribute. If yes, return the texture name, if not define the currentColor and return the color name.
MString colorOrMap(MFnDependencyNode& shaderNode, MString& attributeName)
{
	MString definition = defineTexture(shaderNode, attributeName);
	if (definition.length() == 0)
	{
		MColor color = getColorAttr(attributeName.asChar(), shaderNode);
		defineColor(attributeName + "_color", color, 1.0f);
		definition = attributeName + "_color";
	}
	return definition;
}

void removeTextureEntityIfItExists(MString& textureName)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	assert(appleRenderer != nullptr);
	asr::Scene *scene = getSceneFromProject(appleRenderer->getProjectPtr());

	MString textureInstanceName = textureName + "_texInst";
	asr::Entity *texture = (asr::Entity *)scene->textures().get_by_name(textureName.asChar());
	if( texture != nullptr)
		scene->textures().remove(texture);

	asr::TextureInstance *textureInstance = scene->texture_instances().get_by_name(textureInstanceName.asChar());
	if( textureInstance != nullptr)
		scene->texture_instances().remove(textureInstance);
}
MString defineTexture(MFnDependencyNode& shader, MString& attributeName)
{
	std::shared_ptr<AppleRender::AppleseedRenderer> appleRenderer = std::static_pointer_cast<AppleRender::AppleseedRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	assert(appleRenderer != nullptr);
	asr::Scene *scene = getSceneFromProject(appleRenderer->getProjectPtr());
	asf::SearchPaths &searchPaths = appleRenderer->getProjectPtr()->search_paths();

	MStatus stat;
	MString textureDefinition("");
	MPlug plug = shader.findPlug(attributeName, &stat);	
	if( stat != MStatus::kSuccess)
		return textureDefinition;
	if( !plug.isConnected() )
		return textureDefinition;
	MObject connectedNode = getConnectedInNode(plug);
	
	if (!connectedNode.hasFn(MFn::kFileTexture))
		return textureDefinition;

	MFnDependencyNode fileTextureNode(connectedNode, &stat);
	MString textureName = fileTextureNode.name() + "_texture";
	MString fileTextureName = "";
	getString(MString("fileTextureName"), fileTextureNode, fileTextureName);
	if( !pystring::endswith(fileTextureName.asChar(), ".exr") || (fileTextureName.length() == 0))
	{
		if( fileTextureName.length() == 0)
			Logging::warning(MString("FileTextureName has no content."));
		else
			Logging::warning(MString("FileTextureName does not have an .exr extension. Other filetypes are not yet supported, sorry."));
		return textureDefinition;
	}

	removeTextureEntityIfItExists(textureName);

	//MString colorProfile = getTextureColorProfile(fileTextureNode);
	MString colorProfile = "srgb";

	asr::ParamArray params;
	Logging::debug(MString("Now inserting file name: ") + fileTextureName);
	params.insert("filename", fileTextureName.asChar());      // OpenEXR only for now. The param is called filename but it can be a path
	params.insert("color_space", colorProfile.asChar());
	
    asf::auto_release_ptr<asr::Texture> textureElement(
		asr::DiskTexture2dFactory().create(
	    textureName.asChar(),
            params,
			searchPaths));    // the project holds a set of search paths to find textures and other assets
	scene->textures().insert(textureElement);

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
	
	scene->texture_instances().insert(tinst);

	return textureInstanceName;
}

void fillTransformMatrices(MMatrix matrix, asr::AssemblyInstance *assInstance)
{
	assInstance->transform_sequence().clear();
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = matrix;
	MMatrixToAMatrix(colMatrix, appMatrix);
	assInstance->transform_sequence().set_transform(
			0.0f,
			asf::Transformd::from_local_to_parent(appMatrix));
}

void fillMatrices(std::shared_ptr<MayaObject> obj, asr::TransformSequence& transformSequence)
{
	MMatrix conversionMatrix = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->globalConversionMatrix;
	float scaleFactor = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->scaleFactor;
	transformSequence.clear();

	// in ipr mode we have to update the matrix manually
	if (MayaTo::getWorldPtr()->getRenderType() == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
	{
		obj->transformMatrices.clear();
		obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
	}
	size_t numSteps = obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if (divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	// cameras pos has to be scaled because it is not placed into the world assembly but directly in the scene.
	// We only scale the transform because a scaled camera will result in different renderings (e.g. dof)
	asf::Matrix4d appMatrix;
	MMatrix transformMatrix;
	for (size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		if (obj->mobject.hasFn(MFn::kCamera))
		{
			colMatrix.matrix[3][0] *= scaleFactor;
			colMatrix.matrix[3][1] *= scaleFactor;
			colMatrix.matrix[3][2] *= scaleFactor;
		}
		MMatrixToAMatrix(colMatrix, appMatrix);
		transformSequence.set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}
void fillTransformMatrices(std::shared_ptr<MayaObject> obj, asr::Light *light)
{
	// in ipr mode we have to update the matrix manually
	if (MayaTo::getWorldPtr()->getRenderType() == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
	{
		obj->transformMatrices.clear();
		obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
	}
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = obj->transformMatrices[0];
	MMatrixToAMatrix(colMatrix, appMatrix);
	light->set_transform(asf::Transformd::from_local_to_parent(appMatrix));	
}

void MMatrixToAMatrix(MMatrix& mayaMatrix, asf::Matrix4d& appleMatrix)
{
	MMatrix rowMatrix = mayaMatrix.transpose();
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
}

asf::Matrix4d MMatrixToAMatrix(MMatrix& mayaMatrix)
{
	asf::Matrix4d appleMatrix;
	MMatrix rowMatrix = mayaMatrix.transpose();
	for (int i = 0; i < 4; i++)
		for (int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
	return appleMatrix;
}

asf::Matrix4d MMatrixToAMatrix(MMatrix mayaMatrix)
{
	asf::Matrix4d appleMatrix;
	MMatrix rowMatrix = mayaMatrix.transpose();
	for (int i = 0; i < 4; i++)
		for (int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
	return appleMatrix;
}

void addVisibilityFlags(MObject& obj, asr::ParamArray& paramArray)
{
	MFnDependencyNode depFn(obj);

	if (obj.hasFn(MFn::kMesh))
	{
		if (!getBoolAttr("primaryVisibility", depFn, true))
			paramArray.insert_path("visibility.camera", false);

		if (!getBoolAttr("castsShadows", depFn, true))
			paramArray.insert_path("visibility.shadow", false);

		if (!getBoolAttr("visibleInRefractions", depFn, true))
			paramArray.insert_path("visibility.transparency", false);

		if (!getBoolAttr("mtap_visibleLights", depFn, true))
			paramArray.insert_path("visibility.light", false);

		if (!getBoolAttr("mtap_visibleProbe", depFn, true))
			paramArray.insert_path("visibility.probe", false);

		if (!getBoolAttr("mtap_visibleGlossy", depFn, true))
			paramArray.insert_path("visibility.glossy", false);

		if (!getBoolAttr("mtap_visibleSpecular", depFn, true))
			paramArray.insert_path("visibility.specular", false);

		if (!getBoolAttr("mtap_visibleDiffuse", depFn, true))
			paramArray.insert_path("visibility.diffuse", false);
	}

	if (obj.hasFn(MFn::kAreaLight))
	{
		if (!getBoolAttr("primaryVisibility", depFn, true))
			paramArray.insert_path("visibility.camera", false);

		if (!getBoolAttr("castsShadows", depFn, true))
			paramArray.insert_path("visibility.shadow", false);

		if (!getBoolAttr("visibleInRefractions", depFn, true))
			paramArray.insert_path("visibility.transparency", false);

		if (!getBoolAttr("mtap_visibleLights", depFn, true))
			paramArray.insert_path("visibility.light", false);

		if (!getBoolAttr("mtap_visibleProbe", depFn, true))
			paramArray.insert_path("visibility.probe", false);

		if (!getBoolAttr("mtap_visibleGlossy", depFn, true))
			paramArray.insert_path("visibility.glossy", false);

		if (!getBoolAttr("mtap_visibleSpecular", depFn, true))
			paramArray.insert_path("visibility.specular", false);

		if (!getBoolAttr("mtap_visibleDiffuse", depFn, true))
			paramArray.insert_path("visibility.diffuse", false);
	}
}

void addVisibilityFlags(std::shared_ptr<MayaObject> obj, asr::ParamArray& paramArray)
{
	addVisibilityFlags(obj->mobject, paramArray);
}
