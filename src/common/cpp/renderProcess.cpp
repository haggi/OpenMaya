#include "renderProcess.h"
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include "world.h"
#include "renderGlobals.h"
#include "mayaScene.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"

namespace RenderProcess{

	void doPreRenderJobs(){}
	
	void doPreFrameJobs()
	{
		MString result;
		MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->preFrameScript, result, true);
	}
	void doPostFrameJobs()
	{
		MString result;
		MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->postFrameScript, result, true);
	}

	void doPostRenderJobs(){}
	void doPrepareFrame()
	{
		float currentFrame = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getFrameNumber();
		std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
		Logging::progress(MString("\n========== doPrepareFrame ") + currentFrame + " ==============\n");

		mayaScene->parseScene();
		MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getMbSteps();

		if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size() == 0)
		{
			Logging::error(MString("no mb steps, something's wrong."));
			return;
		}

		int numMbSteps = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size();

		for (auto camera : mayaScene->camList)
		{
			if (!isCameraRenderable(camera->mobject) && (!(camera->dagPath == mayaScene->uiCamera)))
			{
				Logging::debug(MString("Camera ") + camera->shortName + " is not renderable, skipping.");
				continue;
			}
			Logging::info(MString("Rendering camera ") + camera->shortName);

			for (int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
			{
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep = mbStepId;
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbElement = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId];
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber = (float)(currentFrame + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time);
				bool needView = true;

				// we can have some mb time steps at the same time, e.g. for xform and deform, then we do not need to update the view
				if (mbStepId > 0)
				{
					if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time == MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId - 1].time)
					{
						needView = false;
					}
				}

				if (needView)
				{
					Logging::debug(MString("doFrameJobs() viewFrame: ") + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
					if (MGlobal::mayaState() != MGlobal::kBatch)
					{
						MRenderView::setCurrentCamera(camera->dagPath);
					}
					MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
				}

				if (MayaTo::getWorldPtr()->worldScenePtr)
					mayaScene->updateScene();
				else
					Logging::error(MString("no maya scene ptr."));

				Logging::info(MString("update scene done"));
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep++;
			}

			if (MGlobal::mayaState() != MGlobal::kBatch)
				MGlobal::viewFrame(currentFrame);
		}
	}

	void doFrameJobs()
	{
		Logging::debug("doFrameJobs()");
		std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
		float currentFrame = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame;
		Logging::progress(MString("\n========== Start rendering of frame ") + currentFrame + " ==============\n");

		mayaScene->parseScene();
		MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getMbSteps();

		if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size() == 0)
		{
			Logging::error(MString("no mb steps, something's wrong."));
			return;
		}

		int numMbSteps = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size();

		for (auto camera:mayaScene->camList)
		{
			if (!isCameraRenderable(camera->mobject) && (!(camera->dagPath == mayaScene->uiCamera)))
			{
				Logging::debug(MString("Camera ") + camera->shortName + " is not renderable, skipping.");
				continue;
			}
			Logging::info(MString("Rendering camera ") + camera->shortName);

			for (int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
			{
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep = mbStepId;
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbElement = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId];
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber = (float)(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time);
				bool needView = true;

				// we can have some mb time steps at the same time, e.g. for xform and deform, then we do not need to update the view
				if (mbStepId > 0)
				{
					if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time == MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId - 1].time)
					{
						needView = false;
					}
				}

				if (needView)
				{
					Logging::debug(MString("doFrameJobs() viewFrame: ") + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
					if (MGlobal::mayaState() != MGlobal::kBatch)
					{
						MRenderView::setCurrentCamera(camera->dagPath);
					}
					float tst = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber;
					MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
					tst = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber;
				}

				if(MayaTo::getWorldPtr()->worldScenePtr)
					mayaScene->updateScene();
				else
					Logging::error(MString("no maya scene ptr."));

				Logging::info(MString("update scene done"));
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep++;
			}

			if (MGlobal::mayaState() != MGlobal::kBatch)
				MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame);

			//MayaTo::getWorldPtr()->worldRendererPtr->render();

			//EventQueue::Event e;
			//e.type = EventQueue::Event::FRAMEDONE;
			//theRenderEventQueue()->push(e);
		}
	}

	void render()
	{
		//Logging::info("MayaScene::renderScene()");
		//std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
		//mayaScene->getPasses();

		////if (this->renderType == IPR)
		////	if (!this->canDoIPR())
		////	{
		////		Logging::warning("Sorry this renderer cannot render in IPR mode.");
		////		return false;
		////	}

		//doPreRenderJobs();
		//int numFrames = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameList.size();
		//for (int frameNr = 0; frameNr < numFrames; frameNr++)
		//{
		//	float currentFrame = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameList[frameNr];
		//	mayaScene->currentFrame = currentFrame;
		//	MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame = currentFrame;
		//	doPreFrameJobs();
		//	doFrameJobs();
		//	doPostFrameJobs();
		//	Logging::progress(MString("\n========== Rendering of frame ") + currentFrame + " done ==============\n");
		//}

		//doPostRenderJobs();
		//MayaTo::getWorldPtr()->renderState = MayaTo::MayaToWorld::WorldRenderState::RSTATENONE;

		//// if not ipr
		//EventQueue::Event e;
		//e.type = EventQueue::Event::RENDERDONE;
		//theRenderEventQueue()->push(e);

	}

}
