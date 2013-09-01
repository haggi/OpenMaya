#include "binMeshTranslator.h"

#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnDependencyNode.h>
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
							 const MString& opts,
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
	options = opts;
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.fullName().asChar());
		fileName(nameBuffer);
	#else
		fileName = file.fullName();
	#endif

	if (MPxFileTranslator::kExportAccessMode == mode) 
	{
		MGlobal::displayInfo("writer - export all.");
		return exportObjects("all");
	}

	if (MPxFileTranslator::kExportActiveAccessMode == mode) 
	{
		MGlobal::displayInfo("writer - export selected.");
		return exportObjects("selected");
	}

	return MS::kSuccess;
}


MStatus BinMeshTranslator::reader(const MFileObject& file,
							 const MString& opts,
							 MPxFileTranslator::FileAccessMode mode) 
{
	options = opts;
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.fullName().asChar());
		fileName(nameBuffer);
	#else
		fileName = file.fullName();
	#endif
	
	MGlobal::displayInfo("Options " + options);
	
	return this->importObjects();
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


MStatus BinMeshTranslator::importObjects() 
{
	MStatus status;
	MString cmd = MString("import binMeshTranslator; binMeshTranslator.binMeshTranslatorRead(");
	cmd += "'" + fileName + "',";
	cmd += "'" + options + "',";
	cmd += "'read')";

	MGlobal::displayInfo(MString("translator cmd: ") + cmd);
	return MGlobal::executePythonCommand(cmd, true);
}


MStatus BinMeshTranslator::exportObjects(MString mode) 
//Summary:	finds and outputs all selected polygonal meshes in the DAG
//Args   :	os - an output stream to write to
//Returns:  MStatus::kSuccess if the method succeeds
//			MStatus::kFailure if the method fails
{
	MStatus status;
	MString cmd = MString("import binMeshTranslator; binMeshTranslator.binMeshTranslatorWrite(");
	cmd += "'" + fileName + "',";
	cmd += "'" + options + "',";
	cmd += "'" + mode + "')";
	
	MGlobal::displayInfo(MString("translator cmd: ") + cmd);
	return MGlobal::executePythonCommand(cmd, true);
}


