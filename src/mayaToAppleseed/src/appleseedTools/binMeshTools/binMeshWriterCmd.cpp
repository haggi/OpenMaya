#include "binMeshWriterCmd.h"

#include "foundation/mesh/genericmeshfilewriter.h"
#include "appleseedMeshWalker.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;

#include "proxyMesh.h"


void* BinMeshWriterCmd::creator()
{
	return new BinMeshWriterCmd();
}

BinMeshWriterCmd::BinMeshWriterCmd() {}
BinMeshWriterCmd::~BinMeshWriterCmd() {}

MSyntax BinMeshWriterCmd::newSyntax()
{
	MSyntax syntax;
	MStatus stat;

	stat = syntax.addFlag( "-pa", "-path", MSyntax::kString);
	stat = syntax.addFlag( "-dp", "-doProxy", MSyntax::kBoolean);
	stat = syntax.addFlag( "-p", "-percentage", MSyntax::kDouble);
	stat = syntax.addFlag( "-all", "-exportAll", MSyntax::kBoolean);
	stat = syntax.addFlag( "-fpm", "-oneFilePerMesh", MSyntax::kBoolean);
	stat = syntax.addFlag( "-dt", "-doTransform", MSyntax::kBoolean);
	syntax.setObjectType(MSyntax::kSelectionList);
	syntax.useSelectionAsDefault(true);
	
	return syntax;
}

void BinMeshWriterCmd::printUsage()
{
	MGlobal::displayInfo("BinMeshWriterCmd usage: ... ");
}

bool BinMeshWriterCmd::exportBinMeshes()
{
	asf::GenericMeshFileWriter globalWriter(this->path.asChar());

	ProxyMesh globalProxyMesh(this->percentage);

	// transform
	// single files
	
	for( uint dagPathId = 0; dagPathId < this->exportObjects.length(); dagPathId++)
	{
		MeshWalker walker(this->exportObjects[dagPathId], this->doTransform);
		if( this->oneFilePerMesh )
		{
			// replace filename.binaraymesh with filename_objname.binarymesh
			MString perFileMeshPath = pystring::replace(this->path.asChar(), ".binarymesh", "").c_str();
			perFileMeshPath += makeGoodString(this->exportObjects[dagPathId].partialPathName()) + ".binarymesh";
			logger.debug(MString("BinMeshWriterCmd::exportBinMeshes - exporting ") + this->exportObjects[dagPathId].partialPathName() + " to  " + perFileMeshPath);
			asf::GenericMeshFileWriter writer(perFileMeshPath.asChar());
			writer.write(walker);
			if( this->doProxy )
			{
				ProxyMesh proxyMesh(this->percentage);
				proxyMesh.addMesh(walker);
				MString proxyFileName = pystring::replace(perFileMeshPath.asChar(), ".binarymesh" , ".proxymesh").c_str();
				proxyMesh.writeFile(proxyFileName);
			}
		}else{
			globalWriter.write(walker);
			if( this->doProxy )
				globalProxyMesh.addMesh(walker);
		}
	}	

	if( this->doProxy && (!this->oneFilePerMesh))
	{
		MString proxyFileName = pystring::replace(this->path.asChar(), ".binarymesh" , ".proxymesh").c_str();
		globalProxyMesh.writeFile(proxyFileName);
	}
	return true;
}

MStatus BinMeshWriterCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing BinMeshWriterCmd...");
	logger.setLogLevel(Logging::Debug);
	
	MArgDatabase argData(syntax(), args);

	exportAll = false;
	if( argData.isFlagSet("-all", &stat))
	{
		argData.getFlagArgument("-exportAll", 0, exportAll);
		logger.debug(MString("export all: ") + exportAll);
	}

	path = "";
	if( argData.isFlagSet("-path", &stat))
	{
		argData.getFlagArgument("-path", 0, path);
		logger.debug(MString("path: ") + path);
	}

	this->doProxy = true;
	if( argData.isFlagSet("-doProxy", &stat))
	{
		argData.getFlagArgument("-doProxy", 0, this->doProxy);
		logger.debug(MString("Create proxyFile:") + this->doProxy);
	}

	this->percentage = 0.0f;
	if( argData.isFlagSet("-percentage", &stat))
	{
		double p = 0.0;
		argData.getFlagArgument("-percentage", 0, p);
		percentage = (float)p;
		logger.debug(MString("percentage: ") + percentage);
		if( percentage <= 0.0)
			doProxy = false;
	}

	if( (path == ""))
	{
		MGlobal::displayError("binMeshTranslator failed: no path for export.\n");
		printUsage();
		return  MStatus::kFailure;
	}

	this->oneFilePerMesh = false;
	if( argData.isFlagSet("-oneFilePerMesh", &stat))
	{
		argData.getFlagArgument("-oneFilePerMesh", 0, this->oneFilePerMesh);
		logger.debug(MString("Create one file per mesh: ") + this->oneFilePerMesh);
	}

	this->doTransform = true;
	if( argData.isFlagSet("-doTransform", &stat))
	{
		argData.getFlagArgument("-doTransform", 0, this->doTransform);
		logger.debug(MString("Use transform: ") + this->doTransform);
	}

	getObjectsForExport(args);
	if(this->exportObjects.length() == 0)
	{
		logger.error("No mesh objects for export found.");
		return MStatus::kFailure;
	}

	// if we write all objects into one file, is is not useful to ignore the transformation, so we turn it on.
	if( (!this->oneFilePerMesh) && (this->exportObjects.length() > 1))
	{
		if( this->doTransform != true)
			logger.warning(MString("Do transform is off but we have several meshes in one file and ignoring transform is not useful here -> turning it on."));
		this->doTransform = true;
	}

	exportBinMeshes();
	
	MGlobal::displayInfo("BinMeshWriterCmd done.\n");
	return MStatus::kSuccess;
}

void BinMeshWriterCmd::getObjectsForExport(const MArgList& args)
{
	MStatus status;

	if( exportAll )
	{
		MStatus status;

		MItDag itDag(MItDag::kDepthFirst, MFn::kMesh, &status);

		if (MStatus::kFailure == status) 
		{
			MGlobal::displayError("MItDag::MItDag");
			return;
		}

		for(;!itDag.isDone();itDag.next()) 
		{
			MDagPath dagPath;
			if (MStatus::kFailure == itDag.getPath(dagPath)) 
			{
				MGlobal::displayError("MDagPath::getPath");
				continue;
			}

			if( IsVisible(dagPath) )
			{
				if( dagPath.node().hasFn(MFn::kMesh))
					this->exportObjects.append(dagPath);
			}else{
				logger.debug(MString("Node ") + dagPath.partialPathName() + " is not visible and will not be exported");
			}
		}

	}else{
		MSelectionList list;
		MArgDatabase argData(syntax(), args);
		if( argData.getObjects(list) == MS::kSuccess)
		{
			for ( MItSelectionList listIter( list ); !listIter.isDone(); listIter.next() )
			{
				MDagPath dagPath;
				MObject component;
				listIter.getDagPath( dagPath, component );
				MFnDependencyNode nodeFn;
				nodeFn.setObject(dagPath.node());
				MGlobal::displayInfo(MString("Selected object: ") + nodeFn.name());
				if( dagPath.node().hasFn(MFn::kMesh))
					this->exportObjects.append(dagPath);
			}
			if( this->exportObjects.length() == 0)
			{
				logger.error("Not mesh objects selected for export.");
				return;
			}
		}else{
			logger.error("Nothing selected for export.");
			return;
		}
	}
}