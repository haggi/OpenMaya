#include "mayatoCorona.h"
#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include "mtco_common/mtco_mayaScene.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "mayaSceneFactory.h"
#include "mtco_common/mtco_renderGlobals.h"
#include "utilities\attrTools.h"

static Logging logger;

void* MayaToCorona::creator()
{
	return new MayaToCorona();
}

MayaToCorona::MayaToCorona() {}
MayaToCorona::~MayaToCorona() {}

MSyntax MayaToCorona::newSyntax()
{
	MSyntax syntax;
	MStatus stat;

	stat = syntax.addFlag( "-cam", "-camera", MSyntax::kString);
	stat = syntax.addFlag( "-wi", "-width", MSyntax::kLong);
	stat = syntax.addFlag( "-hi", "-height", MSyntax::kLong);
	// Flag -startIPR
	stat = syntax.addFlag( "-sar", "-startIpr");
	// Flag -stopIPR
	syntax.addFlag( "-str", "-stopIpr");
	// Flag -pauseIPR
	syntax.addFlag( "-par", "-pauseIpr");
	
	return syntax;
}

void MayaToCorona::setLogLevel()
{
	MObject globalsObj = objectFromName("coronaGlobals");
	if (globalsObj == MObject::kNullObj)
	{
		Logging::setLogLevel(Logging::Debug);
		return;
	}
	MFnDependencyNode globalsNode(globalsObj);
	int logLevel = getIntAttr("translatorVerbosity", globalsNode, 0);
	Logging::setLogLevel((Logging::LogLevel)logLevel);
}

MStatus MayaToCorona::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing mayaToCorona...");
	
	setLogLevel();

	MArgDatabase argData(syntax(), args);
	
	int width = -1;
	int height = -1;

	MayaScene *mayaScene = MayaTo::MayaSceneFactory().getMayaScene();

	if( !mayaScene->good )
	{
		logger.error("Problems have occurred during creation of mayaScene(). Sorry cannot proceed.\n\n");
		MayaTo::MayaSceneFactory().deleteMaysScene();
		return MS::kFailure;
	}

	if (mayaScene->renderState == MayaScene::RENDERING)
	{
		MGlobal::displayError("\nThere is already a render process running... please stop current rendering first.\n");
		return MS::kFailure;
	}

	if ( argData.isFlagSet("-width", &stat))
	{
		argData.getFlagArgument("-width", 0, width);
		logger.debug(MString("width: ") + width);
		if( width > 0 )
			mayaScene->renderGlobals->imgWidth = width;
	}

	if ( argData.isFlagSet("-height", &stat))
	{
		argData.getFlagArgument("-height", 0, height);
		logger.debug(MString("height: ") + height);
		if( height > 0 )
			mayaScene->renderGlobals->imgHeight = height;
	}

	if (mayaScene->renderGlobals->useRenderRegion)
	{
		int imgWidth = mayaScene->renderGlobals->imgWidth;
		int imgHeight = mayaScene->renderGlobals->imgHeight;
		RenderGlobals *rg = mayaScene->renderGlobals;
		if ((rg->regionLeft > imgWidth) || (rg->regionRight > imgWidth) || (rg->regionBottom > imgHeight) || (rg->regionTop > imgHeight))
			rg->useRenderRegion = false;
	}
	if ( argData.isFlagSet("-stopIpr", &stat))
	{
		logger.debug(MString("-stopIpr"));
		EventQueue::Event e;
		e.type = EventQueue::Event::IPRSTOP;
		theRenderEventQueue()->push(e);
		return MS::kSuccess;
	}

	if ( argData.isFlagSet("-pauseIpr", &stat))
	{
		logger.debug(MString("-pauseIpr"));
		logger.debug(MString("-stopIpr"));
		EventQueue::Event e;
		e.type = EventQueue::Event::IPRPAUSE;
		theRenderEventQueue()->push(e);
		return MS::kSuccess;
	}

	if ( argData.isFlagSet("-startIpr", &stat))
	{
		logger.debug(MString("-startIpr"));
		mayaScene->setRenderType(MayaScene::IPR);
	}
	
	if ( argData.isFlagSet("-camera", &stat))
	{
	    MDagPath camera;
        MSelectionList selectionList;
		argData.getFlagArgument("-camera", 0, selectionList);
        stat = selectionList.getDagPath(0, camera);
		camera.extendToShape();
		Logging::debug(MString("camera: ") + camera.fullPathName());
		mayaScene->setCurrentCamera(camera);
	}			
	
	mayaScene->renderState = MayaScene::RENDERING;

	EventQueue::Event e;
	e.type = EventQueue::Event::INITRENDER;
	theRenderEventQueue()->push(e);
	
	RenderQueueWorker::startRenderQueueWorker();
	
	return MStatus::kSuccess;
}
