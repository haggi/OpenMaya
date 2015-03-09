#include "world.h"
#include "mayaSceneFactory.h"
#include "renderGlobalsFactory.h"
#include "rendering/rendererFactory.h"
#include "threads/renderQueueWorker.h"

static MCallbackId timerCallbackId = 0;

namespace MayaTo{
	void *getObjPtr(MString name)
	{
		if (worldPointer != NULL)
			return MayaTo::getWorldPtr()->getObjPtr(name);
		return NULL;
	}

	static void addObjectPtr(MString name, void *ptr)
	{
		if (worldPointer != NULL)
			worldPointer->addObjectPtr(name, ptr);
	}

	void deleteWorld()
	{
		if (worldPointer != NULL)
			delete worldPointer;
		worldPointer = NULL;
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

	MayaToWorld::MayaToWorld()
	{
		timerCallbackId = MTimerMessage::addTimerCallback(0.001, RenderQueueWorker::renderQueueWorkerTimerCallback, NULL);

		initialize();
		renderType = WorldRenderType::RTYPENONE;
		renderState = WorldRenderState::RSTATENONE;
		worldScenePtr = NULL;
		worldRendererPtr = NULL;
		worldRenderGlobalsPtr = NULL;
	};

	MayaToWorld::~MayaToWorld()
	{
		if (timerCallbackId != 0)
			MTimerMessage::removeCallback(timerCallbackId);
		cleanUp();
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