#include "mtap_standinMeshNode.h"

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>
#include <maya/MBoundingBox.h>

#include <vector>

#include "renderer/api/object.h"
#include "renderer/modeling/object/meshobjectreader.h"
#include "foundation/utility/searchpaths.h"

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;

namespace asf = foundation;
namespace asr = renderer;

MStatus returnStatus;

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}


MTypeId mtap_standinMeshNode::id( 0x0011CF42 );
								  

MObject mtap_standinMeshNode::time;
MObject mtap_standinMeshNode::outputMesh;
MObject mtap_standinMeshNode::binMeshFile;	
MObject mtap_standinMeshNode::polySizeMultiplier;

void* mtap_standinMeshNode::creator()
{
	return new mtap_standinMeshNode;
}

MStatus mtap_standinMeshNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute tAttr;
    MFnNumericAttribute nAttr; 

	MStatus returnStatus;

	time = unitAttr.create( "time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus );
	CHECK_MSTATUS(addAttribute( time ));

	outputMesh = tAttr.create( "outputMesh", "out", MFnData::kMesh, &returnStatus ); 
	tAttr.setStorable(false);
	CHECK_MSTATUS(addAttribute( outputMesh ));

	binMeshFile = tAttr.create( "binMeshFile", "binMeshFile", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( binMeshFile ));

	polySizeMultiplier = nAttr.create( "polySizeMultiplier", "polySizeMultiplier", MFnNumericData::kFloat, 1.0f);
	nAttr.setMin(0.01);
	nAttr.setSoftMax(10.0);
	CHECK_MSTATUS(addAttribute( polySizeMultiplier ));

	//returnStatus = attributeAffects(time, outputMesh);
	//CHECK_MSTATUS(returnStatus);

	returnStatus = attributeAffects(polySizeMultiplier, outputMesh);
	returnStatus = attributeAffects(binMeshFile, outputMesh);
	CHECK_MSTATUS(returnStatus);

	return MS::kSuccess;
}


MObject mtap_standinMeshNode::createMesh(const MTime& time,
							  MObject& outData,
							  MStatus& stat)

{
	int					numVertices;
	float				cubeSize;
	MFloatPointArray	points;
	MFnMesh				meshFS;

	MString proxyFile = this->binmesh_file;
	proxyFile = pystring::replace(this->binmesh_file.asChar(), ".binarymesh" , ".proxymesh").c_str();

	MGlobal::displayInfo(MString("Trying to find proxy mesh file for binmesh file: ") + this->binmesh_file + " --> " + proxyFile);

	pFile.open(proxyFile.asChar(),  std::ios_base::in | std::ios_base::binary);

	MObject newMesh;

	if( pFile.good() )
	{
		int numPoints = 0;
		MBoundingBox box;
		MPoint min, max;
		int numShaders = 0;
		MStringArray shaderNames;
		MString shaderName;
		int numFaces;

		read(numShaders);
		MGlobal::displayInfo(MString("Object has: ") + numShaders + " shading groups.");
		for( int i = 0; i < numShaders; i++)
		{
			read(shaderName);
			MGlobal::displayInfo(MString("Read shader name from file: ") + shaderName);
			std::vector<int> shaderAssingments;
			pma.assignments.push_back(shaderAssingments);
			pma.shadingGroupsNames.append(shaderName);
			shadingEngineNames.append(shaderName);
		}

		read(numFaces);
		MGlobal::displayInfo(MString("Object has: ") + numFaces + " shading ids.");
		for( int i = 0; i < numFaces; i++)
		{
			int shaderId = 0;
			read(shaderId);
			polyShaderIds.append(shaderId);
			if( shaderId >= numShaders)
			{
				MGlobal::displayInfo(MString("Shader Id not in shader list: Face: ") + i + " shader id " + shaderId);
				pma.assignments[shaderId].push_back(0);
			}
			else
				pma.assignments[shaderId].push_back(i);
		}

		read(numPoints);
		numVertices = numPoints;
		numFaces = numPoints / 3;
		read(min);
		read(max);
		MGlobal::displayInfo(MString("File successfully opended. File contains: ") + numPoints + " points what means " + numFaces + " triangles");
		MPointArray points(numPoints);
		read(points);		
		pFile.close();

		if( this->poly_size_multiplier != 1.0f)
		{
			for( int f = 0; f < numFaces; f++)
			{
				MPoint p0 = points[f * 3];
				MPoint p1 = points[f * 3 + 1];
				MPoint p2 = points[f * 3 + 2];
				MPoint center = (p0 + p1 + p2)/ 3.0;
				p0 = ((p0 - center) * poly_size_multiplier) + center;
				p1 = ((p1 - center) * poly_size_multiplier) + center;
				p2 = ((p2 - center) * poly_size_multiplier) + center;
				points[f * 3] = p0;
				points[f * 3 + 1] = p1;
				points[f * 3 + 2] = p2;
			}
		}

		// create poly structure
		MIntArray faceCounts(numFaces, 3);
		MIntArray faceConnects(numPoints);
		for( uint i = 0; i < numPoints; i++)
			faceConnects[i] = i;
		newMesh = meshFS.create(numVertices, numFaces, points, faceCounts, faceConnects, outData, &stat);
		if( !stat )
			MGlobal::displayError("Mesh creation failure.");		
	}

	return newMesh;
}

void mtap_standinMeshNode::createMaterialAssignments()
{
	MString idListString = "[";
	for( size_t i = 0; i < pma.assignments.size(); i++)
	{
		MGlobal::displayInfo(MString("Shading Group: ") + pma.shadingGroupsNames[i] + " is connected to: " + pma.assignments[i].size() + " polygons.");
		idListString += "[";
		IDList ids = pma.assignments[i];
		for( size_t k = 0; k < ids.size(); k++)
		{
			idListString += ids[k];
			if( k < (ids.size() - 1))
				idListString += ",";
		}			
		idListString += "]";
		if( i < (pma.assignments.size() - 1))
			idListString += ",";
	}
	idListString += "]";

	MString thisObjectName = getObjectName(this->thisMObject());
	MString shadingEngineList = "[";		
	for( uint seId = 0; seId < shadingEngineNames.length(); seId++)
	{
		shadingEngineList += "'" + shadingEngineNames[seId] + "'";
		if( seId < (shadingEngineNames.length() - 1))
			shadingEngineList += ",";
	}
	shadingEngineList += "]";

	//def binMeshAssignShader(polyShape = None, shadingGroupList=[], perFaceAssingments=[]):
	MString pythonCmd = MString("import binMeshTranslator as bmt; bmt.binMeshAssignShader( creatorShape = '")  + thisObjectName + "', shadingGroupList = " +  shadingEngineList + ", perFaceAssingments = " + idListString + ")";
	MGlobal::displayInfo(pythonCmd);
	//MGlobal::executePythonCommand(pythonCmd);
	MGlobal::executePythonCommandOnIdle(pythonCmd);
}

bool mtap_standinMeshNode::checkMeshFileName(MString meshFileName)
{
	// do we have content
	// we need at least .binarymesh == 11 characters  
	if( meshFileName.length() < 11 )
	{
		cerr << "mesh file name has less than 11 characters, what means it has no .binarymesh ending.\n";
		return false;
	}

	return true;
}

MStatus mtap_standinMeshNode::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus returnStatus;

	if (plug == outputMesh) {
		/* Get time */
		MDataHandle dataHandle = data.inputValue( time, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		MTime time = dataHandle.asTime();

		dataHandle = data.inputValue( polySizeMultiplier, &returnStatus ); 
		McheckErr(returnStatus, "Error getting polySizeMultiplier data handle\n");
		this->poly_size_multiplier = dataHandle.asFloat();

		dataHandle = data.inputValue( binMeshFile, &returnStatus ); 
		McheckErr(returnStatus, "Error getting binMeshFile handle\n");
		MString fileName = dataHandle.asString();

		if( !checkMeshFileName(fileName))
			McheckErr(MS::kFailure, "Filename problem\n");

		this->binmesh_file = fileName;

		/* Get output object */
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		if (createMesh(time, newOutputData, returnStatus) == MObject::kNullObj)
			return MS::kFailure;

		McheckErr(returnStatus, "ERROR creating new geo");

		outputHandle.set(newOutputData);
		data.setClean( plug );

		createMaterialAssignments();

	} else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

