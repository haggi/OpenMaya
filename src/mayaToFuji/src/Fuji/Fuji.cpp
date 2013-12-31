#include "Fuji.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"

#include "../mtfu_common/mtfu_renderGlobals.h"
#include "../mtfu_common/mtfu_mayaObject.h"
#include "../mtfu_common/mtfu_mayaScene.h"

#include "src/fj_scene_interface.h"
#include "FujiCallbacks.h"
#include <stdio.h>

#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>

#include "FujiImgTools.h"

static Logging logger;

using namespace FujiRender;

FujiRenderer::FujiRenderer()
{
	// define scene
	//SiOpenScene();
}

FujiRenderer::~FujiRenderer()
{
	//SiCloseScene();
}

void FujiRenderer::setTransform(mtfu_MayaObject *obj)
{
	if( obj->objectID == SI_BADID)
	{
		logger.error(MString("Object ") + obj->shortName + " has bad ID");
		return;
	}


	MMatrix rotMatrix;
	rotMatrix.setToIdentity();

	if( obj->mobject.hasFn(MFn::kAreaLight))
	{
		MTransformationMatrix tm(rotMatrix);
		double areaScale[3] = {2,2,2};
		tm.setScale(areaScale, MSpace::kWorld);
		MEulerRotation e(-90.0, 0.0, 0.0);
		tm.rotateBy(e, MSpace::kWorld);
		rotMatrix = tm.asMatrix();
	}
	
	MMatrix transformMatrix = rotMatrix * obj->dagPath.inclusiveMatrix();
	MTransformationMatrix objTMatrix(transformMatrix);
	double rot[3];
	double scale[3];

	MTransformationMatrix::RotationOrder rotOrder =  MTransformationMatrix::kXYZ;
	objTMatrix.getRotation(rot, rotOrder, MSpace::kWorld);
	MVector pos = objTMatrix.getTranslation(MSpace::kWorld);
	objTMatrix.getScale(scale, MSpace::kWorld);
	SiSetProperty3(obj->objectID, "translate", pos[0], pos[1], pos[2]);
	SiSetProperty3(obj->objectID, "rotate", RadToDeg(rot[0]), RadToDeg(rot[1]), RadToDeg(rot[2]));
	SiSetProperty3(obj->objectID, "scale", scale[0], scale[1], scale[2]);

	//logger.debug(MString("SetProperty3 ") + obj->shortName + " translate " + pos[0] + " " + pos[1] + " " + pos[2]);
	//logger.debug(MString("SetProperty3 ") + obj->shortName + " rotate " + RadToDeg(rot[0]) + " " + RadToDeg(rot[1]) + " " + RadToDeg(rot[2]));

}

void FujiRenderer::render()
{
	const int W = this->mtfu_renderGlobals->imgWidth;
	const int H = this->mtfu_renderGlobals->imgHeight;
	
	ID object;
	ID shader;

	logger.info("------- Starting fujiyama rendering --------");

	// Display pre render infos

	try{

		ID framebuffer;
		ID light;

		SiOpenScene();
		renderer = SiNewRenderer();
		if (renderer == SI_BADID) 
		{
			fprintf(stderr, "Could not allocate renderer\n");
			throw("Could not allocate renderer");
		}

		/* Plugin */
		if (SiOpenPlugin("PlasticShader")) 
		{
		}

		this->defineCamera();
		this->defineLights();
		this->defineSettings();

		shader = SiNewShader("PlasticShader");
		if (shader == SI_BADID) 
		{
			fprintf(stderr, "Could not create shader: PlasticShader\n");
			throw("Could not create shader: PlasticShader");
		}

		for( uint objId = 0; objId < this->mtfu_scene->objectList.size(); objId++)
		{
			mtfu_MayaObject *obj = (mtfu_MayaObject *)this->mtfu_scene->objectList[objId];
			ID mesh = SI_BADID;
			if( obj->exportFileNames.size() > 0)
			{

				mesh = SiNewMesh(obj->exportFileNames[0].asChar());
				if (mesh == SI_BADID) 
				{
					logger.error(MString("Could not create mesh from file: ") + obj->exportFileNames[0]);
					continue;
				}

				if( mesh == SI_BADID)
					continue;
				obj->objectID = mesh;
			}else{
				if( obj->origObject != NULL )
				{
					mtfu_MayaObject *origObj = (mtfu_MayaObject *)obj->origObject;
					mesh = origObj->objectID;
					if( mesh == SI_BADID)
						continue;					
				}else{
					continue;
				}
			};

			object = SiNewObjectInstance(mesh);
			if (object == SI_BADID) 
			{
				throw("Could not create object instance");
			}

			MTransformationMatrix objTMatrix(obj->dagPath.inclusiveMatrix());
			double rot[3];
			double scale[3];
			MTransformationMatrix::RotationOrder rotOrder =  MTransformationMatrix::kXYZ;
			objTMatrix.getRotation(rot, rotOrder, MSpace::kWorld);
			MVector pos = objTMatrix.getTranslation(MSpace::kWorld);
			objTMatrix.getScale(scale, MSpace::kWorld);
			SiSetProperty3(object, "translate", pos[0], pos[1], pos[2]);
			SiSetProperty3(object, "rotate", RadToDeg(rot[0]), RadToDeg(rot[1]), RadToDeg(rot[2]));
			SiSetProperty3(object, "scale", scale[0], scale[1], scale[2]);
			logger.debug(MString("SetProperty3 ") + obj->shortName + " translate " + pos[0] + " " + pos[1] + " " + pos[2]);
			logger.debug(MString("SetProperty3 ") + obj->shortName + " rotate " + RadToDeg(rot[0]) + " " + RadToDeg(rot[1]) + " " + RadToDeg(rot[2]));

			SiAssignShader(object, shader);
		}

		framebuffer = SiNewFrameBuffer("rgba");
		if (framebuffer == SI_BADID) 
		{
			fprintf(stderr, "Could not allocate framebuffer\n");
			throw("Could not allocate framebuffer");
		}
		SiAssignFrameBuffer(renderer, framebuffer);

		callbacks.setCallbacks(renderer);

		MString imageOutputFile = this->mtfu_renderGlobals->imageOutputFile;
		FujiImgTools::ImgTools imgTools;
		MString fbFile = imageOutputFile + ".fb";

		Status result = SiRenderScene(renderer);
		if( result == SI_BADID )
		{
			logger.error("Renderer exited with error.");
			SiCloseScene();
			throw("Renderer exited with error.");
		}
		SiSaveFrameBuffer(framebuffer, fbFile.asChar());
		imgTools.FrameBufferToExr(fbFile);
		SiCloseScene();

	}catch(char *errorMsg){
		logger.error(errorMsg);
		EventQueue::Event e;
		e.data = NULL;
		e.type = EventQueue::Event::RENDERERROR;
		theRenderEventQueue()->push(e);
	}

	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void FujiRenderer::initializeRenderer()
{

}

void FujiRenderer::updateShape(MayaObject *obj)
{
	if( !obj->geometryShapeSupported())
		return;
	
	if( obj->mobject.hasFn(MFn::kMesh ))
		this->updateGeometry(obj);
}

void FujiRenderer::updateTransform(MayaObject *obj)
{}
void FujiRenderer::IPRUpdateEntities()
{}
void FujiRenderer::reinitializeIPRRendering()
{}
void FujiRenderer::abortRendering()
{
	logger.info("User interrupt reuqested.");
	callbacks.stopRendering();
}

