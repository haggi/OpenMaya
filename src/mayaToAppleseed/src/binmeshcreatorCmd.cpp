#include "binmeshcreatorCmd.h"

#include "appleseed/appleseedMeshWalker.h"
#include "foundation/mesh/genericmeshfilewriter.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>

#include "utilities/logging.h"

static Logging logger;

void* AppleseedBinMeshWriterCmd::creator()
{
	return new AppleseedBinMeshWriterCmd();
}

AppleseedBinMeshWriterCmd::AppleseedBinMeshWriterCmd() {}
AppleseedBinMeshWriterCmd::~AppleseedBinMeshWriterCmd() {}

MSyntax AppleseedBinMeshWriterCmd::newSyntax()
{
	MSyntax syntax;
	MStatus stat;

	stat = syntax.addFlag( "-p", "-path", MSyntax::kString);
	stat = syntax.addFlag( "-m", "-mesh", MSyntax::kString);
	
	return syntax;
}

void AppleseedBinMeshWriterCmd::printUsage()
{
	MGlobal::displayInfo("meshToBinaraymesh usage: ");
	MGlobal::displayInfo("meshToBinaraymesh -p exportPath -m mesh");
	MGlobal::displayInfo("e.g. meshToBinaraymesh -p c:/data/binmeshdir/mesh.binarymesh -m bodyShape");
}

bool AppleseedBinMeshWriterCmd::exportBinMesh(MObject meshObject)
{
	asf::GenericMeshFileWriter writer(this->path.asChar());
	MeshWalker walker(meshObject);
	writer.write(walker);
	return true;
}

MStatus AppleseedBinMeshWriterCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing AppleseedBinMeshWriterCmd...");
	logger.setLogLevel(Logging::Debug);
	
	MArgDatabase argData(syntax(), args);
	
	path = "";
	if( argData.isFlagSet("-path", &stat))
	{
		argData.getFlagArgument("-path", 0, path);
		logger.debug(MString("path: ") + path);
	}

	meshName = "";
	if( argData.isFlagSet("-mesh", &stat))
	{
		argData.getFlagArgument("-mesh", 0, meshName);
		logger.debug(MString("mesh: ") + meshName);
	}

	if( (path == "") || (meshName == ""))
	{
		MGlobal::displayError("meshToBinaraymesh failed.\n");
		printUsage();
		return  MStatus::kFailure;
	}

	MSelectionList selectionList;
	selectionList.add(meshName);
	MDagPath dagPath;
	stat = selectionList.getDagPath(0, dagPath);
	if( !stat )
	{
		MGlobal::displayError(MString("meshToBinaraymesh: could not get dagPath from mesh: ") + meshName);
		printUsage();
		return  MStatus::kFailure;
	}

	exportBinMesh(dagPath.node());
	
	MGlobal::displayInfo("AppleseedBinMeshWriterCmd done.\n");
	return MStatus::kSuccess;
}
