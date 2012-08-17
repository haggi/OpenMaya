#include "iprcmd.h"

#include <memory.h>

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include "mtap_mayaScene.h"
#include <maya/MArgList.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MNodeMessage.h>


#include "utilities/logging.h"
#include "utilities/tools.h"

#include "dummyRenderer/dummyScene.h"
#include "threads/renderQueueWorker.h"

static Logging logger;

IprTestCmd::IprTestCmd()
{
}

void* IprTestCmd::creator()
{
	return new IprTestCmd();
}

IprTestCmd::~IprTestCmd() 
{
}

MSyntax IprTestCmd::newSyntax()
{
	MSyntax				syntax;
	MStatus stat;
	stat = syntax.addFlag( "-str", "-startRender");
	if( !stat )
		MGlobal::displayInfo("Could not add startRender flag");
	//// Flag -startIPR
	syntax.addFlag( "-si", "-startIpr");
	//// Flag -stopIPR
	syntax.addFlag( "-sti", "-stopIpr");
	//// Flag -pauseIPR
	syntax.addFlag( "-pi", "-pauseIpr");

	return syntax;
}


void timerCallback( float time, float lastTime, void *userPtr)
{
	//logger.debug("TimerCallback.");
	RenderQueueWorker::startRenderQueueWorker();
}

MStatus IprTestCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing IprTestCmd ...");
	logger.setLogLevel(Logging::Debug);


	MArgDatabase argData(syntax(), args);
	EventQueue::Event e;
	e.data = NULL;
	if ( argData.isFlagSet("-startRender", 	&stat))
	{
		// create new scene, parse scene, render it
		MScene *scene = new MScene();
		scene->type = MScene::NORMAL;
		MGlobal::displayInfo("Executing startRender...");
		logger.debug("-startRender set, trying to start dummyRenderer.");
		e.type = EventQueue::Event::STARTRENDER;
		e.data = scene;
		theRenderEventQueue()->push(e);
		RenderQueueWorker::startRenderQueueWorker();
	}

	if ( argData.isFlagSet("-startIpr"))
	{
		logger.debug("-startIpr set, trying to start ipr.");
		int dummy = 1;
		MScene *scene = new MScene();
		scene->type = MScene::IPR;
		e.type = EventQueue::Event::STARTRENDER;
		e.data = scene;
		theRenderEventQueue()->push(e);
		RenderQueueWorker::startRenderQueueWorker();
	}

	if ( argData.isFlagSet("-stopIpr"))
	{
		logger.debug("-stopIpr set, trying to start ipr.");
		e.type = EventQueue::Event::IPRSTOP;
		theRenderEventQueue()->push(e);
	}

	if ( argData.isFlagSet("-pauseIpr"))
	{
		logger.debug("-pauseIpr set, trying to start ipr.");
		//e.type = EventQueue::Event::IPRPAUSE;
		//iprEventQueue()->push(e);
	}

	MGlobal::displayInfo("IprTestCmd rendering done.");
	return MStatus::kSuccess;
}
