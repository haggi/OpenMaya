#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnInstancer.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MMatrixArray.h>
#include <maya/MIntArray.h>
#include <maya/MLightLinks.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MRenderView.h>
#include <maya/MVectorArray.h>
#include <maya/MFileIO.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnComponent.h>

#include "mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingUtils.h"
#include "threads/renderQueueWorker.h"
#include "rendering/renderer.h"
#include "world.h"

static Logging logger;

MayaScene::MayaScene()
{
	Logging::debug("MayaScene::MayaScene()");
	this->init();
}

void MayaScene::init()
{
	this->cando_ipr = false;
	this->good = true;
	this->renderType = NORMAL;
	this->renderState = MayaScene::UNDEF;
	this->renderingStarted = false;
}

MayaScene::~MayaScene()
{
	Logging::debug("MayaScene::~MayaScene()");
}

// Here is checked if the linklist is complete, what means that we do not have to do any 
// complicated light linking, but default procedures. If excludedLights is > 0 then we found 
// any excluded Lights.
bool MayaScene::listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights)
{
	excludedLights.clear();	
	for( uint lId = 0; lId < this->lightList.size(); lId++)
	{
		MDagPath path = this->lightList[lId]->dagPath;
		bool found = false;
		for( uint liId = 0; liId < linkedLights.length(); liId++)
		{
			MDagPath linkedPath = linkedLights[liId];
			if( linkedPath == path )
			{
				found = true;
				break;
			}
		}		
		if( found )
			continue;
		else
			excludedLights.append(path);
	}
	
	if( excludedLights.length() > 0)
		return false;

	return true;
}

bool MayaScene::lightObjectIsInLinkedLightList(std::shared_ptr<MayaObject> lightObject, MDagPathArray& linkedLightsArray)
{
	for (uint lId = 0; lId < linkedLightsArray.length(); lId++)
	{
		if (lightObject->mobject == linkedLightsArray[lId].node())
			return true;
	}
	return false;
}

// we have to take care for the component assignments in light linking. 
// if a mesh has per face shader assignments, we have to ask for the components to get the correct light linking
// because lightLink.getLinkedLights() will give us a wrong result in this case.

void MayaScene::getLightLinking()
{
	//Logging::debug(MString("----------- MayaScene::getLightLinking ---------------"));
	MLightLinks lightLink;
    bool parseStatus;
	parseStatus = lightLink.parseLinks(MObject::kNullObj);

	for (auto obj:this->objectList)
	{
		MDagPathArray lightArray;

		if (!obj->mobject.hasFn(MFn::kMesh) && !obj->mobject.hasFn(MFn::kNurbsSurface) && !obj->mobject.hasFn(MFn::kNurbsCurve))
			continue;

		if (obj->mobject.hasFn(MFn::kMesh))
		{
			MObjectArray shadingGroups, components;
			MFnMesh meshFn(obj->mobject);
			meshFn.getConnectedSetsAndMembers(obj->instanceNumber, shadingGroups, components, true);	
			//Logging::debug(MString("Object ") + obj->shortName + " has " + components.length() + " component groups and " + shadingGroups.length() + " shading groups.");
			int componentElements = 0;
			for (uint cId = 0; cId < components.length(); cId++)
			{
				MFnComponent compFn(components[cId]);
				if (compFn.componentType() == MFn::kMeshPolygonComponent)
					componentElements += compFn.elementCount();
			}
			if ((shadingGroups.length() > 1) || (componentElements > 0))
			{
				Logging::debug(MString("Object ") + obj->shortName + " has " + components.length() + " component groups and " + shadingGroups.length() + " shading groups.");
				for (uint cId = 0; cId < components.length(); cId++)
				{
					MDagPathArray tmpLightArray;
					lightLink.getLinkedLights(obj->dagPath, components[cId], tmpLightArray); // Lights linked to the face component
					for (uint lp = 0; lp < tmpLightArray.length(); lp++)
						lightArray.append(tmpLightArray[lp]);
				}
			}
			else{
				lightLink.getLinkedLights(obj->dagPath, MObject::kNullObj, lightArray);
			}
		}
		else{
			lightLink.getLinkedLights(obj->dagPath, MObject::kNullObj, lightArray);
		}
		// if one of the light in my scene light list is NOT in the linked light list,
		// the light has either turned off "Illuminate by default" or it is explicilty not linked to this object.
		for (size_t lObjId = 0; lObjId < this->lightList.size(); lObjId++)
		{
			if (lightObjectIsInLinkedLightList(this->lightList[lObjId], lightArray))
			{
				//Logging::debug(MString("Light object ") + this->lightList[lObjId]->shortName + " IS in lightList -> linked.");
			}
			else{
				//Logging::debug(MString("Light object ") + this->lightList[lObjId]->shortName + " is NOT in lightList -> " + obj->shortName + " is not linked.");
				this->lightList[lObjId]->excludedObjects.push_back(obj);
			}
		}
	}
}


//
//	Parse scene starting with "world" through all childs.
//	This allows an easy inheritance of attributes. e.g. you can give some element
//	in the hierarchy the colorAttribute "diffuseColor" and all objects below will inherit
//	this attribute until it is removed or overwritten by another value.
//

void MayaScene::getPasses()
{
	// clear
	//for( size_t renderPassId = 0; renderPassId < MayaTo::getWorldPtr()->worldRenderGlobalsPtr->renderPasses.size(); renderPassId++)
	//	delete MayaTo::getWorldPtr()->worldRenderGlobalsPtr->renderPasses[renderPassId];
	//MayaTo::getWorldPtr()->worldRenderGlobalsPtr->renderPasses.clear();

	//// add pass for lights for shadow maps 

	//// add pass for lights with photon maps

	//// finally do the normal beauty passes
	//RenderPass *rp = new RenderPass();
	//rp->evalFrequency = RenderPass::OncePerFrame;
	//rp->passType = RenderPass::Beauty;

	//int numcams = (int)this->camList.size();
	//for( int objId = 0; objId < numcams; objId++)
	//{
	//	std::shared_ptr<MayaObject> obj = this->camList[objId];

	//	// the renderable check is moved here because it will disable
	//	// rendering from the UI with a normally not renderable camera.
	//	if( MGlobal::mayaState() == MGlobal::kBatch )
	//	{
	//		bool renderable = false;
	//		MFnDependencyNode camFn(obj->mobject);
	//		getBool("renderable", camFn, renderable);
	//		if( renderable )
	//		{
	//			rp->objectList.push_back(obj);
	//			Logging::debug(MString("Adding camera ") + camFn.name() + " to render cam list.");
	//		}
	//	}else{
	//		rp->objectList.push_back(obj);
	//	}
	//}	
	//
	//MayaTo::getWorldPtr()->worldRenderGlobalsPtr->renderPasses.push_back(rp);
}


//bool MayaScene::doFrameJobs()
//{
//	Logging::debug("MayaScene::doFrameJobs()");
//	Logging::progress(MString("\n========== Start rendering of frame ") + this->currentFrame + " ==============\n");
//
//	this->parseScene(); 
//	MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getMbSteps();
//
//	if(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size() == 0)
//	{
//		Logging::error(MString("no mb steps, somethings wrong."));
//		return false;
//	}
//
//	int numMbSteps = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size();
//	
//	for( uint camId = 0; camId < this->camList.size(); camId++)
//	{
//		std::shared_ptr<MayaObject> camera = this->camList[camId];
//		if( !isCameraRenderable(camera->mobject) && (!(camera->dagPath == this->uiCamera)))
//		{	
//			Logging::debug(MString("Camera ") + camera->shortName + " is not renderable, skipping.");
//			continue;
//		}
//		
//		Logging::info(MString("Rendering camera ") + camera->shortName);
//
//		for( int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
//		{
//			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep = mbStepId;
//			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->isMbStartStep = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time == MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[0].time;
//			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbElement = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId];
//			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber = (float)(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time);
//			bool needView = true;
//
//			// we can have some mb time steps at the same time, e.g. for xform and deform, then we do not need to update the view
//			if( mbStepId > 0)
//			{
//				if( MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time == MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId-1].time)
//				{
//					needView = false;
//				}
//			}
//
//			if( needView )
//			{
//				Logging::debug(MString("doFrameJobs() viewFrame: ") + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
//				if( MGlobal::mayaState() != MGlobal::kBatch )
//				{
//					MRenderView::setCurrentCamera( camera->dagPath );
//				}
//				MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
//			}
//
//			this->updateScene();
//			Logging::info(MString("update scene done"));
//			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep++;
//		}
//
//		if( MGlobal::mayaState() != MGlobal::kBatch )
//			MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame);
//
//		MayaTo::getWorldPtr()->worldRendererPtr->render();
//
//		EventQueue::Event e;
//		e.type = EventQueue::Event::FRAMEDONE;
//		theRenderEventQueue()->push(e);
//	}
//
//	return true;
//}
//
//bool MayaScene::renderScene()
//{
//	Logging::info("MayaScene::renderScene()");
//	MayaTo::getWorldPtr()->renderType = MayaTo::MayaToWorld::UIRENDER;
//	this->getPasses();
//
//	if( this->renderType == IPR )
//		if(!this->canDoIPR())
//		{
//			Logging::warning("Sorry this renderer cannot render in IPR mode.");
//			return false;
//		}
//
//	if(!this->doPreRenderJobs())
//	{
//		Logging::error("doPreRenderJobs failed.");
//		return false;
//	}
//
//	int numFrames = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameList.size();
//	for( int frameNr = 0; frameNr < numFrames; frameNr++)
//	{
//		this->currentFrame = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameList[frameNr];
//		MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrame = this->currentFrame;
//		if(!this->doPreFrameJobs())
//		{
//			Logging::error("doPreFrameJobs failed.");
//			return false;
//		}
//		if(!this->doFrameJobs())
//		{
//			Logging::error("doFrameJobs failed.");
//			return false;
//		}
//
//		if(!this->doPostFrameJobs())
//		{
//			Logging::error("doPostFrameJobs failed.");
//			return false;
//		}
//		Logging::progress(MString("\n========== Rendering of frame ") + this->currentFrame + " done ==============\n");
//	}
//
//	if(!this->doPostRenderJobs())
//	{
//		Logging::error("doPostRenderJobs failed.");
//		return false;
//	}
//
//	MayaTo::getWorldPtr()->renderState = MayaTo::MayaToWorld::WorldRenderState::RSTATENONE;
//	return true;
//}
//
////void MayaScene::waitForFrameCompletion()
////{
////	while( 1 )
////	{
////		if( this->renderState == RenderState::FRAMEDONE )
////			break;
////		if( this->renderState == RenderState::RENDERERROR )
////			break;
////		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
////	}
////}
//
//bool MayaScene::doPreRenderJobs()
//{
//	return true;
//}
//
//bool MayaScene::doPreFrameJobs()
//{
//	MString result;
//	MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->preFrameScript, result, true);
//	return true;
//}
//
//bool MayaScene::doPostFrameJobs()
//{
//	MString result;
//	MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->postFrameScript, result, true);
//	return true;
//}
//
//bool MayaScene::doPostRenderJobs()
//{
//	return true;
//}