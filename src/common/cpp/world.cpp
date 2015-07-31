#include "world.h"
#include <thread>
#include <maya/MSceneMessage.h>
#include <maya/MGlobal.h>
#include "mayaSceneFactory.h"
#include "renderGlobalsFactory.h"
#include "rendering/rendererFactory.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"

static MCallbackId timerCallbackId = 0;
static MCallbackId beforeExitId = 0;

namespace MayaTo{
	MCallbackId MayaToWorld::afterOpenCallbackId;
	MCallbackId MayaToWorld::afterNewCallbackId;

	void *getObjPtr(MString name)
	{
		if (worldPointer != nullptr)
			return MayaTo::getWorldPtr()->getObjPtr(name);
		return nullptr;
	}


	static void addObjectPtr(MString name, void *ptr)
	{
		if (worldPointer != nullptr)
			worldPointer->addObjectPtr(name, ptr);
	}

	void deleteWorld()
	{
		if (worldPointer != nullptr)
			delete worldPointer;
		worldPointer = nullptr;
	}

	void defineWorld()
	{
		deleteWorld();
		worldPointer = new MayaToWorld();
	}

	MayaToWorld *getWorldPtr()
	{
		return worldPointer;
	}

	void MayaToWorld::beforeExitCallback(void *ptr)
	{
		if (timerCallbackId != 0)
			MTimerMessage::removeCallback(timerCallbackId);
		if (MayaToWorld::afterNewCallbackId != 0)
			MSceneMessage::removeCallback(MayaToWorld::afterNewCallbackId);
		if (beforeExitId != 0)
			MSceneMessage::removeCallback(beforeExitId);
		MayaTo::getWorldPtr()->cleanUp();
	}

	MayaToWorld::MayaToWorld()
	{
		MStatus stat;

		// in batch mode we do not need any renderView callbacks, and timer callbacks do not work anyway in batch
		if (MGlobal::mayaState() != MGlobal::kBatch)
			timerCallbackId = MTimerMessage::addTimerCallback(0.001, RenderQueueWorker::renderQueueWorkerTimerCallback, nullptr);
		//MSceneMessage::addCallback(MSceneMessage::kAfterNew, MayaToWorld::callAfterNewCallback, nullptr, &stat);		
		MayaToWorld::afterNewCallbackId = MSceneMessage::addCallback(MSceneMessage::kAfterOpen, MayaToWorld::callAfterOpenCallback, nullptr, &stat);

		initialize();
		renderType = WorldRenderType::RTYPENONE;
		renderState = WorldRenderState::RSTATENONE;
		worldScenePtr = nullptr;
		worldRendererPtr = nullptr;
		worldRenderGlobalsPtr = nullptr;

	};

	MayaToWorld::~MayaToWorld()
	{
		MayaToWorld::beforeExitCallback(nullptr);
	}

	void MayaToWorld::initializeScene()
	{
		MayaTo::MayaSceneFactory().createMayaScene();
	}

	void MayaToWorld::initializeRenderGlobals()
	{
		MayaTo::RenderGlobalsFactory().createRenderGlobals();
	}

	void MayaToWorld::initializeRenderer()
	{
		MayaTo::MayaRendererFactory().createRenderer();
	}

	void MayaToWorld::initializeRenderEnvironment()
	{
		this->initializeRenderGlobals();
		this->initializeScene();
		this->initializeRenderer();
	}
}