#include "appleseed.h"
#include "mtap_tileCallback.h"

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

#include "renderer/modeling/environmentedf/sphericalcoordinates.h"

#include "maya/MFnDependencyNode.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "threads/threads.h"
#include "threads/queue.h"
#include "utilities/pystring.h"
#include "../mtap_common/mtap_mayaScene.h"
#include "threads/renderQueueWorker.h"

#include <stdio.h>
#include <iostream>

#include "utilities/logging.h"

static Logging logger;

static int tileCount = 0;
static int tileCountTotal = 0;

using namespace AppleRender;

std::vector<asr::Entity *> definedEntities;

static AppleseedRenderer *appleRenderer = NULL;

AppleseedRenderer::AppleseedRenderer()
{
	this->mtap_scene = NULL;
	this->renderGlobals = NULL;
	definedEntities.clear();
	appleRenderer = this;
    this->project = asf::auto_release_ptr<asr::Project>(asr::ProjectFactory::create("mtap_project"));
	hasAOVs = false;
}

AppleseedRenderer::~AppleseedRenderer()
{
	logger.debug("Releasing scene");
	this->scene.reset();
	logger.debug("Releasing project");
	this->project.reset();
	logger.debug("Releasing done.");
	appleRenderer = NULL;
}

void AppleseedRenderer::defineProject()
{
	//TODO: the logging procedures of applesse causes a crash, no idea why.
	//asf::auto_release_ptr<asf::LogTargetBase> log_target(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());
	//asr::global_logger().add_target(log_target.release());

	//RENDERER_LOG_INFO("Testlog");

	//fprintf(stdout, "-------------DassnTest---------: %d:",123);
	//fflush(stdout);

	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";
	this->project->set_path(outputPath.asChar());
	this->scene = asr::SceneFactory::create();
	this->scenePtr = this->scene.get();
	this->project->set_scene(this->scene);

	definedEntities.clear();
}

void AppleseedRenderer::definePreRender()
{
	this->defineProject();
	this->defineConfig();
	clearShaderAssemblyAssignments();
}

void AppleseedRenderer::writeXML()
{
	logger.debug("AppleseedRenderer::writeXML");
	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + "." + (int)this->renderGlobals->currentFrame + ".appleseed";

	asr::ProjectFileWriter::write(this->project.ref(), outputPath.asChar());
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
                .insert("camera",this->scenePtr->get_camera()->get_name())
				.insert("resolution", res.asChar())
				.insert("tile_size", tileSize.asChar())
				.insert("color_space", colorSpaceString.asChar())));

}


void AppleseedRenderer::updateEntitiesCaller()
{
	logger.debug("AppleseedRenderer::updateEntities");
	if(appleRenderer != NULL)
		appleRenderer->updateEntities();
}



bool AppleseedRenderer::initializeRenderer(mtap_RenderGlobals *renderGlobals,  std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList)
{
	return true;
}

void AppleseedRenderer::defineMasterAssembly()
{
	if( this->scenePtr->assemblies().get_by_name("world") == NULL)
	{
		asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create( "world", asr::ParamArray());
		this->scenePtr->assemblies().insert(assembly);			

		asf::Matrix4d appMatrix;
		MMatrix transformMatrix;
		transformMatrix.setToIdentity();
		transformMatrix *= this->renderGlobals->globalConversionMatrix;
		asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create(
			"world_assInst",
			asr::ParamArray(),
			"world");

		this->MMatrixToAMatrix(transformMatrix, appMatrix);
		assemblyInstance->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
		this->scenePtr->assembly_instances().insert(assemblyInstance);
	}

	this->masterAssembly = this->scenePtr->assemblies().get_by_name("world");
}

void AppleseedRenderer::defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList, std::vector<MayaObject *>& instancerList)
{
	logger.debug("AppleseedRenderer::defineScene");

	// camera has to be defined here because the next command defineOutput will need a camera
	this->updateCamera(true);
	this->defineOutput();
	this->defineEnvironment(renderGlobals);
}


void AppleseedRenderer::addDeformStep(mtap_MayaObject *obj, asr::Assembly *assembly)
{
	MString geoName = obj->fullNiceName;
	asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());

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

	logger.debug(MString("Defining deform step for mesh object ") + obj->fullNiceName);
	
	asr::MeshObject *meshShape = (asr::MeshObject *)geoObject;
	MString meshName = meshShape->get_name();
	int numMbSegments = (int)meshShape->get_motion_segment_count();
	logger.debug(MString("Mesh name ") + meshName + " has " + numMbSegments + " motion segments");

	// now adding one motion segment
	size_t mbSegmentIndex = meshShape->get_motion_segment_count() + 1;
	meshShape->set_motion_segment_count(mbSegmentIndex);

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		//logger.debug(MString("update vtx0: ") + points[vtxId].x + " " + points[vtxId].y + " " + points[vtxId].z);

		meshShape->set_vertex_pose(vtxId, mbSegmentIndex - 1, vtx);
	}
}

//
//	Puts Object (only mesh at the moment) into this assembly.
//

void AppleseedRenderer::putObjectIntoAssembly(asr::Assembly *assembly, mtap_MayaObject *obj)
{
	MMatrix matrix;
	matrix.setToIdentity();
	putObjectIntoAssembly(assembly, obj, matrix);
}

void AppleseedRenderer::putObjectIntoAssembly(asr::Assembly *assembly, mtap_MayaObject *obj, MMatrix matrix)
{

	asf::StringArray material_names;
	// here the mesh per face assignments are read and placed into the obj->perFaceAssignments
	this->defineObjectMaterial(renderGlobals, obj, material_names);

	asr::MeshObjectArray meshArray;
	bool isProxy = false;
	this->createMesh(obj, meshArray, isProxy);

	logger.debug(MString("Found ") + meshArray.size() + " meshes.");

	for( size_t meshId = 0; meshId < meshArray.size(); meshId++)
	{
		MString meshName = meshArray[meshId]->get_name();
		logger.debug(MString("checking mesh ") + meshName);
		assembly->objects().insert(asf::auto_release_ptr<asr::Object>(meshArray[meshId]));
		asr::Object *meshObject = assembly->objects().get_by_name(meshName.asChar());
	
		asf::Matrix4d tmatrix = asf::Matrix4d::identity();
		this->MMatrixToAMatrix(matrix, tmatrix);

		asf::StringDictionary matDict = asf::StringDictionary();
		asf::StringDictionary matBackDict = asf::StringDictionary();

		int counterFront = 0;
		int counterBack = 0;
		if( isProxy )
		{
			logger.debug(MString("mesh is proxy, getting material names from mesh."));
			size_t numMat = meshObject->get_material_slot_count();
			for( size_t i = 0; i < numMat; i++)
			{						
				matBackDict.insert(MString(MString("slot_") + counterBack++).asChar(), meshObject->get_material_slot(i));
				matDict.insert(MString(MString("slot_") + counterFront++).asChar(), meshObject->get_material_slot(i));			
			}
		}else{
			for( size_t i = 0; i < material_names.size(); i++)
			{	
				if( pystring::endswith(material_names[i], "_back") )
					matBackDict.insert(MString(MString("slot_") + counterBack++).asChar(), material_names[i]);
				else
					matDict.insert(MString(MString("slot_") + counterFront++).asChar(), material_names[i]);			
			}
		}

		bool doubleSided = true;
		MFnDependencyNode depFn(obj->mobject);
		getBool(MString("doubleSided"), depFn, doubleSided);

		asr::ParamArray meshInstanceParams;

		MString ray_bias_method = "none";
		int id;
		getEnum("mtap_ray_bias_method", depFn, id, ray_bias_method);
		if( ray_bias_method != "none")
		{
			float ray_bias_distance = 0.0f;
			getFloat("mtap_ray_bias_distance", depFn, ray_bias_distance);
			meshInstanceParams.insert("ray_bias_method", ray_bias_method.asChar());
			meshInstanceParams.insert("mtap_ray_bias_distance", ray_bias_distance);
		}

		if(doubleSided)
		{
			if( matBackDict.size() == 0)
				matBackDict = matDict;

			assembly->object_instances().insert(
					asr::ObjectInstanceFactory::create(
					(meshName + "_inst").asChar(),
					meshInstanceParams,
					meshObject->get_name(),
					asf::Transformd::from_local_to_parent(tmatrix),
					matDict,
					matBackDict
					));
		}else
			assembly->object_instances().insert(
					asr::ObjectInstanceFactory::create(
					(meshName + "_inst").asChar(),
					asr::ParamArray(),
					meshObject->get_name(),
					asf::Transformd::from_local_to_parent(tmatrix),
					matDict
					));
	}
}
	

void AppleseedRenderer::render()
{
	logger.debug("AppleseedRenderer::render");

	bool isIpr = this->mtap_scene->renderType == MayaScene::IPR;

	this->tileCallbackFac.reset(new mtap_ITileCallbackFactory());

	masterRenderer = NULL;
	
	mtap_controller.entityUpdateProc = updateEntitiesCaller;

	if( this->mtap_scene->renderType == MayaScene::NORMAL)
	{
		masterRenderer = new asr::MasterRenderer(
			this->project.ref(),
			this->project->configurations().get_by_name("final")->get_inherited_parameters(),
			&mtap_controller,
			this->tileCallbackFac.get());
	}

	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		masterRenderer = new asr::MasterRenderer(
			this->project.ref(),
			this->project->configurations().get_by_name("interactive")->get_inherited_parameters(),
			&mtap_controller,
			this->tileCallbackFac.get());
	}
	
	
	//log_target.reset(asf::create_console_log_target(stderr));
    //asr::global_logger().add_target(log_target.get());

	m_log_target.reset(asf::create_file_log_target());
	MString logPath = this->renderGlobals->basePath + "/applelog" + (int)this->mtap_scene->renderGlobals->currentFrame + ".log";
	m_log_target->open(logPath.asChar() );
	asr::global_logger().add_target(m_log_target.get());

	logger.debug("------------- start rendering ----------------------");

	masterRenderer->render();

	//asr::global_logger().remove_target(log_target.get());
	asr::global_logger().remove_target(m_log_target.get());
	//m_log_target->close();

	this->renderGlobals->getImageName();
	logger.debug(MString("Writing image: ") + renderGlobals->imageOutputFile);
	MString imageOutputFile =  renderGlobals->imageOutputFile;
	project->get_frame()->write_main_image(imageOutputFile.asChar());
	project->get_frame()->write_aov_images(imageOutputFile.asChar());

	// if we render sequences, we remove all geometry and other elemenet because
	// otherwise we could have the case that we have a deforming geometry and it will receive more and more deform steps.
	if( this->mtap_scene->renderType != MayaScene::IPR )
	{
		this->scene.reset();

		if( MGlobal::mayaState() != MGlobal::kBatch )
		{
			MString cmd = MString("import mtap_initialize as minit; minit.theRenderer().showLogFile(\"") + logPath + "\")";
			logger.debug(cmd);
			MGlobal::executePythonCommand(cmd, true, false);	
		}
	}

}
