#include "iprcmd.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include "mtap_mayaScene.h"
#include <maya/MArgList.h>
#include <maya/MRenderView.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MNodeMessage.h>
#include <maya/MSceneMessage.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtap_iprTest.h"

static Logging logger;

void* IprTestCmd::creator()
{
	return new IprTestCmd();
}

IprTestCmd::~IprTestCmd() {}

MSyntax IprTestCmd::newSyntax()
{
	MSyntax				syntax;

	// Flag -startIPR
	syntax.addFlag( "-startIpr", "-startIpr");
	// Flag -stopIPR
	syntax.addFlag( "-stopIpr", "-stopIpr");
	// Flag -pauseIPR
	syntax.addFlag( "-pauseIpr", "-pauseIpr");

	syntax.setObjectType( MSyntax::kStringObjects, 1, 1);
	syntax.useSelectionAsDefault( false );

	return syntax;
}

void nodeDirtyCallback( MObject& node, void* clientData)
{

	logger.debug(MString("nodeDirtyCallback ") + getObjectName(node));
}

std::vector<MCallbackId> callbackIds;

void IprTestCmd::addCallbacks()
{
	MItDag   dagIterator(MItDag::kDepthFirst, MFn::kInvalid);
	MDagPath dagPath;
	
	for (dagIterator.reset(); (!dagIterator.isDone()); dagIterator.next())
	{
		if (!dagIterator.getPath(dagPath))
		{
			logger.error(MString("parseScene ERROR: Could not get path for DAG iterator."));
			return;
		}
		
		MFnDagNode node(dagPath.node());
		MObject obj = node.object();
		logger.debug(MString("parsing node: ") + node.name());
		
		callbackIds.push_back(MNodeMessage::addNodeDirtyCallback( obj, nodeDirtyCallback, NULL));      
	}
}

void IprTestCmd::removeCallbacks()
{
	for( size_t i = 0; i < callbackIds.size(); i++)
	{
		MSceneMessage::removeCallback(callbackIds[i]);
	}
}

void IprTestCmd::startIPR(int dummy)
{
	logger.debug("IprTestCmd::startIPR thread waiting for messages....");
	
	EventQueue::Event e;
	bool needsRestart = false;
	addCallbacks();

	while(true)
	{
		iprEventQueue()->wait_and_pop(e);
		if(e.type == EventQueue::Event::IPRSTART)
		{
			logger.debug("IprTestCmd::startIPR event loop received IPRSTART");
			if(iprActive())
			{
				logger.debug("IprTestCmd::startIPR IPRSTART - ipr active, try to stop ipr...");
				getIPR()->stopRendering();
				// in case the ipr has to be stopped, I have no idea when it is really removed.
				// the queue will receive a remove from the ipr thread and below a new IPRSTART event is sent to 
				// the list.
				needsRestart = true;
			}else{
				logger.debug("IprTestCmd::startIPR IPRSTART - ipr not active, try to start ipr...");
				MStatus status = MRenderView::startRender(640, 480, false, true);

				iprCreator()->startRendering();
			}
		}
		
		if(e.type == EventQueue::Event::IPRSTOP)
		{
			logger.debug("IprTestCmd::startIPR event loop received IPRSTOP");
			if(iprActive())
			{
				logger.debug("IprTestCmd::startIPR IPRSTOP - ipr active, try to stop rendering...");
				getIPR()->stopRendering();
				MStatus status = MRenderView::endRender();
			}
		}

		if(e.type == EventQueue::Event::IPRPAUSE)
		{
			logger.debug("IprTestCmd::startIPR event loop received IPRPAUSE");
		}

		if(e.type == EventQueue::Event::IPRREMOVE)
		{
			logger.debug("IprTestCmd::startIPR event loop received IPRREMOVE");
			iprDeleter();
			if(needsRestart)
			{
				logger.debug("IprTestCmd::startIPR IPRREMOVE needsRestart --> sending IPRSTART to the queue.");
				EventQueue::Event re;
				re.data = NULL;
				re.type = EventQueue::Event::IPRSTART;
				iprEventQueue()->push(re);
				needsRestart = false;
			}else{
				break;
			};
		}

		if(e.type == EventQueue::Event::IPRUPDATE)
		{
			logger.debug("IprTestCmd::startIPR event loop received IPRUPDATE");
			RV_PIXEL* pixels = (RV_PIXEL*)e.data;
			if( pixels != NULL)
			{
				MRenderView::updatePixels(0, 639, 0, 479, pixels);
				MRenderView::refresh(0, 639, 0, 479);
			}
			delete[] pixels;
		}
	}

	removeCallbacks();
	logger.debug("IprTestCmd::startIPR thread finished.");
}

static boost::thread rendererThread;

MStatus IprTestCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing IprTestCmd...");
	logger.setLogLevel(Logging::Debug);

	if( args.length() == 0)
	{
		logger.debug("please use iprtest -startIpr/-stopIpr/-pauseIpr");
		return MS::kFailure;
	}

	// does not work, no idea why, so skip it at the moment
	// MArgDatabase argData(syntax(), args);
	EventQueue::Event e;
	e.data = NULL;

	for ( int i = 0; i < args.length(); i++ )
	{
		if ( MString( "-startIpr" ) == args.asString( i, &stat ))
		{
			logger.debug("-startIpr set, trying to start ipr.");
			int dummy = 1;
			rendererThread = boost::thread(IprTestCmd::startIPR, dummy);
			rendererThread.detach();
			e.type = EventQueue::Event::IPRSTART;
			iprEventQueue()->push(e);
			break;
		}

		if ( MString( "-stopIpr" ) == args.asString( i, &stat ))
		{
			logger.debug("-stopIpr set, trying to start ipr.");
			e.type = EventQueue::Event::IPRSTOP;
			iprEventQueue()->push(e);
			break;
		}

		if ( MString( "-pauseIpr" ) == args.asString( i, &stat ))
		{
			logger.debug("-pauseIpr set, trying to start ipr.");
			e.type = EventQueue::Event::IPRPAUSE;
			iprEventQueue()->push(e);
			break;
		}
	}


	//ipr->startRenderThread();

	//logger.debug("Waiting 10 seconds...");
	//boost::this_thread::sleep(boost::posix_time::milliseconds(10 * 1000)); 
	//ipr->stopRendering();

	MGlobal::displayInfo("IprTestCmd rendering done.");
	return MStatus::kSuccess;
}
