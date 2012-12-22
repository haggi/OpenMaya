#include "MayaToKray.h"
#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include "mtkr_common/mtkr_mayaScene.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"

static Logging logger;

void* MayaToKray::creator()
{
	return new MayaToKray();
}

MayaToKray::MayaToKray() {}
MayaToKray::~MayaToKray() {}

MSyntax MayaToKray::newSyntax()
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

MStatus MayaToKray::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing MayaToKray...");
	logger.setLogLevel(Logging::Debug);
	
	MArgDatabase argData(syntax(), args);
	
	int width = -1;
	int height = -1;

	if ( argData.isFlagSet("-width", &stat))
	{
		argData.getFlagArgument("-width", 0, width);
		logger.debug(MString("width: ") + width);
	}

	if ( argData.isFlagSet("-height", &stat))
	{
		argData.getFlagArgument("-height", 0, height);
		logger.debug(MString("height: ") + height);
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
		return MS::kSuccess;
	}

	MayaScene::RenderType rtype = MayaScene::NORMAL;
	if ( argData.isFlagSet("-startIpr", &stat))
	{
		logger.debug(MString("-startIpr"));
		rtype = MayaScene::IPR;
	}else{
		logger.debug(MString("normal render"));
	}

	// if we are here, we want a normal or an startIPR rendering, so initialize the scene
	mtkr_MayaScene *mayaScene = new mtkr_MayaScene(rtype);
	
	if( !mayaScene->good )
	{
		logger.error("Problems have occurred during creation of mayaScene(). Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}

	if( !mayaScene->parseScene(MayaScene::HIERARCHYPARSE) )
	{
		logger.error("Problems have occurred during parsing of the scene. Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}	

	if ( argData.isFlagSet("-camera", &stat))
	{
	    MDagPath camera;
        MSelectionList selectionList;
		argData.getFlagArgument("-camera", 0, selectionList);
        stat = selectionList.getDagPath(0, camera);
		camera.extendToShape();
		logger.debug(MString("camera: ") + camera.fullPathName());
		mayaScene->setCurrentCamera(camera);
	}			

	if( height > 0)
		mayaScene->renderGlobals->imgHeight = height;

	if( width > 0)
		mayaScene->renderGlobals->imgWidth = width;

	if(!mayaScene->renderScene())
	{
		logger.error("Problems have occurred during rendering of the scene. Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}	

	MGlobal::displayInfo("MayaToKray rendering done.\n");
	return MStatus::kSuccess;
}
