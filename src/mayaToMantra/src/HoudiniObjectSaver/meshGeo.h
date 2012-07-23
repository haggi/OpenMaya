#include "hsff.h"
#include "pystring.h"
#include <fstream>
#include <stdio.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <GEO/GEO_PrimPoly.h>


typedef unsigned int uint;


//void addPolygon(GU_Detail* gdp)
//{
//    int point_idx;
//    const int num_points = 3;
//    UT_Vector3 positions[num_points];
//    GEO_PrimPoly *prim_poly_ptr;
//    GEO_Point* added_points[num_points];
//
//    // Set positions for the points of the polygon
//    positions[0].assign(0, 0, 0);
//    positions[1].assign(10, 0, 0);
//    positions[2].assign(0, 10, 0);
//
//    // Add new points to the gdp. This is optional - you can also
//    // reuse existing points and add them to the polygon instead.
//    for(point_idx = 0; point_idx < num_points; point_idx++)
//    {
//        // Append a new point
//        added_points[point_idx] = gdp->appendPoint();
//        // Set its coordinates
//        added_points[point_idx]->getPos().assign(positions[point_idx][0],positions[point_idx][1],
//            positions[point_idx][2], 1.0);
//    }
//
//    // Append a new polygon primitive. Primitive types are defined in GEO_PrimType.h
//    prim_poly_ptr = dynamic_cast<GEO_PrimPoly *>(gdp->appendPrimitive(GEOPRIMPOLY));
//    prim_poly_ptr->setSize(0);
//
//    // Append vertices that form this primitive
//    for(point_idx = 0; point_idx < num_points; point_idx++)
//        prim_poly_ptr->appendVertex(added_points[point_idx]);
//
//    // Close the primitive to make it a polygon instead of a polyline.
//    prim_poly_ptr->close();
//}

void createMeshGeo(Hsff& inFile)
{
	GU_Detail	gdp;
	GU_PrimPoly *poly;
	GEO_PrimPoly *prim_poly_ptr;

	uint numPoints = inFile.readUInt();
	//poly = GU_PrimPoly::build(&gdp, numPoints, GU_POLY_CLOSED);
	//poly = GU_PrimPoly::build(&gdp, 0);
	//poly->setSize(numPoints);

	GEO_Point *geoPPtr;
	double *vertices = (double *)malloc(sizeof(double) * 3 * numPoints);
	GEO_Point **ptList = (GEO_Point **)malloc(sizeof(geoPPtr) * numPoints);

	inFile.readDouble( vertices, numPoints * 3);
	
	for( uint ptId = 0; ptId < numPoints; ptId++)
	{
		uint index = ptId * 3;
		//poly->getVertex(ptId).getPt()->setPos(vertices[index],vertices[index+1],vertices[index+2]);
		GEO_Point *p = gdp.appendPoint();
		p->setPos(vertices[index],vertices[index+1],vertices[index+2]);
		ptList[ptId] = p;
	}
	free(vertices);

	uint numNormals = inFile.readUInt();
	double *normals = (double *)malloc(sizeof(double) * 3 * numNormals);
	inFile.readDouble( normals, numNormals * 3);

	uint numPolys = inFile.readUInt();
	uint numUs = inFile.readUInt(); // uv lengths
	uint numVs = inFile.readUInt();
	double *uCoords = (double *)malloc(sizeof(double) * numUs);
	double *vCoords = (double *)malloc(sizeof(double) * numVs);
	inFile.readDouble( uCoords, numUs);
	inFile.readDouble( vCoords, numVs);

	GA_RWAttributeRef normalRef = gdp.addFloatTuple(GA_ATTRIB_POINT, "N", 3);
	if (normalRef.isValid()) normalRef.getAttribute()->setTypeInfo(GA_TYPE_NORMAL);
	GEO_AttributeHandle normalHandle = gdp.getVertexAttribute("N");
	
	float uvs[3];
	uvs[0] = 1;
	uvs[1] = 2;
	uvs[2] = 3;
	GA_RWAttributeRef uvRef = gdp.addFloatTuple(GA_ATTRIB_POINT, "uv", 3);
	GEO_AttributeHandle uvHandle = gdp.getVertexAttribute("uv");
	
	for( uint fId = 0; fId < numPolys; fId++)
	{
		int numVtx = inFile.readUInt();
		uint *vtxIds = (uint *)malloc(sizeof(uint) * numVtx);
		uint *normalIds = (uint *)malloc(sizeof(uint) * numVtx);
		inFile.readInt(vtxIds, numVtx);
		inFile.readInt(normalIds, numVtx);

		uint *uvIds = NULL;
		int faceHasUvs = inFile.readUInt();
		if( faceHasUvs == 1)
		{
			uvIds = (uint *)malloc(sizeof(uint) * numVtx);
			inFile.readInt(uvIds, numVtx);
		}

	    prim_poly_ptr = dynamic_cast<GEO_PrimPoly *>(gdp.appendPrimitive(GEO_PRIMPOLY));
	    //prim_poly_ptr = dynamic_cast<GEO_PrimPoly *>(gdp.appendPrimitive(GEOPRIMPOLY));
	    prim_poly_ptr->setSize(0);
	
	    //Append vertices that form this primitive
	    for(uint vtxId = 0; vtxId < numVtx; vtxId++)
		{
			GEO_Point *pt = ptList[vtxIds[vtxId]];
	        prim_poly_ptr->appendVertex(pt);
			uint startId = normalIds[vtxId] * 3;
			double *startPtr = &normals[startId];
			UT_Vector3 n(startPtr[0],startPtr[1],startPtr[2]);
			prim_poly_ptr->getVertex(vtxId).setValue<UT_Vector3>(normalRef, n);

			if( faceHasUvs )
			{
				float uv[3];
				uv[0] = uCoords[uvIds[vtxId]];
				uv[1] = vCoords[uvIds[vtxId]];
				uv[2] = 0.0;
				prim_poly_ptr->getVertex(vtxId).set<float>(uvRef, uv, 3);
			}
		}	
	    //Close the primitive to make it a polygon instead of a polyline.
	    prim_poly_ptr->close();

		if( uvIds != NULL) free(uvIds);
		free(vtxIds);
		free(normalIds);
	}
	free(ptList);
	free(normals);
	free(uCoords);
	free(vCoords);

	if( inFile.doBinary )
	{
		std::string outFile = pystring::replace(inFile.fileName, ".hsff", ".bgeo");
		cout << "Writing to file " << outFile << "\n";
		gdp.save(outFile.c_str(), NULL, NULL);
	}else{
		std::string outFile = pystring::replace(inFile.fileName, ".hsff", ".geo");
		cout << "Writing to file " << outFile << "\n";
		gdp.save(outFile.c_str(), NULL, NULL);
	}

}

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

//            poly = GU_PrimPoly::build(gdp, divisions, GU_POLY_CLOSED);
//            tinc = M_PI*2 / (float)divisions;
//
//            // Now, set all the points of the polygon
//            for (i = 0; i < divisions; i++)
//            {
//                // Check to see if the user has interrupted us...
//                if (boss->opInterrupt())
//                    break;
//
//                myCurrPoint = i;
//
//                // Since we expect the local variables to be used in specifying
//                // the radii, we have to evaluate the channels INSIDE the loop
//                // through the points...
//
//                tmp = (float)i * tinc;
//                rad = (i & 1) ? XRADIUS(now) : YRADIUS(now);
//                if (!negradius && rad < 0)
//                    rad = 0;
//
//                ppt = poly->getVertex(i).getPt();
//                ppt->getPos()(xcoord) = cos(tmp) * rad + tx;
//                ppt->getPos()(ycoord) = sin(tmp) * rad + ty;
//                ppt->getPos()(zcoord) = 0 + tz;
//            }
//
