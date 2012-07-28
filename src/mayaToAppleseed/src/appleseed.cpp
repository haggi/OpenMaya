

#include "appleseed.h"
#include "mtap_rendererController.h"
#include "mtap_tileCallback.h"
#include "shadingtools/material.h"

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

// only temporary until api is updated
#include "renderer/modeling/light/spotlight.h" 

#include "utilities/logging.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnCamera.h>
#include <maya/MFnLight.h>
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include <maya/MFileIO.h>



#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "threads/threads.h"
#include "threads/queue.h"
#include "utilities/pystring.h"
#include "mtap_mayaScene.h"

static Logging logger;

static int tileCount = 0;
static int tileCountTotal = 0;

using namespace AppleRender;

std::vector<asr::Entity *> definedEntities;

AppleseedRenderer::AppleseedRenderer()
{
	this->mtap_scene = NULL;
	this->renderGlobals = NULL;
	definedEntities.clear();
}

AppleseedRenderer::~AppleseedRenderer()
{
	logger.debug("Releasing scene");
	this->scene.reset();
	logger.debug("Releasing project");
	this->project.reset();
	logger.debug("Releasing done.");
}

void AppleseedRenderer::defineProject()
{
	//TODO: the logging procedures of applesse causes a crash, no idea why.
	//std::auto_ptr<asf::LogTargetBase> log_target(asf::create_console_log_target(stderr));
    //asr::global_logger().add_target(log_target.get());

	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";
    this->project = asf::auto_release_ptr<asr::Project>(asr::ProjectFactory::create("test project"));
	this->project->set_path(outputPath.asChar());
	definedEntities.clear();
}

void AppleseedRenderer::definePreRender()
{
	// Create an default master assembly.
	// if we use per shape assemblys, it will only contain lights if we have any

	//if( this->masterAssembly.get() != NULL)
	//	this->masterAssembly.reset();

	//if( this->scene.get() != NULL)
	//	this->scene.release();

	//if( this->project.get() != NULL)
	//	this->project.release();
	this->project.reset();

	this->defineProject();
	this->defineConfig();

	this->masterAssembly = asr::AssemblyFactory::create(
			"assembly",
			asr::ParamArray());	
	// 
	// define default scene, mayabe this will change in future releases, but at the moment
	// we do not reuse the scene during a render sequence
	this->scene = asr::SceneFactory::create();

	//this->addDefaultMaterial();
	this->defineDefaultMaterial();

	// maybe enable this one if no lights in the scene and no mesh lights are in the scene and 
	// the defaultRenderGlobals switch is turned on, but it can be confusing.
	//this->defineDefaultLight();
}

void AppleseedRenderer::writeXML()
{
	logger.debug("AppleseedRenderer::writeXML");
	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";

	asr::ProjectFileWriter::write(this->project.ref(), outputPath.asChar());
}

void AppleseedRenderer::defineMesh(mtap_MayaObject *obj)
{}
void AppleseedRenderer::defineNurbsSurface(mtap_MayaObject *obj)
{}
void AppleseedRenderer::defineParticle(mtap_MayaObject *obj)
{}
void AppleseedRenderer::defineFluid(mtap_MayaObject *obj)
{}

void AppleseedRenderer::defineDefaultMaterial()
{
    // Create a color called "gray" and insert it into the assembly.
    static const float GrayReflectance[] = { 0.8f, 0.8f, 0.8f };
	this->masterAssembly->colors().insert(
        asr::ColorEntityFactory::create(
            "gray",
            asr::ParamArray()
                .insert("color_space", "srgb"),
            asr::ColorValueArray(3, GrayReflectance)));

    // Create a BRDF called "diffuse_gray_brdf" and insert it into the assembly.
    this->masterAssembly->bsdfs().insert(
        asr::LambertianBRDFFactory().create(
            "diffuse_gray_brdf",
            asr::ParamArray()
                .insert("reflectance", "gray")));

    // Create a physical surface shader and insert it into the assembly.
    this->masterAssembly->surface_shaders().insert(
        asr::PhysicalSurfaceShaderFactory().create(
            "physical_surface_shader",
            asr::ParamArray()));

    // Create a material called "gray_material" and insert it into the assembly.
    this->masterAssembly->materials().insert(
        asr::MaterialFactory::create(
            "gray_material",
            asr::ParamArray()
                .insert("surface_shader", "physical_surface_shader")
                .insert("bsdf", "diffuse_gray_brdf")));

}

void AppleseedRenderer::addDefaultMaterial()
{
    // Create a color called "gray" and insert it into the assembly.
    static const float GrayReflectance[] = { 0.8f, 0.8f, 0.8f };
	this->masterAssembly->colors().insert(
        asr::ColorEntityFactory::create(
            "gray",
            asr::ParamArray()
                .insert("color_space", "srgb"),
            asr::ColorValueArray(3, GrayReflectance)));

    // Create a BRDF called "diffuse_gray_brdf" and insert it into the assembly.
    this->masterAssembly->bsdfs().insert(
        asr::LambertianBRDFFactory().create(
            "diffuse_gray_brdf",
            asr::ParamArray()
                .insert("reflectance", "gray")));

    // Create a physical surface shader and insert it into the assembly.
    this->masterAssembly->surface_shaders().insert(
        asr::PhysicalSurfaceShaderFactory().create(
            "physical_surface_shader",
            asr::ParamArray()));

    // Create a material called "gray_material" and insert it into the assembly.
    this->masterAssembly->materials().insert(
        asr::MaterialFactory::create(
            "gray_material",
            asr::ParamArray()
                .insert("surface_shader", "physical_surface_shader")
                .insert("bsdf", "diffuse_gray_brdf")));

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

	asr::ParamArray params;
	logger.debug(MString("Now inserting file name: ") + fileTextureName);
	params.insert("filename", fileTextureName.asChar());      // OpenEXR only for now. The param is called filename but it can be a path
    params.insert("color_space", "srgb");					  // the color space the texture is in, often it's sRGB

    asf::auto_release_ptr<asr::Texture> textureElement(
        asr::DiskTexture2dFactory().create(
	    textureName.asChar(),
            params,
            this->project->get_search_paths()));    // the project holds a set of search paths to find textures and other assets
	this->scene->textures().insert(textureElement);

	asf::auto_release_ptr<asr::TextureInstance> tinst = asr::TextureInstanceFactory().create(
	   textureInstanceName.asChar(),
	   asr::ParamArray()
		   .insert("addressing_mode", "clamp")
		   .insert("filtering_mode", "bilinear"),
		   textureName.asChar());

	this->scene->texture_instances().insert(tinst);

	textureDefinition = textureInstanceName;

}

void AppleseedRenderer::addDefaultMaterial(asr::Assembly *assembly, asf::StringArray& materialNames)
{
	// if it is already defined, then ignore it
	asr::ColorEntity *col = assembly->colors().get_by_name("gray");
	if( col != NULL)
	{
		materialNames.push_back("gray_material");
		return;
	}
    // Create a color called "gray" and insert it into the assembly.
    static const float GrayReflectance[] = { 0.8f, 0.8f, 0.8f };
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
        asr::MaterialFactory::create(
            "gray_material",
            asr::ParamArray()
                .insert("surface_shader", "physical_surface_shader")
                .insert("bsdf", "diffuse_gray_brdf")));

	materialNames.push_back("gray_material");
}

void AppleseedRenderer::defineLights()
{
	// Create a color called "light_exitance" and insert it into the assembly.
	MStatus stat;

	for(int objId = 0; objId < this->mtap_scene->lightList.size(); objId++)
	{
		mtap_MayaObject *mlight = (mtap_MayaObject *)this->mtap_scene->lightList[objId];
		asf::Matrix4d appMatrix = asf::Matrix4d::identity();
		logger.debug(MString("Creating light: ") + mlight->shortName);
		MMatrix colMatrix = mlight->transformMatrices[0];
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		MFnLight lightFn(mlight->mobject, &stat);
		MColor color(1.0f,1.0f,1.0f);
		float intensity = 1.0f;
		if( !stat )
		{
			logger.error(MString("Could not get light info from ") + mlight->shortName);
			return;
		}
		getColor(MString("color"), lightFn, color);
		getFloat(MString("intensity"), lightFn, intensity);
		
		float apColor[3];
		apColor[0] = color.r;
		apColor[1] = color.g;
		apColor[2] = color.b;
		asr::ColorValueArray aLightColor(3, apColor);
		MString lightColorName = mlight->shortName + "_exitance";
		//this->defineColor(lightColorName, color, NULL);

		intensity *= 30.0f; // 30 is the default value in the example
		this->masterAssembly->colors().insert(
			asr::ColorEntityFactory::create(
				lightColorName.asChar(),
				asr::ParamArray()
					.insert("color_space", "srgb")
					.insert("multiplier", (MString("")+intensity).asChar()),
					aLightColor));

		
		// Create a point light called "light" and insert it into the assembly.
		if( mlight->mobject.hasFn(MFn::kSpotLight))
		{
			//inner_angle
			//outer_angle
			logger.debug(MString("Creating spotLight: ") + lightFn.name());
			float coneAngle = 45.0f;
			float penumbraAngle = 3.0f;
			getFloat(MString("coneAngle"), lightFn, coneAngle);
			getFloat(MString("penumbraAngle"), lightFn, penumbraAngle);
			coneAngle = RadToDeg(coneAngle);
			penumbraAngle = RadToDeg(penumbraAngle);

			logger.debug(MString("ConeAngle: ") + coneAngle);
			logger.debug(MString("penumbraAngle: ") + penumbraAngle);
			float inner_angle = coneAngle;
			float outer_angle = coneAngle + penumbraAngle;
			
			// spot light is pointing in -z, appleseeds spot light is pointing in y, at least until next update...
			// I create a rotation matrix for this case.
			asf::Matrix4d rotMatrix = asf::Matrix4d::rotation(asf::Vector3d(1.0, 0.0, 0.0), asf::deg_to_rad(-90.0));
			asf::Matrix4d finalMatrix = appMatrix * rotMatrix;

			asf::auto_release_ptr<asr::Light> light(
				asr::SpotLightFactory().create(
					mlight->shortName.asChar(),
					asr::ParamArray()
						.insert("exitance", lightColorName.asChar())
						.insert("inner_angle", (MString("") + inner_angle).asChar())
						.insert("outer_angle", (MString("") + outer_angle).asChar())
						));
			light->set_transform(asf::Transformd(finalMatrix));
			this->masterAssembly->lights().insert(light);
			
		}else{
			asf::auto_release_ptr<asr::Light> light(
				asr::PointLightFactory().create(
					mlight->shortName.asChar(),
					asr::ParamArray()
						.insert("exitance", lightColorName.asChar())));
			light->set_transform(asf::Transformd(appMatrix));
			this->masterAssembly->lights().insert(light);
		}
	}
}


void AppleseedRenderer::defineLights(std::vector<MayaObject *>& lightList)
{
	this->defineLights();
	return;
	// Create a color called "light_exitance" and insert it into the assembly.
	MStatus stat;

	for(int objId = 0; objId < lightList.size(); objId++)
	{
		mtap_MayaObject *mlight = (mtap_MayaObject *)lightList[objId];
		asf::Matrix4d appMatrix = asf::Matrix4d::identity();
		logger.debug(MString("Creating light: ") + mlight->shortName);
		MMatrix colMatrix = mlight->transformMatrices[0];
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		MFnLight lightFn(mlight->mobject, &stat);
		MColor color(1.0f,1.0f,1.0f);
		float intensity = 1.0f;
		if( !stat )
		{
			logger.error(MString("Could not get light info from ") + mlight->shortName);
			return;
		}
		getColor(MString("color"), lightFn, color);
		getFloat(MString("intensity"), lightFn, intensity);

		//asf::FloatArray lightColor(1.0f, 1.0f, 1.0f);
		
		float apColor[3];
		apColor[0] = color.r;
		apColor[1] = color.g;
		apColor[2] = color.b;
		asr::ColorValueArray aLightColor(3, apColor);
		MString lightColorName = mlight->shortName + "_exitance";
		intensity *= 30.0f; // 30 is the default value in the example
		this->masterAssembly->colors().insert(
			asr::ColorEntityFactory::create(
				lightColorName.asChar(),
				asr::ParamArray()
					.insert("color_space", "srgb")
					.insert("multiplier", (MString("")+intensity).asChar()),
					aLightColor));


		// Create a point light called "light" and insert it into the assembly.
		asf::auto_release_ptr<asr::Light> light(
			asr::PointLightFactory().create(
				mlight->shortName.asChar(),
				asr::ParamArray()
					.insert("exitance", lightColorName.asChar())));
		light->set_transform(asf::Transformd(appMatrix));
		this->masterAssembly->lights().insert(light);
	}
}

void AppleseedRenderer::defineDefaultLight()
{
	// default light will be created only if there are no other lights in the scene
	if( this->mtap_scene->lightList.size() > 0)
		return;
	if( !this->renderGlobals->createDefaultLight )
		return;
	
    // Create a color called "light_exitance" and insert it into the assembly.
    static const float LightExitance[] = { 1.0f, 1.0f, 1.0f };
	this->masterAssembly->colors().insert(
        asr::ColorEntityFactory::create(
            "light_exitance",
            asr::ParamArray()
                .insert("color_space", "srgb")
                .insert("multiplier", "30.0"),
            asr::ColorValueArray(3, LightExitance)));

    // Create a point light called "light" and insert it into the assembly.
    asf::auto_release_ptr<asr::Light> light(
        asr::PointLightFactory().create(
            "light",
            asr::ParamArray()
                .insert("exitance", "light_exitance")));
    light->set_transform(asf::Transformd(
        asf::Matrix4d::translation(asf::Vector3d(0.6, 2.0, 1.0))));
    this->masterAssembly->lights().insert(light);
}

void AppleseedRenderer::defineConfig()
{
	logger.debug("AppleseedRenderer::defineConfig");
	
    // Add default configurations to the project.
    project->add_default_configurations();
	
	MString minSamples = MString("") + renderGlobals->minSamples;
	MString maxSamples = MString("") + renderGlobals->maxSamples;
	MString numThreads = MString("") + renderGlobals->threads;
	MString maxTraceDepth = MString("") + renderGlobals->maxTraceDepth;
	MString lightingEngine = "pt";
	lightingEngine = renderGlobals->lightingEngine == 0 ? lightingEngine : MString("drt");
	
	logger.debug(MString("Samples min: ") + minSamples + " max: " + maxSamples);
	MString directLightSamples = MString("") + renderGlobals->directLightSamples;

	this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path("generic_tile_renderer.min_samples", minSamples.asChar())
        .insert_path("generic_tile_renderer.max_samples", maxSamples.asChar())
		.insert_path("generic_frame_renderer.rendering_threads", numThreads.asChar())
		.insert_path("lighting_engine", lightingEngine.asChar())
		.insert_path((lightingEngine + ".max_path_length").asChar(), maxTraceDepth.asChar());
	if( renderGlobals->directLightSamples > 0)
	{
		this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path((lightingEngine + ".dl_light_samples").asChar(), directLightSamples.asChar());
	}
}

void AppleseedRenderer::defineOutput()
{
	logger.debug("AppleseedRenderer::defineOutput");
    // Create a frame and bind it to the project.
	MString res = MString("") + renderGlobals->imgWidth + " " + renderGlobals->imgHeight;
	MString colorSpaceString = renderGlobals->colorSpaceString;
	MString tileSize =  MString("") + renderGlobals->tilesize + " " + renderGlobals->tilesize;
	
    this->project->set_frame( 
        asr::FrameFactory::create(
            "beauty",
            asr::ParamArray()
                .insert("camera",this->scene->get_camera()->get_name())
				.insert("resolution", res.asChar())
				.insert("tile_size", tileSize.asChar())
				.insert("color_space", colorSpaceString.asChar())));

}

void AppleseedRenderer::defineMeshDeformStep(mtap_MayaObject *obj)
{
	logger.debug(MString("Defining mesh deform step at time: ") + this->renderGlobals->currentFrameNumber);	
	if( this->renderGlobals->isMbStartStep)
	{
		// if it is the start of rendering obj assembly has to be always zero
		if( obj->objectAssembly.get() != NULL)
			obj->objectAssembly.reset();

		if( obj->objectAssembly.get() == NULL)
		{
			logger.debug(MString("Is mb deform start step and object has not been defined yet, calling defineObject."));
			this->defineObject(obj);
		}
		return;
	}

	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(obj->mobject, &stat);
	CHECK_MSTATUS(stat);
	if( !stat )
		return;
	MItMeshPolygon faceIt(obj->mobject, &stat);
	CHECK_MSTATUS(stat);
	if( !stat )
		return;

	MPointArray points;
	stat = meshFn.getPoints(points);
	CHECK_MSTATUS(stat);
    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );

	logger.debug(MString("Defining deform step for mesh object ") + meshFn.name().asChar());
	
	asr::Assembly *assembly = this->masterAssembly.get();
	if( obj->objectAssembly.get() != NULL)
		assembly = obj->objectAssembly.get();

	if( assembly == NULL)
	{
		logger.debug("No valid master or object assembly found.");
		return;
	}

	logger.debug(MString("Found objectAssembly called ") +  assembly->get_name());
	asr::Object *meshObject = assembly->objects().get_by_name(obj->shortName.asChar());
	if( meshObject == NULL)
	{
		logger.debug("could not find base mesh in assembly");
		return;
	}
	asr::MeshObject *meshShape = (asr::MeshObject *)meshObject;
	MString meshName = meshShape->get_name();
	int numMbSegments = meshShape->get_motion_segment_count();
	logger.debug(MString("Mesh name ") + meshName + " has " + numMbSegments + " motion segments");

	// now adding one motion segment
	size_t mbSegmentIndex = meshShape->get_motion_segment_count() + 1;
	meshShape->set_motion_segment_count(mbSegmentIndex);

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		meshShape->set_vertex_pose(vtxId, mbSegmentIndex - 1, vtx);
	}
}

asf::auto_release_ptr<asr::MeshObject> AppleseedRenderer::createMesh(MObject& meshObject)
{

	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);
	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	meshFn.getPoints(points);
    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );
	MFloatArray uArray, vArray;
	meshFn.getUVs(uArray, vArray);

	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
    // Create a new mesh object.
	asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFn.name().asChar(), asr::ParamArray());

	// add vertices
    // Vertices.
    //object->push_vertex(GVector3(552.8f, 0.0f,   0.0f));
	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		mesh->push_vertex(vtx);
	}

	// add normals
    // Vertex normals.
    //object->push_vertex_normal(GVector3(0.0f, 1.0f, 0.0f));
	for( uint nId = 0; nId < normals.length(); nId++)
	{
		asr::GVector3 vtx((float)normals[nId].x, (float)normals[nId].y, (float)normals[nId].z);
		mesh->push_vertex_normal(vtx);
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		asr::GVector2 vtx((float)uArray[tId], (float)vArray[tId]);
		mesh->push_tex_coords(vtx);
	}

   
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;

	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		int faceId = faceIt.index();
		int numTris;
		faceIt.numTriangles(numTris);
		faceIt.getVertices(faceVtxIds);
		
		MIntArray faceUVIndices;

		faceNormalIds.clear();
		for( uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
		{
			faceNormalIds.append(faceIt.normalIndex(vtxId));
			int uvIndex;
			faceIt.getUVIndex(vtxId, uvIndex);
			faceUVIndices.append(uvIndex);
		}

		for( int triId = 0; triId < numTris; triId++)
		{
			int faceRelIds[3];
			faceIt.getTriangle(triId, triPoints, triVtxIds);

			for( uint triVtxId = 0; triVtxId < 3; triVtxId++)
			{
				for(uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
				{
					if( faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
					{
						faceRelIds[triVtxId] = faceVtxId;
					}
				}
			}

			
			uint vtxId0 = faceVtxIds[faceRelIds[0]];
			uint vtxId1 = faceVtxIds[faceRelIds[1]];
			uint vtxId2 = faceVtxIds[faceRelIds[2]];
			uint normalId0 = faceNormalIds[faceRelIds[0]];
			uint normalId1 = faceNormalIds[faceRelIds[1]];
			uint normalId2 = faceNormalIds[faceRelIds[2]];
			uint uvId0 = faceUVIndices[faceRelIds[0]];
			uint uvId1 = faceUVIndices[faceRelIds[1]];
			uint uvId2 = faceUVIndices[faceRelIds[2]];

			mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, 0));
		}		
	}

	return mesh;
}


asr::MeshObject *AppleseedRenderer::createMeshPtr(MObject& meshObject)
{

	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);
	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	meshFn.getPoints(points);
    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );
	MFloatArray uArray, vArray;
	meshFn.getUVs(uArray, vArray);

	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
    // Create a new mesh object.
	//asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFn.name().asChar(), asr::ParamArray());
	asr::MeshObject *mesh = asr::MeshObjectFactory::create(meshFn.name().asChar(), asr::ParamArray()).release();

	// add vertices
    // Vertices.
    //object->push_vertex(GVector3(552.8f, 0.0f,   0.0f));
	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		mesh->push_vertex(vtx);
	}

	// add normals
    // Vertex normals.
    //object->push_vertex_normal(GVector3(0.0f, 1.0f, 0.0f));
	for( uint nId = 0; nId < normals.length(); nId++)
	{
		asr::GVector3 vtx((float)normals[nId].x, (float)normals[nId].y, (float)normals[nId].z);
		mesh->push_vertex_normal(vtx);
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		asr::GVector2 vtx((float)uArray[tId], (float)vArray[tId]);
		mesh->push_tex_coords(vtx);
	}

   
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;

	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		int faceId = faceIt.index();
		int numTris;
		faceIt.numTriangles(numTris);
		faceIt.getVertices(faceVtxIds);
		
		MIntArray faceUVIndices;

		faceNormalIds.clear();
		for( uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
		{
			faceNormalIds.append(faceIt.normalIndex(vtxId));
			int uvIndex;
			faceIt.getUVIndex(vtxId, uvIndex);
			faceUVIndices.append(uvIndex);
		}

		for( int triId = 0; triId < numTris; triId++)
		{
			int faceRelIds[3];
			faceIt.getTriangle(triId, triPoints, triVtxIds);

			for( uint triVtxId = 0; triVtxId < 3; triVtxId++)
			{
				for(uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
				{
					if( faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
					{
						faceRelIds[triVtxId] = faceVtxId;
					}
				}
			}

			
			uint vtxId0 = faceVtxIds[faceRelIds[0]];
			uint vtxId1 = faceVtxIds[faceRelIds[1]];
			uint vtxId2 = faceVtxIds[faceRelIds[2]];
			uint normalId0 = faceNormalIds[faceRelIds[0]];
			uint normalId1 = faceNormalIds[faceRelIds[1]];
			uint normalId2 = faceNormalIds[faceRelIds[2]];
			uint uvId0 = faceUVIndices[faceRelIds[0]];
			uint uvId1 = faceUVIndices[faceRelIds[1]];
			uint uvId2 = faceUVIndices[faceRelIds[2]];

			//mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2,  0));
			mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, 0));
		}		
	}

	return mesh;
}

// 
//	Same as with materials: Every render sequence will define the colors and textures new
//	because I cannot be sure that nothing has changed. This means as soon as a rendering will
//	start, all existing colors and textures in an existing scene will be removed, and then new defined.
//	For this function here, it means I can reuse an existing color because its up to date.
//
void AppleseedRenderer::defineColor(MString& name, MColor& color, asr::Assembly *assembly, float intensity)
{
	float colorDef[3];
	colorDef[0] = color.r;
	colorDef[1] = color.g;
	colorDef[2] = color.b;
	float alpha = color.a;

	asf::auto_release_ptr<asr::ColorEntity> colorEntity;
	
	asr::Entity *entity;
	if( assembly != NULL)
		entity = assembly->colors().get_by_name(name.asChar());
	else
		entity = scene->colors().get_by_name(name.asChar());

	if( entity != NULL)
	{
		//assembly->colors().remove(entity);
		return;
	}

	MString intensityString = MString("") + intensity;
	colorEntity = asr::ColorEntityFactory::create(
				name.asChar(),
				asr::ParamArray()
					.insert("color_space", "srgb")
					.insert("multiplier", intensityString.asChar()),
				asr::ColorValueArray(3, colorDef),
				asr::ColorValueArray(1, &alpha));
	if( assembly != NULL)
	{
		if( assembly->colors().get_by_name(name.asChar()) == NULL)
			assembly->colors().insert(colorEntity);
	}else{
		if( this->scene->colors().get_by_name(name.asChar()) == NULL)
			this->scene->colors().insert(colorEntity);
	}
}


#define isBlack(x) ((x.r + x.g + x.b) <= 0.0f)
// translate shaders here, will be seperated later if we have a real shading language
void AppleseedRenderer::defineObjectMaterial(mtap_RenderGlobals *renderGlobals, mtap_MayaObject *obj, asf::StringArray& materialNames)
{
	bool createAssembly = (renderGlobals->assemblyExportType == 1);
	asr::Assembly *assembly = this->masterAssembly.get();
	if( createAssembly )
		assembly = obj->objectAssembly.get();

	MObject shadingGroup;
	getObjectShadingGroups(obj->dagPath, shadingGroup);
	
	MObject surfaceShaderNode = getOtherSideNode(MString("surfaceShader"), shadingGroup);
	if( surfaceShaderNode == MObject::kNullObj)
		return;

	MFnDependencyNode shadingGroupNode(shadingGroup);
	MString materialName = shadingGroupNode.name();

	// here I assume that all colors, textures are redefined for every new rendersequence.
	// If they already exist, I can reuse the materials because the objects are assigned to
	// the same shading group. Of course this can be the case only if the assembly is a master assembly.
	if( assembly->materials().get_by_name(materialName.asChar()) != NULL)
	{
		materialNames.push_back(materialName.asChar());
		return;		
	}

	MFn::Type shaderType = surfaceShaderNode.apiType();
	MFnDependencyNode depFn(surfaceShaderNode);

	//materialName.asChar(),
	int type_id = depFn.typeId().id();
	if( type_id ==  0x00106EF4)
	{
		MFnDependencyNode shaderNode(surfaceShaderNode);
		logger.debug(MString("Translating appleseedSurface Shader: ") + shaderNode.name());

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
		getInt(MString("shaderType"), shaderNode, shaderType);
		MString shaderName = shaderNode.name();
		asf::auto_release_ptr<asr::BSDF> bsdf, bsdfFront, bsdfBack;
		
		float transmittanceMultiplier = 1.0f;
		float from_ior = 1.0f;
		float to_ior = 1.0f;
		MColor transmittance(1.0f,1.0f,1.0f);
		MString transmittanceName = shaderNode.name() + "_transmittance";
		bool doubleSided = false;

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

		//assembly->materials().insert(
		//		asr::MaterialFactory::create(
		//		materialName.asChar(),
		//		asr::ParamArray()
		//			.insert("surface_shader", surfaceShaderNode.asChar())
		//			.insert("bsdf", shaderName.asChar())));
		assembly->materials().insert(
				asr::MaterialFactory::create(
				materialName.asChar(),
				materialParams));

		materialNames.push_back(materialName.asChar());

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
			materialNames.push_back((materialName + "_back").asChar());
		}

		return;
	}


	if( shaderType == MFn::kLambert)
	{
		MFnDependencyNode shaderNode(surfaceShaderNode);
		logger.debug(MString("Translating lambert shader: ") + shaderNode.name());
		MColor color(1.0f,1.0f,1.0f);
		getColor(MString("color"), shaderNode, color);

		MString matteRefName = shaderNode.name() + "_matteRefl";
		this->defineColor(matteRefName, color, assembly);
		this->defineTexture(shaderNode, MString("color"), matteRefName);

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
		MString shaderName = shaderNode.name();
		
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
				asr::ParamArray());

		assembly->surface_shaders().insert(lambertSurfaceShader);

		MFnDependencyNode shadingGroupNode(shadingGroup);
		MString materialName = shadingGroupNode.name();

		entity = assembly->materials().get_by_name(materialName.asChar());
		if( entity != NULL)
			assembly->materials().remove(entity);

		if(edf_name.length() == 0)
		{
			assembly->materials().insert(
				asr::MaterialFactory::create(
					materialName.asChar(),
					asr::ParamArray()
						.insert("surface_shader", surfaceShaderNode.asChar())
						.insert("bsdf", shaderName.asChar())));
		}else{
			assembly->materials().insert(
				asr::MaterialFactory::create(
					materialName.asChar(),
					asr::ParamArray()
						.insert("surface_shader", surfaceShaderNode.asChar())
						.insert("bsdf", shaderName.asChar())
						.insert("edf", edf_name.asChar())));
		}
		materialNames.push_back(materialName.asChar());

		return;
	}
	if( shaderType == MFn::kPhong)
	{
		MFnDependencyNode shaderNode(surfaceShaderNode);
		logger.debug(MString("Translating phong shader: ") + shaderNode.name());
		MColor color(1.0f,1.0f,1.0f);
		getColor(MString("color"), shaderNode, color);

		MString matteRefName = shaderNode.name() + "_matteRefl";
		this->defineColor(matteRefName, color, assembly);

		asf::auto_release_ptr<asr::BSDF> phongShader;
		MString shaderName = shaderNode.name();
		phongShader = asr::SpecularBRDFFactory().create(
			shaderName.asChar(),
			asr::ParamArray()
				.insert("reflectance", matteRefName.asChar()));

		assembly->bsdfs().insert(phongShader);

		MString surfaceShaderNode = shaderName + "phys_surf";
		asf::auto_release_ptr<asr::SurfaceShader> phongSurfaceShader;
		phongSurfaceShader = asr::PhysicalSurfaceShaderFactory().create(
				surfaceShaderNode.asChar(),
				asr::ParamArray());

		assembly->surface_shaders().insert(phongSurfaceShader);

		MFnDependencyNode shadingGroupNode(shadingGroup);
		MString materialName = shadingGroupNode.name();

		assembly->materials().insert(
			asr::MaterialFactory::create(
				materialName.asChar(),
				asr::ParamArray()
					.insert("surface_shader", surfaceShaderNode.asChar())
					.insert("bsdf", shaderName.asChar())));

		materialNames.push_back(materialName.asChar());
		return;
	}	

	this->addDefaultMaterial(assembly, materialNames);
}

void AppleseedRenderer::defineObject(mtap_MayaObject *obj)
{
	logger.debug(MString("asr define obj ") + obj->shortName);

	asf::StringArray material_names;
	material_names.push_back("gray_material");

	bool createAssembly = (renderGlobals->assemblyExportType == 1); // per shape?

	// only create a mesh description for the original shape
	asf::auto_release_ptr<asr::MeshObject> mesh;

	MString meshName(obj->shortName);
	mesh = this->createMesh(obj->mobject);
	asr::Assembly *assembly = this->masterAssembly.get();

	// only create an assembly for the original object instances will be assembly instances
	if( createAssembly)
	{
		obj->objectAssembly = asr::AssemblyFactory::create(
			(MString("shapeAssembly") + obj->shortName).asChar(),
			asr::ParamArray());
		assembly = obj->objectAssembly.get();
	}

	assembly->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));

	asr::Object *meshObject = assembly->objects().get_by_name(obj->shortName.asChar());

	asf::Matrix4d tmatrix = asf::Matrix4d::identity();

	// if we have per object assemblies, the assemblies will inherit the transformation matrix,
	// this is not the case if we have one master assembly, then we have all elements in one assembly
	if( !createAssembly)
		this->MMatrixToAMatrix(obj->transformMatrices[0], tmatrix);
	//logger.debug(MString("TransMatrix obj ") + obj->shortName + " tx: " + tmatrix[3]);

	material_names.clear();
	this->defineObjectMaterial(renderGlobals, obj, material_names);
	asf::StringArray backSideMaterial_names = material_names;

	if( material_names.size() > 1)
	{
		MString mn = material_names[0];
		material_names.clear();
		material_names.push_back(mn.asChar());
		mn = backSideMaterial_names[0];
		backSideMaterial_names.clear();
		backSideMaterial_names.push_back(mn.asChar());
	}

	bool doubleSided = true;
	MFnDependencyNode depFn(obj->mobject);
	getBool(MString("doubleSided"), depFn, doubleSided);

	//TODO: assign object materials 2 sided only for non light objects
	if( doubleSided )
	{
		assembly->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			((meshName + "_inst" + obj->instanceNumber).asChar()),
			asr::ParamArray(),
			*meshObject,
			asf::Transformd(tmatrix),
			material_names,
			backSideMaterial_names));
	}else{
		assembly->object_instances().insert(
			asr::ObjectInstanceFactory::create(
			((meshName + "_inst" + obj->instanceNumber).asChar()),
			asr::ParamArray(),
			*meshObject,
			asf::Transformd(tmatrix),
			material_names));
	}
}

void AppleseedRenderer::defineGeometry()
{
	// Create the array of material names.
	asf::StringArray material_names;
	material_names.push_back("gray_material");

	bool createAssembly = (renderGlobals->assemblyExportType == 1);

	for(int objId = 0; objId < this->mtap_scene->objectList.size(); objId++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)this->mtap_scene->objectList[objId];

		// only meshes
		logger.debug(MString("Translating object ") + obj->shortName);
		if( !obj->mobject.hasFn(MFn::kMesh) )
		{
			logger.debug("Object is not a mesh, igoring");
			continue;
		}

		// if we create an assembly per geometry, then all instances will be created later with assembly instances
		if( createAssembly && (obj->instanceNumber > 0))
		{
			logger.debug("Object is instance, no direct mesh creation, trying to add an obj/assembly instance");
			continue;
		}

		// only create a mesh description for the original shape
		asf::auto_release_ptr<asr::MeshObject> mesh;
		if( obj->instanceNumber == 0)		
			mesh = this->createMesh(obj->mobject);

		MString meshName(obj->shortName);
		asr::Assembly *assembly = this->masterAssembly.get();

		// only create an assembly for the original object instances will be assembly instances
		if( createAssembly)
		{
			asf::auto_release_ptr<asr::Assembly> testAss;
			testAss = asr::AssemblyFactory::create(
				(MString("shapeAssembly") + obj->shortName).asChar(),
				asr::ParamArray());
			//obj->objectAssembly = asr::AssemblyFactory::create(
			//	(MString("shapeAssembly") + obj->shortName).asChar(),
			//	asr::ParamArray());
			obj->objectAssembly.reset();
			obj->objectAssembly = testAss;
			assembly = obj->objectAssembly.get();
		}

		if( obj->instanceNumber == 0)
			assembly->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));

		asr::Object *meshObject = assembly->objects().get_by_name(obj->shortName.asChar());

		asf::Matrix4d tmatrix = asf::Matrix4d::identity();

		// if we have per object assemblies, the assemblies will be inherit the transformation matrix,
		// this is not the case if we have one master assembly, then we have all elements in one assembly
		if( !createAssembly)
			this->MMatrixToAMatrix(obj->transformMatrices[0], tmatrix);
		logger.debug(MString("TransMatrix obj ") + obj->shortName + " tx: " + tmatrix[3]);
	    // Create an instance of this object and insert it into the assembly.
		//if( obj->instanceNumber == 0)

		material_names.clear();
		this->defineObjectMaterial(renderGlobals, obj, material_names);
		asf::StringArray backSideMaterial_names = material_names;

		if( material_names.size() > 1)
		{
			MString mn = material_names[0];
			material_names.clear();
			material_names.push_back(mn.asChar());
			mn = backSideMaterial_names[0];
			backSideMaterial_names.clear();
			backSideMaterial_names.push_back(mn.asChar());
		}

		bool doubleSided = true;
		MFnDependencyNode depFn(obj->mobject);
		getBool(MString("doubleSided"), depFn, doubleSided);

		//TODO: assign object materials 2 sided only for non light objects
		if( doubleSided )
		{
			assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				((meshName + "_inst" + obj->instanceNumber).asChar()),
				asr::ParamArray(),
				*meshObject,
				asf::Transformd(tmatrix),
				material_names,
				backSideMaterial_names));
		}else{
			assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				((meshName + "_inst" + obj->instanceNumber).asChar()),
				asr::ParamArray(),
				*meshObject,
				asf::Transformd(tmatrix),
				material_names));
		}
	}
}


void AppleseedRenderer::defineGeometry(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList)
{
	this->defineGeometry();
	return;
	// Create the array of material names.
	asf::StringArray material_names;
	material_names.push_back("gray_material");

	bool createAssembly = (renderGlobals->assemblyExportType == 1);

	for(int objId = 0; objId < objectList.size(); objId++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)objectList[objId];

		// only meshes
		logger.debug(MString("Translating object ") + obj->shortName);
		if( !obj->mobject.hasFn(MFn::kMesh) )
		{
			logger.debug("Object is not a mesh, igoring");
			continue;
		}

		// if we create an assembly per geometry, then all instances will be created later with assembly instances
		if( createAssembly && (obj->instanceNumber > 0))
		{
			logger.debug("Object is instance, no direct mesh creation, trying to add an obj/assembly instance");
			continue;
		}

		// only create a mesh description for the original shape
		asf::auto_release_ptr<asr::MeshObject> mesh;
		if( obj->instanceNumber == 0)		
			mesh = this->createMesh(obj->mobject);

		MString meshName(obj->shortName);
		asr::Assembly *assembly = this->masterAssembly.get();

		// only create an assembly for the original object instances will be assembly instances
		if( createAssembly)
		{
			asf::auto_release_ptr<asr::Assembly> testAss;
			testAss = asr::AssemblyFactory::create(
				(MString("shapeAssembly") + obj->shortName).asChar(),
				asr::ParamArray());
			//obj->objectAssembly = asr::AssemblyFactory::create(
			//	(MString("shapeAssembly") + obj->shortName).asChar(),
			//	asr::ParamArray());
			obj->objectAssembly.reset();
			obj->objectAssembly = testAss;
			assembly = obj->objectAssembly.get();
		}

		if( obj->instanceNumber == 0)
			assembly->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));

		asr::Object *meshObject = assembly->objects().get_by_name(obj->shortName.asChar());

		asf::Matrix4d tmatrix = asf::Matrix4d::identity();

		// if we have per object assemblies, the assemblies will be inherit the transformation matrix,
		// this is not the case if we have one master assembly, then we have all elements in one assembly
		if( !createAssembly)
			this->MMatrixToAMatrix(obj->transformMatrices[0], tmatrix);
		logger.debug(MString("TransMatrix obj ") + obj->shortName + " tx: " + tmatrix[3]);
	    // Create an instance of this object and insert it into the assembly.
		//if( obj->instanceNumber == 0)

		material_names.clear();
		this->defineObjectMaterial(renderGlobals, obj, material_names);
		asf::StringArray backSideMaterial_names = material_names;

		if( material_names.size() > 1)
		{
			MString mn = material_names[0];
			material_names.clear();
			material_names.push_back(mn.asChar());
			mn = backSideMaterial_names[0];
			backSideMaterial_names.clear();
			backSideMaterial_names.push_back(mn.asChar());
		}

		bool doubleSided = true;
		MFnDependencyNode depFn(obj->mobject);
		getBool(MString("doubleSided"), depFn, doubleSided);

		//TODO: assign object materials 2 sided only for non light objects
		if( doubleSided )
		{
			assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				((meshName + "_inst" + obj->instanceNumber).asChar()),
				asr::ParamArray(),
				*meshObject,
				asf::Transformd(tmatrix),
				material_names,
				backSideMaterial_names));
		}else{
			assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				((meshName + "_inst" + obj->instanceNumber).asChar()),
				asr::ParamArray(),
				*meshObject,
				asf::Transformd(tmatrix),
				material_names));
		}
	}
}

void AppleseedRenderer::defineEnvironment(mtap_RenderGlobals *renderGlobals)
{
 //------------------------------------------------------------------------
    // Environment
    //------------------------------------------------------------------------

	//stat = eAttr.addField( "Constant", 0 );
	//stat = eAttr.addField( "Gradient", 1 );
	//stat = eAttr.addField( "Latitude Longitude", 2 );
	//stat = eAttr.addField( "Mirror Ball", 3 );

	MString textInstName = "bg_texture_inst";
	renderGlobals->environmentColor.a = 0.0f;
	renderGlobals->gradientHorizon.a = 0.0f;
	renderGlobals->gradientZenit.a = 0.0f;
	MString envName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";
	this->defineColor(envName, renderGlobals->environmentColor, NULL, renderGlobals->environmentIntensity);
	this->defineColor(gradHorizName, renderGlobals->gradientHorizon, NULL, renderGlobals->environmentIntensity);
	this->defineColor(gradZenitName, renderGlobals->gradientZenit, NULL, renderGlobals->environmentIntensity);
	this->defineColor(envMapName, renderGlobals->environmentMap, NULL, renderGlobals->environmentIntensity);
	MFnDependencyNode globalsFn(renderGlobals->renderGlobalsMobject);
	MString envMapAttrName = envMapName;
	this->defineTexture(globalsFn, envMapAttrName, envMapName);

	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF;

	switch(renderGlobals->environmentType){
	case 0:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envName));
		break;
	case 1:
		environmentEDF = asr::GradientEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("horizon_exitance", gradHorizName)
				.insert("zenith_exitance", gradZenitName)
				);
		break;
	case 2:
	    environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar())
				.insert("exitance_multiplier", (MString("")+renderGlobals->environmentIntensity).asChar()));
		break;
	case 3:
		environmentEDF = asr::MirrorBallMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar())
				.insert("exitance_multiplier", (MString("")+renderGlobals->environmentIntensity).asChar()));
		break;
	default:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envName));
	}
	this->scene->environment_edfs().insert(environmentEDF);


    // Create an environment shader called "sky_shader" and insert it into the scene.
    this->scene->environment_shaders().insert(
        asr::EDFEnvironmentShaderFactory().create(
            "sky_shader",
            asr::ParamArray()
                .insert("environment_edf", "sky_edf")));

    // Create an environment called "sky" and bind it to the scene.
    this->scene->set_environment(
        asr::EnvironmentFactory::create(
            "sky",
            asr::ParamArray()
                .insert("environment_edf", "sky_edf")
                .insert("environment_shader", "sky_shader")));	

}

// 
//
//

bool AppleseedRenderer::initializeRenderer(mtap_RenderGlobals *renderGlobals,  std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList)
{
	//this->defineProject();
	//this->defineConfig();

	//this->defineGeometry(renderGlobals, objectList);

	//this->defineLights(lightList);

	return true;
}

void AppleseedRenderer::defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList, std::vector<MayaObject *>& instancerList)
{
	logger.debug("AppleseedRenderer::defineScene");

    // Create an instance of the assembly and insert it into the scene.

    this->scene->assembly_instances().insert(
        asr::AssemblyInstanceFactory::create(
            "master_assembly_inst",
            asr::ParamArray(),
            *this->masterAssembly,
            asf::Transformd(asf::Matrix4d::identity())));

    // Insert the assembly into the scene.
    this->scene->assemblies().insert(this->masterAssembly);

	// per object assembly
	// lets go through all objects and check if they are instance objects.
	// If yes, then get the original shape and read the assembly of the original.
	if( renderGlobals->assemblyExportType == 1)
	{
		for(size_t objId = 0; objId < objectList.size(); objId++)
		{
			mtap_MayaObject *obj = (mtap_MayaObject *)objectList[objId];
			if( !obj->geometryShapeSupported())
				continue;
			asf::Matrix4d tmatrix = asf::Matrix4d::identity();
			this->MMatrixToAMatrix(obj->transformMatrices[0], tmatrix);

			if( obj->instanceNumber == 0)
			{
				asr::Assembly *assembly = obj->objectAssembly.get();
				logger.debug(MString("Adding orig geo assembly for ") + obj->shortName);
				this->scene->assembly_instances().insert(
					asr::AssemblyInstanceFactory::create(
					(MString("assembly_inst_") + obj->shortName + obj->instanceNumber).asChar(),
						asr::ParamArray(),
						*assembly,
						asf::Transformd(tmatrix)));
				this->scene->assemblies().insert((asf::auto_release_ptr<asr::Assembly>)assembly);

				continue;
			}

			if( obj->origObject == NULL)
				continue;
			logger.debug(MString("Found instance for ") + obj->shortName);
			mtap_MayaObject *origObj = (mtap_MayaObject *)obj->origObject;
			asr::Assembly *assembly = origObj->objectAssembly.get();
		    this->scene->assembly_instances().insert(
		        asr::AssemblyInstanceFactory::create(
					(MString("assembly_inst_") + obj->shortName + obj->instanceNumber).asChar(),
				    asr::ParamArray(),
					*assembly,
					asf::Transformd(tmatrix)));

		}
		for(size_t objId = 0; objId < instancerList.size(); objId++)
		{
			mtap_MayaObject *obj = (mtap_MayaObject *)instancerList[objId];
			if( obj->origObject == NULL)
				continue;
			logger.debug(MString("Found instance for instancerElement ") + obj->shortName);
			mtap_MayaObject *origObj = (mtap_MayaObject *)obj->origObject;
			asr::Assembly *assembly = origObj->objectAssembly.get();
			asf::Matrix4d tmatrix = asf::Matrix4d::identity();
			this->MMatrixToAMatrix(obj->transformMatrices[0], tmatrix);

		    this->scene->assembly_instances().insert(
		        asr::AssemblyInstanceFactory::create(
					(MString("assembly_instancer_") + obj->shortName + obj->instanceNumber).asChar(),
				    asr::ParamArray(),
					*assembly,
					asf::Transformd(tmatrix)));
		}
	}

    // Create a pinhole camera with film dimensions 0.980 x 0.735 in (24.892 x 18.669 mm).
    //asf::auto_release_ptr<asr::Camera> camera(
    //    asr::PinholeCameraFactory().create(
    //        "camera",
    //        asr::ParamArray()
    //            .insert("film_dimensions", "0.024892 0.018669")
    //            .insert("focal_length", "0.035")));
	this->defineCamera(camList, renderGlobals);
    // Place and orient the camera. By default cameras are located in (0.0, 0.0, 0.0)
    // and are looking toward Z- (0.0, 0.0, -1.0).
    //camera->transform_sequence().set_transform(
    //    0.0,
    //    asf::Transformd(
    //        asf::Matrix4d::rotation(asf::Vector3d(1.0, 0.0, 0.0), asf::deg_to_rad(-20.0)) *
    //        asf::Matrix4d::translation(asf::Vector3d(0.0, 0.8, 11.0))));

    // Bind the camera to the scene.
    this->scene->set_camera(camera);

	this->defineOutput();
    // Create a frame and bind it to the project.
    //project->set_frame(
    //    asr::FrameFactory::create(
    //        "beauty",
    //        asr::ParamArray()
    //            .insert("camera", scene->get_camera()->get_name())
    //            .insert("resolution", "768 415")
    //            .insert("color_space", "srgb")));

    // Create an environment and bind it to the scene.
	this->defineEnvironment(renderGlobals);

    //this->scene->set_environment(
    //    asr::EnvironmentFactory::create(
    //        "environment",
    //        asr::ParamArray()));

    // Bind the scene to the project.
    this->project->set_scene(this->scene);


	//this->scene = asr::SceneFactory::create();
	//this->masterAssembly = asr::AssemblyFactory::create("Master_Assembly", asr::ParamArray());

	//this->defineConfig(renderGlobals);
	//this->defineCamera(camList, renderGlobals);
	//this->addDefaultMaterial();
	//this->defineGeometry(objectList);
	//this->defineLights(lightList);
 //   
 //   // Create an instance of the assembly and insert it into the scene.
 //   this->scene->assembly_instances().insert(
 //       asr::AssemblyInstanceFactory::create(
 //           "assembly_inst",
 //           asr::ParamArray(),
 //           *this->masterAssembly,
 //           asf::Transformd(asf::Matrix4d::identity())));

 //   // Insert the assembly into the scene.
 //   this->scene->assemblies().insert(this->masterAssembly);

 //   // Bind the camera to the scene.
	//this->scene->set_camera(this->camera);

	//this->defineOutput(renderGlobals);	

 //   this->project->set_scene(this->scene);

}

void AppleseedRenderer::MMatrixToAMatrix(MMatrix& mayaMatrix, asf::Matrix4d& appleMatrix)
{
	MMatrix rowMatrix;
	rowToColumn(mayaMatrix, rowMatrix);
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
}

//
//	Appleseed does not support more than one camera at the moment, so break after the first one.
//

void AppleseedRenderer::defineCamera(std::vector<MayaObject *>& cameraList, mtap_RenderGlobals *renderGlobals)
{
	MStatus stat;

	MayaObject *cam = NULL;
	// There is at least one camera
	for(int objId = 0; objId < cameraList.size(); objId++)
	{
		mtap_MayaObject *cam = (mtap_MayaObject *)cameraList[objId];
		logger.debug(MString("Creating camera: ") + cam->shortName);

		float horizontalFilmAperture = 24.892f;
		float verticalFilmAperture = 18.669f;
		float imageAspect = (float)renderGlobals->imgHeight / (float)renderGlobals->imgWidth;
		bool dof = false;
		float mtap_cameraType = 0;
		int mtap_diaphragm_blades = 0;
		float mtap_diaphragm_tilt_angle = 0.0;
		float focusDistance = 0.0;
		float fStop = 0.0;

		float focalLength = 35.0f;
		if( cam != NULL )
		{
			MFnCamera camFn(cam->mobject, &stat);
			if( stat == MStatus::kSuccess)
			{
				getFloat(MString("horizontalFilmAperture"), camFn, horizontalFilmAperture);
				getFloat(MString("verticalFilmAperture"), camFn, verticalFilmAperture);
				getFloat(MString("focalLength"), camFn, focalLength);
				getBool(MString("depthOfField"), camFn, dof);
				if( dof )
				{
					getFloat(MString("focusDistance"), camFn, focusDistance);
					getFloat(MString("fStop"), camFn, fStop);
					getInt(MString("mtap_diaphragm_blades"), camFn, mtap_diaphragm_blades);
					getFloat(MString("mtap_diaphragm_tilt_angle"), camFn, mtap_diaphragm_tilt_angle);
				}
			}		
		}

		// maya aperture is given in inces so convert to cm and convert to meters
		horizontalFilmAperture = horizontalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = verticalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = horizontalFilmAperture * imageAspect;
		MString filmBack = MString("") + horizontalFilmAperture + " " + verticalFilmAperture;
		MString focalLen = MString("") + focalLength * 0.001f;

		if( dof )
		{
			this->camera = asr::ThinLensCameraFactory().create(
					cam->shortName.asChar(),
					asr::ParamArray()
						.insert("film_dimensions", filmBack.asChar())
						.insert("focal_length", focalLen.asChar())
						.insert("focal_distance", (MString("") + focusDistance).asChar())
						.insert("f_stop",  (MString("") + fStop).asChar())
						.insert("diaphragm_blades",  (MString("") + mtap_diaphragm_blades).asChar())
						.insert("diaphragm_tilt_angle",  (MString("") + mtap_diaphragm_tilt_angle).asChar())
						);
		}else{
			this->camera = asr::PinholeCameraFactory().create(
					cam->shortName.asChar(),
					asr::ParamArray()
						.insert("film_dimensions", filmBack.asChar())
						.insert("focal_length", focalLen.asChar()));
		}

		asf::Matrix4d appMatrix;
		size_t numSteps =  cam->transformMatrices.size();
		size_t divSteps = numSteps;
		if( divSteps > 1)
			divSteps -= 1;
		//float stepSize = fabs((renderGlobals->mbEndTime - renderGlobals->mbStartTime) / (float)divSteps);
		float stepSize = 1.0f / (float)divSteps;
		float start = 0.0f;

		for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
		{
			MMatrix colMatrix = cam->transformMatrices[matrixId];
			this->MMatrixToAMatrix(colMatrix, appMatrix);

			camera->transform_sequence().set_transform(
				start + stepSize * matrixId,
				asf::Transformd(appMatrix));
		}
		break; // only one camera is supported at the moment
	}
}

//
//	Seperate thread simply waiting for a render interrupt event like pessing the 'esc' key
//

void eventListener()
{
    while(true) 
	{
        if(computation.isInterruptRequested()) 
			break ;
		boost::this_thread::sleep(boost::posix_time::milliseconds(10)); 
    }
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::INTERRUPT;
	EventQueue::theQueue()->push(e);
}

void renderStarter( asr::MasterRenderer *renderer)
{
	renderer->render();
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::RENDERDONE;
	EventQueue::theQueue()->push(e);
}

void AppleseedRenderer::render(mtap_RenderGlobals *renderGlobals)
{
	logger.debug("AppleseedRenderer::render");

	uint width = renderGlobals->imgWidth;
	uint height = renderGlobals->imgHeight;
	MStatus status = MRenderView::startRender(width,
											  height,
											  false,
											  true);

	mtap_IRendererController mtap_controller;	
	asr::DefaultRendererController renderer_controller;
	this->tileCallbackFac.reset(new mtap_ITileCallbackFactory());

	asr::MasterRenderer *renderer = NULL;

	bool renderViewExists = MRenderView::doesRenderEditorExist();
	renderer = new asr::MasterRenderer(
		this->project.ref(),
		this->project->configurations().get_by_name("final")->get_inherited_parameters(),
		&mtap_controller,
		this->tileCallbackFac.get());

	computation.beginComputation();

	boost::thread rendererThread(renderStarter, renderer); 

	// the events will contain interrupts, tile done, frame done etc. 
	// the event structure will eventually contain a pointer to a frame or a tile
	// this data will be created in the tile callback and has to be deleted as soon as 
	// they are not used any more
	float numTilesX = (float)width / (float)renderGlobals->tilesize;
	float numTilesY = (float)height / (float)renderGlobals->tilesize;
	int numTiles = (int)(numTilesX * numTilesY);
	int tileCount = 0;

	EventQueue::Event e;
	while(true)
	{
		EventQueue::theQueue()->wait_and_pop(e);
		if(e.type == EventQueue::Event::INTERRUPT)
		{
			logger.debug(MString("Interrupt called."));
			break;
		}
		if(e.type == EventQueue::Event::PRETILE)
		{
			//logger.debug(MString("PreTile called."));
			if( e.data != NULL)
			{
				//reverse tile position in image
				size_t miny = height - e.tile_ymax;
				size_t maxy = height - e.tile_ymin - 1;

				RV_PIXEL* pixels = (RV_PIXEL*)e.data;
				if( renderViewExists )
				{
					MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, miny, maxy, pixels);
					MRenderView::refresh(e.tile_xmin, e.tile_xmax, miny, maxy);
				}
				delete[] pixels;
			}
		}
		if(e.type == EventQueue::Event::RENDERDONE)
		{
			logger.debug(MString("Rendering done called."));
			break;
		}
		if(e.type == EventQueue::Event::TILEDONE)
		{
			//logger.debug(MString("Tile done called."));
			if( e.data != NULL)
			{
				if( ((tileCount++ % 20) == 0) || (tileCount == numTiles))
				{
					//logger.progress(MString("Tile ") + tileCount + " of " + numTiles + " done");
					int p =  (int)((float)tileCount/(float)numTiles * 100.0);
					MString perc =  MString("") + p;
					if( p < 10)
						perc = MString(" ") + perc;
					logger.progress(MString("") + perc + "% done");
				}
				RV_PIXEL* pixels = (RV_PIXEL*)e.data;
				if( renderViewExists )
				{
					MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, pixels);
					MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
				}
				delete[] pixels;
			}
		}
	}

	computation.endComputation();

	logger.debug(MString("EventQueue done."));
	
	rendererThread.join();
	
	MRenderView::endRender();

	renderGlobals->getImageName();
	logger.debug(MString("Current Image name: ") + renderGlobals->imageOutputFile);
	MString imageOutputFile =  renderGlobals->imageOutputFile;

	project->get_frame()->write(imageOutputFile.asChar());
}
