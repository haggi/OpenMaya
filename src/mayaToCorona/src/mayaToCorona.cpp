#include "mayatoCorona.h"
#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "world.h"
#include "Version.h"

#include <fstream>
#include <iostream>

#if MAYA_API_VERSION >= 201600

#endif

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

	stat = syntax.addFlag("-cam", "-camera", MSyntax::kString);
	stat = syntax.addFlag("-s", "-state");
	stat = syntax.addFlag("-ver", "-version");
	stat = syntax.addFlag("-ci", "-canDoIPR");
	stat = syntax.addFlag("-wi", "-width", MSyntax::kLong);
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
	MObject globalsObj = getRenderGlobalsNode();
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

	MArgDatabase argData(syntax(), args);

	if (argData.isFlagSet("-version", &stat))
	{
		MStringArray res;
		for (auto v : getFullVersionString())
			res.append(v.c_str());
		setResult(res);
		
		return MS::kSuccess;
	}

	MGlobal::displayInfo("Executing mayaToCorona...");
	setLogLevel();

	if (argData.isFlagSet("-state", &stat))
	{
		Logging::debug(MString("state: ???"));
		MayaTo::MayaToWorld::WorldRenderState rstate = MayaTo::getWorldPtr()->renderState;
		if (rstate == MayaTo::MayaToWorld::RSTATETRANSLATING)
				setResult("rstatetranslating");
		if (rstate == MayaTo::MayaToWorld::RSTATERENDERING)
			setResult("rstaterendering");
		if (rstate == MayaTo::MayaToWorld::RSTATEDONE)
			setResult("rstatedone");
		if (rstate == MayaTo::MayaToWorld::RSTATENONE)
			setResult("rstatenone");
		if (rstate == MayaTo::MayaToWorld::RSTATESTOPPED)
			setResult("rstatestopped");
		return MS::kSuccess;
	}

	if (argData.isFlagSet("-canDoIPR", &stat))
	{
		if(MayaTo::getWorldPtr()->canDoIPR())
			setResult("yes");
		else
			setResult("no");
		return MS::kSuccess;
	}

	if ( argData.isFlagSet("-stopIpr", &stat))
	{
		Logging::debug(MString("-stopIpr"));
		EventQueue::Event e;
		e.type = EventQueue::Event::IPRSTOP;
		theRenderEventQueue()->push(e);
		return MS::kSuccess;
	}

	if ( argData.isFlagSet("-pauseIpr", &stat))
	{
		Logging::debug(MString("-pauseIpr"));
		Logging::debug(MString("-stopIpr"));
		EventQueue::Event e;
		e.type = EventQueue::Event::IPRPAUSE;
		theRenderEventQueue()->push(e);
		return MS::kSuccess;
	}

	// I have to request useRenderRegion here because as soon the command is finished, what happens immediatly after the command is 
	// put into the queue, the value is set back to false.
	std::unique_ptr<MayaTo::CmdArgs> cmdArgs(new MayaTo::CmdArgs);
	MObject drg = objectFromName("defaultRenderGlobals");
	MFnDependencyNode drgfn(drg);
	cmdArgs->useRenderRegion = drgfn.findPlug("useRenderRegion").asBool();

	cmdArgs->renderType = MayaTo::MayaToWorld::WorldRenderType::UIRENDER;
	if (MGlobal::mayaState() == MGlobal::kBatch)
		cmdArgs->renderType = MayaTo::MayaToWorld::WorldRenderType::BATCHRENDER;

	if ( argData.isFlagSet("-startIpr", &stat))
	{
		Logging::debug(MString("-startIpr"));
		cmdArgs->renderType = MayaTo::MayaToWorld::WorldRenderType::IPRRENDER;
	}
	
	if (argData.isFlagSet("-width", &stat))
	{
		argData.getFlagArgument("-width", 0, cmdArgs->width);
		Logging::debug(MString("width: ") + cmdArgs->width);
	}

	if (argData.isFlagSet("-height", &stat))
	{
		argData.getFlagArgument("-height", 0, cmdArgs->height);
		Logging::debug(MString("height: ") + cmdArgs->height);
	}

	if ( argData.isFlagSet("-camera", &stat))
	{
	    MDagPath camera;
        MSelectionList selectionList;
		argData.getFlagArgument("-camera", 0, selectionList);
        stat = selectionList.getDagPath(0, camera);
		camera.extendToShape();
		Logging::debug(MString("camera: ") + camera.fullPathName());
		cmdArgs->cameraDagPath = camera;
	}			
	
	EventQueue::Event e;
	e.cmdArgsData = std::move(cmdArgs);
	e.type = EventQueue::Event::INITRENDER;
	theRenderEventQueue()->push(e);
	
	//
	if (MGlobal::mayaState() == MGlobal::kBatch)
	{
		RenderQueueWorker::startRenderQueueWorker();
	}
	return MStatus::kSuccess;
}
