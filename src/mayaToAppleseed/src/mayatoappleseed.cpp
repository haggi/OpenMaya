#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include "mayatoappleseed.h"
#include "mtap_mayaScene.h"
#include "utilities/logging.h"

static Logging logger;

void* MayaToAppleseed::creator()
{
	return new MayaToAppleseed();
}

MayaToAppleseed::~MayaToAppleseed() {}

MSyntax MayaToAppleseed::newSyntax()
{
	MSyntax syntax;

	return syntax;
}

void printUsage()
{
	MString usage("Usage: mayatoappleseed -basePath /root/projects/myproject/mantra -imagePath /root/projects/myproject/images");
}

MStatus MayaToAppleseed::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing mayatoappleseed...");
	logger.info("");
	logger.setLogLevel(Logging::Debug);

	mtap_MayaScene mayaScene = mtap_MayaScene();
	
	if( !mayaScene.good )
	{
		logger.error("Problems have occurred during creation of mayaScene(). Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}


	if( !mayaScene.parseScene() )
	{
		logger.error("Problems have occurred during parsing of the scene. Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}	

	if(!mayaScene.renderScene())
	{
		logger.error("Problems have occurred during rendering of the scene. Sorry cannot proceed.\n\n");
		return MS::kFailure;
	}	
	MGlobal::displayInfo("mayatoappleseed rendering done.\n");
	return MStatus::kSuccess;
}
