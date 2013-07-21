#include "binmeshcreatorCmd.h"

#include "appleseed/appleseedMeshWalker.h"
#include "foundation/mesh/genericmeshfilewriter.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>

#include "utilities/pystring.h"
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

	stat = syntax.addFlag( "-pa", "-path", MSyntax::kString);
	stat = syntax.addFlag( "-m", "-mesh", MSyntax::kString);
	stat = syntax.addFlag( "-np", "-noProxy", MSyntax::kBoolean);
	stat = syntax.addFlag( "-n", "-nthPoly", MSyntax::kLong);
	stat = syntax.addFlag( "-p", "-percentage", MSyntax::kDouble);
	
	return syntax;
}

void AppleseedBinMeshWriterCmd::printUsage()
{
	MGlobal::displayInfo("meshToBinaraymesh usage: ");
	MGlobal::displayInfo("meshToBinaraymesh -p exportPath -m mesh");
	MGlobal::displayInfo("e.g. meshToBinaraymesh -p c:/data/binmeshdir/mesh.binarymesh -m bodyShape");
	MGlobal::displayInfo("optional -np true -- will create no proxy file.");
}

bool AppleseedBinMeshWriterCmd::exportBinMesh(MDagPath dagPath)
{
	MObject meshObject = dagPath.node();
	asf::GenericMeshFileWriter writer(this->path.asChar());
	MeshWalker walker(meshObject, doProxy, nthPoly, percentage);
	writer.write(walker);
	
	if( this->doProxy )
	{
		MString proxyFile = this->path;
		proxyFile = pystring::replace(this->path.asChar(), ".binarymesh" , ".proxymesh").c_str();
		logger.info(MString("Creating proxy file ") + proxyFile);
		
		pFile.open(proxyFile.asChar(),  std::ios_base::out | std::ios_base::binary);

		if( pFile.good() )
		{
			// the first two points contain the bounding box of the geometry
			// here we only write the number of triangle points
			this->write((int)(walker.proxyPoints.length() - 2));
			for( uint i = 0; i < walker.proxyPoints.length(); i++)
				this->write(walker.proxyPoints[i]);
			pFile.close();
			logger.info(MString("Written ") + ((int)(walker.proxyPoints.length() - 2)) + " points == " + ((walker.proxyPoints.length() - 2)/3) + " triangles");
		}else{
			logger.error(MString("Problem opening proxy file."));
		}
	}	
	return true;
}

MStatus AppleseedBinMeshWriterCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing AppleseedBinMeshWriterCmd...");
	logger.setLogLevel(Logging::Debug);
	
	this->doProxy = true;

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

	if( argData.isFlagSet("-noProxy", &stat))
	{
		this->doProxy = false;
		logger.debug(MString("do not create proxyFile."));
	}

	this->nthPoly = 0;
	if( argData.isFlagSet("-nthPoly", &stat))
	{
		argData.getFlagArgument("-nthPoly", 0, nthPoly);
		logger.debug(MString("nthPoly: ") + nthPoly);
	}

	this->percentage = 0.0f;
	if( argData.isFlagSet("-percentage", &stat))
	{
		double p = 0.0;
		argData.getFlagArgument("-percentage", 0, p);
		percentage = (float)p;
		logger.debug(MString("percentage: ") + percentage);
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

	exportBinMesh(dagPath);
	
	MGlobal::displayInfo("AppleseedBinMeshWriterCmd done.\n");
	return MStatus::kSuccess;
}
