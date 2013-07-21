#include "toolsBinMesh.h"

#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>

#include <maya/MIOStream.h>
#include <maya/MFStream.h>

#include "utilities/tools.h"

BinMeshTranslator::BinMeshTranslator()
{
}


BinMeshTranslator::~BinMeshTranslator() 
{ 
}

MString BinMeshTranslator::defaultExtension() const
{ 
	return "binarymesh";
}

void* BinMeshTranslator::creator() 
{
	return new BinMeshTranslator();
}

MStatus BinMeshTranslator::writer(const MFileObject& file,
							 const MString& /*options*/,
							 MPxFileTranslator::FileAccessMode mode) 
//Summary:	saves a file of a type supported by this translator by traversing
//			the all or selected objects (depending on mode) in the current
//			Maya scene, and writing a representation to the given file
//Args   :	file - object containing the pathname of the file to be written to
//			options - a string representation of any file options 
//			mode - the method used to write the file - export, or export active
//				   are valid values; method will fail for any other values 
//Returns:	MStatus::kSuccess if the export was successful;
//			MStatus::kFailure otherwise
{
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.fullName().asChar());
		const MString fileName(nameBuffer);
	#else
		const MString fileName = file.fullName();
	#endif


	MGlobal::displayInfo("Export to " + fileName + " successful!");
	return MS::kSuccess;
}


MStatus BinMeshTranslator::reader(const MFileObject& file,
							 const MString& /*options*/,
							 MPxFileTranslator::FileAccessMode mode) 
{
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.fullName().asChar());
		const MString fileName(nameBuffer);
	#else
		const MString fileName = file.fullName();
	#endif


	MGlobal::displayInfo("Read from " + fileName + " successful!");
	return MS::kSuccess;
}

bool BinMeshTranslator::haveWriteMethod() const 
//Summary:	returns true if the writer() method of the class is implemented;
//			false otherwise
//Returns:  true since writer() is implemented in this class
{
	return true;
}


bool BinMeshTranslator::haveReadMethod() const 
//Summary:	returns true if the reader() method of the class is implemented;
//			false otherwise
//Returns:  false since reader() is not implemented in this class
{
	return true;
}


bool BinMeshTranslator::canBeOpened() const 
//Summary:	returns true if the translator can open and import files;
//			false if it can only import files
//Returns:  true
{
	return true;
}


MStatus BinMeshTranslator::exportAll() 
{
	MStatus status;

	//create an iterator for only the mesh components of the DAG
	MItDag itDag(MItDag::kDepthFirst, MFn::kMesh, &status);

	if (MStatus::kFailure == status) 
	{
		MGlobal::displayError("MItDag::MItDag");
		return MStatus::kFailure;
	}

	for(;!itDag.isDone();itDag.next()) 
	{
		MDagPath dagPath;
		if (MStatus::kFailure == itDag.getPath(dagPath)) 
		{
			MGlobal::displayError("MDagPath::getPath");
			return MStatus::kFailure;
		}

		MFnDagNode visTester(dagPath);

		//if this node is visible, then process the poly mesh it represents
		//
		if(IsVisible(visTester) && MStatus::kSuccess == status) 
		{
			// add to mesh walker
		}
	}
	return MStatus::kSuccess;
}


MStatus BinMeshTranslator::exportSelection() 
//Summary:	finds and outputs all selected polygonal meshes in the DAG
//Args   :	os - an output stream to write to
//Returns:  MStatus::kSuccess if the method succeeds
//			MStatus::kFailure if the method fails
{
	MStatus status;

	//create an iterator for the selected mesh components of the DAG
	//
	MSelectionList selectionList;
	if (MStatus::kFailure == MGlobal::getActiveSelectionList(selectionList)) 
	{
		MGlobal::displayError("MGlobal::getActiveSelectionList");
		return MStatus::kFailure;
	}

	MItSelectionList itSelectionList(selectionList, MFn::kMesh, &status);	
	if (MStatus::kFailure == status) 
	{
		return MStatus::kFailure;
	}

	for (itSelectionList.reset(); !itSelectionList.isDone(); itSelectionList.next()) 
	{
		MDagPath dagPath;

		//get the current dag path and process the poly mesh on it
		//
		if (MStatus::kFailure == itSelectionList.getDagPath(dagPath)) 
		{
			MGlobal::displayError("MItSelectionList::getDagPath");
			return MStatus::kFailure;
		}
		// add to mesh walker
	}
	return MStatus::kSuccess;
}


