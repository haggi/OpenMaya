#include "MayaToAppleseed.h"
#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "world.h"

void* MayaToAppleseed::creator()
{
	return new MayaToAppleseed();
}

MayaToAppleseed::MayaToAppleseed() {}
MayaToAppleseed::~MayaToAppleseed() {}

MSyntax MayaToAppleseed::newSyntax()
{
	MSyntax syntax;
	MStatus stat;

	stat = syntax.addFlag("-cam", "-camera", MSyntax::kString);
	stat = syntax.addFlag("-s", "-state");
	stat = syntax.addFlag("-wi", "-width", MSyntax::kLong);
	stat = syntax.addFlag("-hi", "-height", MSyntax::kLong);
	// Flag -startIPR
	stat = syntax.addFlag("-sar", "-startIpr");
	// Flag -stopIPR
	syntax.addFlag("-str", "-stopIpr");
	// Flag -pauseIPR
	syntax.addFlag("-par", "-pauseIpr");

	return syntax;
}

void MayaToAppleseed::setLogLevel()
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

MStatus MayaToAppleseed::doIt(const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing MayaToAppleseed...");

	setLogLevel();

	MArgDatabase argData(syntax(), args);

	if (argData.isFlagSet("-state", &stat))
	{
		Logging::debug(MString("state: ???"));
		if (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::RSTATETRANSLATING)
			setResult("rstatetranslating");
		if (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::RSTATERENDERING)
			setResult("rstaterendering");
		if (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::RSTATEDONE)
			setResult("rstatedone");
		if (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::RSTATENONE)
			setResult("rstatenone");
		if (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::RSTATESTOPPED)
			setResult("rstatestopped");
		return MS::kSuccess;
	}

	if (argData.isFlagSet("-stopIpr", &stat))
	{
		Logging::debug(MString("-stopIpr"));
		EventQueue::Event e;
		e.type = EventQueue::Event::IPRSTOP;
		theRenderEventQueue()->push(e);
		return MS::kSuccess;
	}

	if (argData.isFlagSet("-pauseIpr", &stat))
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


	if (argData.isFlagSet("-startIpr", &stat))
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

	if (argData.isFlagSet("-camera", &stat))
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

	return MStatus::kSuccess;
}
