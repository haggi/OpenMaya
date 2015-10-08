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
		defineGlobalConversionMatrix();

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

	float MayaToWorld::toMillimeters(float mm)
	{
		return mm * 1.0 / rendererScaleFactor;
	}

	void MayaToWorld::defineGlobalConversionMatrix()
	{
		globalConversionMatrix.setToIdentity();
		MMatrix scaleMatrix;
		scaleMatrix.setToIdentity();
		this->internalUnit = MDistance::internalUnit();
		this->internalAxis = MGlobal::isYAxisUp() ? YUp : ZUp;
		setRendererUnit();
		setRendererAxis();

		internalScaleFactor = 1.0f; // internal in mm
		rendererScaleFactor = 1.0f; // external in mm

		this->scaleFactor = 1.0f;

		switch (this->internalUnit)
		{
		case MDistance::kCentimeters:
			internalScaleFactor = 10.0f;
			break;
		case MDistance::kFeet:
			internalScaleFactor = 304.8f;
			break;
		case MDistance::kInches:
			internalScaleFactor = 25.4f;
			break;
		case MDistance::kKilometers:
			internalScaleFactor = 1000000.f;
			break;
		case MDistance::kMeters:
			internalScaleFactor = 1000.f;
			break;
		case MDistance::kMillimeters:
			internalScaleFactor = 1.f;
			break;
		case MDistance::kMiles:
			internalScaleFactor = 1609344.f;
			break;
		case MDistance::kYards:
			internalScaleFactor = 914.4f;
			break;
		};

		switch (this->rendererUnit)
		{
		case MDistance::kCentimeters:
			rendererScaleFactor = 10.0f;
			break;
		case MDistance::kFeet:
			rendererScaleFactor = 304.8f;
			break;
		case MDistance::kInches:
			rendererScaleFactor = 25.4f;
			break;
		case MDistance::kKilometers:
			rendererScaleFactor = 1000000.f;
			break;
		case MDistance::kMeters:
			rendererScaleFactor = 1000.f;
			break;
		case MDistance::kMillimeters:
			rendererScaleFactor = 1.f;
			break;
		case MDistance::kMiles:
			rendererScaleFactor = 1609344.f;
			break;
		case MDistance::kYards:
			rendererScaleFactor = 914.4f;
			break;
		};

		scaleFactor = internalScaleFactor / rendererScaleFactor * sceneScale;
		scaleMatrix[0][0] = scaleMatrix[1][1] = scaleMatrix[2][2] = scaleFactor;

		scaleMatrix = scaleMatrix * sceneScaleMatrix;

		MMatrix YtoZ;
		YtoZ.setToIdentity();
		YtoZ[0][0] = 1;
		YtoZ[0][1] = 0;
		YtoZ[0][2] = 0;
		YtoZ[0][3] = 0;

		YtoZ[1][0] = 0;
		YtoZ[1][1] = 0;
		YtoZ[1][2] = 1;
		YtoZ[1][3] = 0;

		YtoZ[2][0] = 0;
		YtoZ[2][1] = -1;
		YtoZ[2][2] = 0;
		YtoZ[2][3] = 0;

		YtoZ[3][0] = 0;
		YtoZ[3][1] = 0;
		YtoZ[3][2] = 0;
		YtoZ[3][3] = 1;


		if ((this->internalAxis == YUp) && (this->rendererAxis == ZUp))
		{
			globalConversionMatrix = YtoZ;
		}

		globalConversionMatrix *= scaleMatrix;
	}


}