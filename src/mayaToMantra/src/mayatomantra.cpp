#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include "mayatomantra.h"
#include "mtm_mayaScene.h"
#include "utilities/logging.h"
//#include "shadingTools/mtm_shadingGroup.h"
//#include "mtm_mayaObject.h"

static Logging logger;

void* mayatomantra::creator()
{
	return new mayatomantra();
}

mayatomantra::~mayatomantra() {}

MSyntax mayatomantra::newSyntax()
{
	MSyntax				syntax;

	return syntax;
}

void printUsage()
{
	MString usage("Usage: mayatomantra -basePath /root/projects/myproject/mantra -imagePath /root/projects/myproject/images");
}

MStatus mayatomantra::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing mayatomantra...");
	mtm_MayaScene mayaScene = mtm_MayaScene();

	// clear shader list
	// clearShaderNodes();
	// int sgsize = (int)mtm_MayaSGList.size();
	//MGlobal::displayInfo(MString("Sgsize before ") + sgsize);
	// clearObjectShadingGroups();
	// sgsize = (int)mtm_MayaSGList.size();
	//MGlobal::displayInfo(MString("Sgsize after ") + sgsize);
	
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
	MGlobal::displayInfo("MayaToMantra rendering done.\n");
	return MStatus::kSuccess;
}
