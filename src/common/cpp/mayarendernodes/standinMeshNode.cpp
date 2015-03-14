#include "standinMeshNode.h"

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>
#include <maya/MBoundingBox.h>

#include "utilities/pystring.h"
#include "utilities/logging.h"

static Logging logger;

MStatus returnStatus;

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MObject standinMeshNode::time;
MObject standinMeshNode::outputMesh;
MObject standinMeshNode::binMeshFile;	
MObject standinMeshNode::percentDisplay;	
MObject standinMeshNode::polySizeMultiplier;

void* standinMeshNode::creator()
{
	return new standinMeshNode;
}

MStatus standinMeshNode::initialize()
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

	percentDisplay = nAttr.create( "percentDisplay", "percentDisplay", MFnNumericData::kFloat, 0.1f);
	nAttr.setMin(0.01);
	nAttr.setSoftMax(1.0);
	CHECK_MSTATUS(addAttribute( percentDisplay ));

	polySizeMultiplier = nAttr.create( "polySizeMultiplier", "polySizeMultiplier", MFnNumericData::kFloat, 1.0f);
	nAttr.setMin(0.01);
	nAttr.setSoftMax(10.0);
	CHECK_MSTATUS(addAttribute( polySizeMultiplier ));

	//returnStatus = attributeAffects(time, outputMesh);
	//CHECK_MSTATUS(returnStatus);

	returnStatus = attributeAffects(polySizeMultiplier, outputMesh);
	returnStatus = attributeAffects(binMeshFile, outputMesh);
	CHECK_MSTATUS(returnStatus);

	//returnStatus = attributeAffects(percentDisplay, outputMesh);
	//CHECK_MSTATUS(returnStatus);

	return MS::kSuccess;
}

MObject standinMeshNode::createMesh(const MTime& time,
							  MObject& outData,
							  MStatus& stat)

{
	int					numVertices, frame;
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
		read(numPoints);
		numVertices = numPoints;
		uint numFaces = numPoints / 3;
		read(min);
		read(max);
		MGlobal::displayInfo(MString("File successfully opended. File contains: ") + numPoints + " points what means " + numFaces + " triangles");
		MPointArray points(numPoints);
		read(points);		
		pFile.close();

		if( this->poly_size_multiplier != 1.0f)
		{
			for( uint f = 0; f < numFaces; f++)
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

bool standinMeshNode::checkMeshFileName(MString meshFileName)
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

MStatus standinMeshNode::compute(const MPlug& plug, MDataBlock& data)

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
	} else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

