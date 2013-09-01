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


MStatus BinMeshTranslator::getMeshObjectInHierarchy(MDagPath currentPath, MObjectArray& objectArray)
{
	if( currentPath.node().hasFn(MFn::kMesh))
		objectArray.append(currentPath.node());

	uint numChilds = currentPath.childCount();
	for( uint chId = 0; chId < numChilds; chId++)
	{
		MDagPath childPath = currentPath;
		MStatus stat = childPath.push(currentPath.child(chId));
		if( !stat )
		{
			continue;
		}
		MString childName = childPath.fullPathName();
		getMeshObjectInHierarchy(childPath, objectArray);
	}

	return MStatus::kSuccess;
}

//bool MayaScene::parseSceneHierarchy(MDagPath currentPath, int level, ObjectAttributes *parentAttributes, MayaObject *parentObject)
//{
//	logger.debug(MString("parse: ") + currentPath.fullPathName(), level);
//	MayaObject *mo = mayaObjectCreator(currentPath);
//	ObjectAttributes *currentAttributes = mo->getObjectAttributes(parentAttributes);
//	mo->parent = parentObject;
//	classifyMayaObject(mo);
//
//	// 
//	//	find the original mayaObject for instanced objects. Can be useful later.
//	//
//
//	if( currentPath.instanceNumber() == 0)
//		origObjects.push_back(mo);
//	else{
//		MFnDagNode node(currentPath.node());
//		for( size_t iId = 0; iId < origObjects.size(); iId++)
//		{
//			MFnDagNode onode(origObjects[iId]->mobject);
//			if( onode.object() == node.object() )
//			{
//				logger.debug(MString("Orig Node found:") + onode.fullPathName(), level);
//				mo->origObject = origObjects[iId];
//				break;
//			}
//		}
//	}
//
//	uint numChilds = currentPath.childCount();
//	for( uint chId = 0; chId < numChilds; chId++)
//	{
//		MDagPath childPath = currentPath;
//		MStatus stat = childPath.push(currentPath.child(chId));
//		if( !stat )
//		{
//			logger.debug(MString("Child path problem: parent: ") + currentPath.fullPathName() + " child id " + chId + " type " + currentPath.child(chId).apiTypeStr());
//			continue;
//		}
//		MString childName = childPath.fullPathName();
//		parseSceneHierarchy(childPath, level + 1, currentAttributes, mo);
//	}
//
//	return true;
//}


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
		exportAll();
	}

	if (MPxFileTranslator::kExportActiveAccessMode == mode) 
	{
		MGlobal::displayInfo("writer - export selected.");
		exportSelection();
	}

	MGlobal::displayInfo("Export to " + fileName + " successful!");
	return MS::kSuccess;
}


MStatus BinMeshTranslator::reader(const MFileObject& file,
							 const MString& options,
							 MPxFileTranslator::FileAccessMode mode) 
{
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.fullName().asChar());
		const MString fileName(nameBuffer);
	#else
		const MString fileName = file.fullName();
	#endif
	
	MGlobal::displayInfo("Options " + options);

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
	MString cmd = MString("python(\"import binMeshTranslator; binMeshTranslator.binMeshTranslatorWrite(");
	cmd += "\"" + fileName + "\",";
	cmd += "\"" + options + "\",";
	cmd += ",\"selection\")";
	

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
	
	MObjectArray objectArray;

	for (itSelectionList.reset(); !itSelectionList.isDone(); itSelectionList.next()) 
	{
		MDagPath dagPath;

		if (MStatus::kFailure == itSelectionList.getDagPath(dagPath)) 
		{
			MGlobal::displayError("MItSelectionList::getDagPath");
			continue;
		}

		MGlobal::displayInfo(MString("Translator: Object - ") + dagPath.fullPathName());
		getMeshObjectInHierarchy(dagPath, objectArray);

		// add to mesh walker
	}

	for( uint i = 0; i < objectArray.length(); i++)
	{
		MFnDependencyNode depFn(objectArray[i]);
		MGlobal::displayInfo(MString("Translator: Object - ") + depFn.name());
	}


	return MStatus::kSuccess;
}


