#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MBoundingBox.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "exporter/hsff.h"

static Logging logger;

#include "mtm_meshExporter.h"

MeshExporter::MeshExporter(MObject& meshObject, MString& fileName)
{
	this->meshObject = meshObject;
	this->fileName = fileName;
}

MeshExporter::~MeshExporter()
{}

MBoundingBox MeshExporter::exportIt()
{
	MBoundingBox bbox;
	MStatus stat;

	std::string fname(this->fileName.asChar());
	fname = pystring::replace(fname, ".geo", ".hsff"); // hsff == haggis super file format
	fname = pystring::replace(fname, ".bgeo", ".hsff"); // hsff == haggis super file format
	
	logger.debug(MString("Exporting mesh to.") + fname.c_str());
	
	Hsff outFile(fname);
	
	if( !outFile.good() )
	{
		MGlobal::displayError(MString("Unable to open ") + MString(fname.c_str()) + " for export.");
		return bbox;
	}
	outFile.write(HSFF); // file identifier
	outFile.write(MESH); // mesh identifier

	MFnMesh meshFn(this->meshObject);
	MItMeshPolygon faceIt(this->meshObject);
	
	MPoint min, max;
	MPointArray points;
	meshFn.getPoints(points);
	outFile.write(points.length());

	outFile.write(points, min, max);
	bbox.expand(min);
	bbox.expand(max);

    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );
	outFile.write(normals.length());
	outFile.write(normals);

	outFile.write(meshFn.numPolygons());
			
	bool hasUvs = meshFn.numUVSets() > 0;
	hasUvs = meshFn.numUVs() > 0;

	MString currentUVSet;
	if( hasUvs )
		meshFn.getCurrentUVSetName(currentUVSet);	

	MFloatArray uCoords, vCoords;
	if( hasUvs )
	{
		stat = meshFn.getUVs(uCoords, vCoords, &currentUVSet);
		if( stat != MStatus::kSuccess )
			hasUvs = false;
	}
	if( hasUvs )
	{
		outFile.write(uCoords.length());
		outFile.write(vCoords.length());
		outFile.write(uCoords);
		outFile.write(vCoords);
	}else{
		outFile.write((uint)0);
		outFile.write((uint)0);
	}		

	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		MIntArray uvIds;
		MIntArray normalIds;
		MIntArray vtxIds;
		uint faceIndex = faceIt.index();
		int numVtx = faceIt.polygonVertexCount();
		bool faceHasUvs = hasUvs;
		for ( int vtx=0; vtx<numVtx; vtx++ ) 
		{
					//fprintf(fp," %d", faceIt.vertexIndex( vtx ) +1 +voff);
			if( hasUvs )
			{
				int uvIndex;

				// save uv coords because we have to reverse them for houdini
				// it is possible that there are no uvs for this polygon...
				if ( faceIt.getUVIndex(vtx, uvIndex, &currentUVSet) ) 
				{
					uvIds.append(uvIndex);						
				}else{
					faceHasUvs = false;
				}
			}
			normalIds.append(faceIt.normalIndex( vtx ));
			vtxIds.append(faceIt.vertexIndex(vtx));
		}
		// write len of normals and vertex ids
		outFile.write(vtxIds.length());
		MIntArray vtxIdsR;
		for( uint vid = 0; vid < vtxIds.length(); vid++)
			vtxIdsR.append(vtxIds[vtxIds.length() - 1 - vid]);
		
		MIntArray normIdsR;
		for( uint vid = 0; vid < normalIds.length(); vid++)
		{
			MVector v = normals[normalIds[vid]];
			normIdsR.append(normalIds[normalIds.length() - 1 - vid]);
		}
		
		outFile.write(vtxIdsR);
		outFile.write(normIdsR);
		// write boolean for uvs
		outFile.write(faceHasUvs);
		if( faceHasUvs )
		{
			// reverse uv ids
			MIntArray uvIdsR;
			for( uint uvId = 0; uvId < uvIds.length(); uvId++)
				uvIdsR.append(uvIds[uvIds.length() - 1 - uvId]);
			outFile.write(uvIdsR);
		}
	}

	outFile.close();


	//MFnMesh meshFn(this->meshObject);
	//MItMeshPolygon faceIt(this->meshObject);
	//MPointArray points;
	//meshFn.getPoints(points);
	//std::vector<GPD_Point *> gpdPointArray;
	////meshFn.getNormals(normals);
	//MString currentUVSet;
	//meshFn.getCurrentUVSetName(currentUVSet);
	////meshFn.getUVs(uCoord, vCoord, &currentUVSet);

	//GPD_Detail	 	*gdp;
 //   GPD_PrimPoly	*poly;
 //   gdp = new GPD_Detail;
	//float defaultVector[3] = {0.0, 0.0, 0.0};

	////int numPoints = points.length();
	////float *pointList = (float *)malloc( numPoints * 3 * sizeof(float));
	////int pcount = 0;
	////for( int pointId = 0; pointId < numPoints; pointId++)
	////{
	////	MPoint p = points[pointId];
	////	bbox.expand(p);
	////	pointList[pcount++] = p.x;
	////	pointList[pcount++] = p.y;
	////	pointList[pcount++] = p.z;
	////	//GPD_Point *dpdPoint = new GPD_Point(p.x, p.y, p.z);
	////	//gpdPointArray.push_back(dpdPoint);
	////}
	////gdp->createPoints(pointList, numPoints);


	//for( int pointId = 0; pointId < points.length(); pointId++)
	//{
	//	MPoint p = points[pointId];
	//	bbox.expand(p);
	//	GPD_Point *dpdPoint = new GPD_Point(p.x, p.y, p.z);
	//	gpdPointArray.push_back(dpdPoint);
	//	gdp->createPoint(dpdPoint);
	//}

	//GPD_Attribute *normalAtt = gdp->createVertexAttribute("N", 3, GPD_ATTRIB_VECTOR, &defaultVector);
	//GPD_Attribute *uvAtt = gdp->createVertexAttribute("uv", 3, GPD_ATTRIB_FLOAT, &defaultVector);

	//for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	//{
	//	MIntArray pointIds;
	//	MVectorArray normals;
	//	MFloatArray uCoord, vCoord;
	//	faceIt.getVertices(pointIds);
	//	faceIt.getNormals(normals);
	//	faceIt.getUVs(uCoord, vCoord, &currentUVSet);

	//	poly = (GPD_PrimPoly *)gdp->buildPrimitive(GPDPRIMPOLY);
	//	int numVertices =  pointIds.length();
	//	for( int id = 0; id < numVertices; id++)
	//	{
	//		int idd = numVertices - 1 - id; 
	//		float *n = new float[3];
	//		n[0] = normals[idd].x;
	//		n[1] = normals[idd].y;
	//		n[2] = normals[idd].z;
	//		float *uvn = new float[3];
	//		uvn[0] = uCoord[idd];
	//		uvn[1] = vCoord[idd];
	//		uvn[2] = 0.0;
	//		int ptId = pointIds[idd];
	//		//MString info = MString("PointId:") + ptId + "gpd len:" + gpdPointArray.size() + " ptsize " + points.length();
	//		//MGlobal::displayInfo(info);
	//		GPD_Point *pt = gpdPointArray[ptId];
	//		GPD_Vertex *v = poly->addPoint(pt);
	//		v->setAttribute(normalAtt, n);
	//		v->setAttribute(uvAtt, uvn);
	//		//poly->getVertex(ptId)->setAttribute(normalAtt, n);
	//		//poly->getVertex(ptId)->setAttribute(uvAtt, uvn);
	//	}
	//}

	//MGlobal::displayInfo("Exporting mesh to " + this->fileName);
	//gdp->save(this->fileName.asChar());

	return bbox;
}

    
//point_positions = ((0,0,0), (1,0,0), (1,1,0), (0,1,0))
//poly_point_indices = ((0,1,2), (2,3,0))
//
//geo = hou.pwd().geometry()
//
//# Create all the points.
//points = []
//for position in point_positions:
//    points.append(geo.createPoint())
//    points[-1].setPosition(position)
//
//# Now create the polygons, adding vertices that refer to the points.
//for point_indices in poly_point_indices:
//    poly = geo.createPolygon()
//    for point_index in point_indices:
//        poly.addVertex(points[point_index])
//
//geo = hou.pwd().geometry()
//poly = geo.createPolygon()
//for position in (0,0,0), (1,0,0), (0,1,0):
//    point = geo.createPoint()
//    point.setPosition(position)
//    poly.addVertex(point)