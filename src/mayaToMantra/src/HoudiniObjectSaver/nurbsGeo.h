#include "hsff.h"
#include "pystring.h"
#include <fstream>
#include <stdio.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimNURBSurf.h>
#include <GU/GU_PrimNURBCurve.h>


typedef unsigned int uint;


void createNurbsGeo(Hsff& inFile)
{
	GU_Detail	gdp;
	GU_PrimNURBSurf *surf;
	// read count of vertices in U and V direction
	uint numCv = inFile.readUInt();
	uint numU = inFile.readUInt();
	uint numV = inFile.readUInt();
	uint degU = inFile.readUInt();
	uint degV = inFile.readUInt();
	uint numKnotsU = inFile.readUInt();
	uint numKnotsV = inFile.readUInt();
	
	double *knotsU = (double *)malloc(sizeof(double) * numKnotsU);
	double *knotsV = (double *)malloc(sizeof(double) * numKnotsV);
	inFile.readDouble(knotsU, numKnotsU);
	inFile.readDouble(knotsV, numKnotsV);
	//cout << "Nurbs with " << numU << " cvs in U and " << numV << " cvs in V " << " results in " << numU * numV << " points \n";
	

	double *points = (double *)malloc(sizeof(double) * 3 *numCv);
	inFile.readDouble(points, numCv * 3);

	uint formU = inFile.readUInt();
	uint formV = inFile.readUInt();
	
	uint formUU = formU == 1 ? 0 : 1;
	uint formVV = formV == 1 ? 0 : 1;


	float *ubasis = (float *)malloc(numKnotsU * sizeof(float));
	float *vbasis = (float *)malloc(numKnotsV * sizeof(float));

	// im Linearen fall
	int knotStartU = degU - 1;
	int knotEndU = numKnotsU - degU + 1;
	int knotStartV = degV - 1;
	int knotEndV = numKnotsV - degV + 1;

	int numKU = knotEndU - knotStartU;
	int numKV = knotEndV - knotStartV;
	int realNumU = numU;
	int realNumV = numV;

	if( formU )
	{
		knotStartU = 0;
		knotEndU = numKnotsU - degU;
		numKU = numKnotsU - degU;
		realNumU -= degU;
	}
	if( formV )
	{
		knotStartV = 0;
		knotEndV = numKnotsV - degV;
		numKV = numKnotsV - degV;
		realNumV -= degV;
	}
	//cout << "NumU " << numU << " realU " << realNumU << " numV " << numV << " realV " << realNumV << "\n";
	float *fstart = ubasis;
	//cout << "NumBaseU " << knotEndU - knotStartU << " numbasev " << knotEndV-knotStartV << " numknotsu " << numKnotsU << " numknotsv " << numKnotsV << "\n";
	for( int i = knotStartU; i < knotEndU; i++)
		*fstart++ = (float)knotsU[i];

	fstart = vbasis;
	for( int i = knotStartV; i < knotEndV; i++)
		*fstart++ = (float)knotsV[i];


	int orderU = degU + 1;
	int orderV = degV + 1;
	int numkuo = numU - orderU + 2; //open
	int numkvo = numV - orderV + 2;
	int numkuc = numU - orderU + 3; //close
	int numkvc = numV - orderV + 3;
	int numkup = numU + orderU - 2; //peri
	int numkvp = numV + orderV - 2;

	int end = 1;
	int noend = 2;
	int endCondU = 1;
	int endCondV = 1;

	bool openU = false;
	bool openV = false;
	if( formU )
	{		
		endCondU = 0;
	}else
		openU = true;

	if( formV )
	{		
		endCondV = 0;
	}else
		openV = true; 

	surf = GU_PrimNURBSurf::build(&gdp, realNumV, realNumU, degU + 1, degV +1, openU, openV, endCondU, endCondV );
	surf->setRowCol(realNumV, realNumU);

	if( !openU )
		surf->wrapU();
	else
		surf->openU();
	if( !openV )
		surf->wrapV();
	else
		surf->openV();

	for( int uId = 0; uId < realNumU; uId++)
	{
		for( int vId = 0; vId < realNumV; vId++)
		{
			int index = (numV * uId + vId) * 3;
			int ptIndex = numV * uId + vId;
			UT_Vector4 vec;
			
			vec.assign(points[index],points[index+1],points[index+2]);
			surf->getVertex(vId, uId).getPt()->setPos(vec);
		}
	}
	GA_Basis *ubase = surf->getUBasis();
	GA_Basis *vbase = surf->getVBasis();
	int ubaseLen = ubase->getLength();
	int vbaseLen = vbase->getLength();

	GA_KnotVector udata = ubase->getKnotVector();
	GA_KnotVector vdata = vbase->getKnotVector();

	// houdini uses alwasy two knots more than maya, so I simply duplicate the first and last one
	int kCount = 0;
	for( int i=0; i<ubaseLen; i++)
	{
		udata.setValue(i, knotsU[kCount]);

		if( (i > 0) && (i < (numKnotsU - 1)))
			kCount++;
	}
	kCount = 0;
	for( int i=0; i<vbaseLen; i++)
	{
		vdata.setValue(i, knotsV[kCount]);
		if( (i > 0) && (i < (numKnotsV - 1)))
			kCount++;
	}

	// in maya the uv coords (here s,t) will be always in a range of 0-1
	// for that reason I simply adapt the uv data below.
	float maxu = 0, maxv = 0;
	for( int i=0; i<ubaseLen; i++)
	{
		if( udata.getValue(i) > maxu)
			maxu = udata.getValue(i);
	}	
	for( int i=0; i<vbaseLen; i++)
	{
		if(vdata.getValue(i) > maxv)
			maxv = vdata.getValue(i);
	}	
	for( int i=0; i<ubaseLen; i++)
	{
		udata.setValue(i,udata.getValue(i)/maxu);
	}	
	for( int i=0; i<vbaseLen; i++)
	{
		vdata.setValue(i,vdata.getValue(i)/maxv);
	}	

	free(points);
	free(ubasis);
	free(vbasis);
	free(knotsU);
	free(knotsV);

    //// Create a NURBS surface.
    //surf = GU_PrimNURBSurf::build(gdp, nv, nu, uorder, vorder);

    ///// @see GEO_TPSurf for basis access methods.

    //for (uint u = 0; u < numU; u++)
    //{
    //    fpreal32 s = SYSfit((fpreal)u, u, (fpreal)numU-1, 0, 1);
    //    for (uint v = 0; v < numV; v++)
    //    {
    //        fpreal32 t = SYSfit((fpreal)v, v, (fpreal)numV-1, 0, 1);

    //        GEO_Vertex  &vtx = (*surf)(v, u);   // Rows/columns
    //        UT_Vector4  P( s, 0, t, 1);
    //        vtx.getPt()->setPos(P);
    //    }
    //}

	//GU_PrimNURBSurf *nurb = GU_PrimNURBSurf::build(gdp, num_rows, num_columns,
   //     u_order, v_order, do_wrap_u, do_wrap_v,
   //     do_interp_u, do_interp_v, GEO_PATCH_QUADS, do_append_points);
   //     
   // // The knot vectors can be accessed as follows:
   // GB_Basis* curr_basis = nurb->getUBasis();
   // int knot_vector_len = curr_basis->getLength();
   // float* knot_vector = curr_basis->getData();

   // // For the points, you can call the standard method on a detail. Please note
   // // that the points may not necessarily belong to the NURB you have just
   // // created if the gdp contains other primitives.
   // UT_Vector4 temp_vec;
   // nurb->getParent()->points()[point_index]->setPos(temp_vec);

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
void createCurveGeo(Hsff& inFile)
{
	GU_Detail	gdp;
	GU_PrimNURBCurve *surf;
	// read count of vertices in U and V direction
	uint numCv = inFile.readUInt();
	uint numU = inFile.readUInt();
	uint degU = inFile.readUInt();
	uint numKnotsU = inFile.readUInt();
	
	double *knotsU = (double *)malloc(sizeof(double) * numKnotsU);
	inFile.readDouble(knotsU, numKnotsU);
	//cout << "Nurbs with " << numU << " cvs\n";
	
	double *points = (double *)malloc(sizeof(double) * 3 *numCv);
	inFile.readDouble(points, numCv * 3);

	uint formU = inFile.readUInt();
	
	uint formUU = formU == 1 ? 0 : 1;

	float *ubasis = (float *)malloc(numKnotsU * sizeof(float));

	// im Linearen fall
	int knotStartU = degU - 1;
	int knotEndU = numKnotsU - degU + 1;

	int numKU = knotEndU - knotStartU;
	int realNumU = numU;

	if( formU )
	{
		knotStartU = 0;
		knotEndU = numKnotsU - degU;
		numKU = numKnotsU - degU;
		realNumU -= degU;
	}
	//cout << "NumU " << numU << " realU " << realNumU << "\n";


	int orderU = degU + 1;
	int numkuo = numU - orderU + 2; //open
	int numkuc = numU - orderU + 3; //close
	int numkup = numU + orderU - 2; //peri

	int end = 1;
	int noend = 2;
	int endCondU = 1;

	bool openU = false;
	if( formU )
	{		
		endCondU = 0;
	}else
		openU = true;
	surf = GU_PrimNURBCurve::build(&gdp, realNumU, degU + 1, openU, endCondU );
	surf->setSize(realNumU);

	if( !openU )
		surf->close();
	else
		surf->open();

	for( int uId = 0; uId < realNumU; uId++)
	{
		int index = uId * 3;
		UT_Vector4 vec;			
		vec.assign(points[index],points[index+1],points[index+2]);
		surf->getVertex(uId).getPt()->setPos(vec);
	}
	GA_Basis *ubase = surf->getBasis();
	int ubaseLen = ubase->getLength();
	GA_KnotVector udata = ubase->getKnotVector();

	// houdini uses alwasy two knots more than maya, so I simply duplicate the first and last one
	int kCount = 0;
	for( int i=0; i<ubaseLen; i++)
	{
		udata.setValue(i, knotsU[kCount]);
		if( (i > 0) && (i < (numKnotsU - 1)))
			kCount++;
	}

	free(points);
	free(ubasis);
	free(knotsU);

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
	//int                 u, v;
    //int                 nu, nv;
    //int                 uorder, vorder;
    //fpreal              s, t;
    //double              now;
    //GU_PrimNURBSurf     *surf;

    //now = context.getTime();

    //nu = SYSmax(COLS(now), 2);          // Number of columns
    //nv = SYSmax(ROWS(now), 2);          // Number of rows
    //uorder = SYSmin(UORDER(now), nu); 
    //vorder = SYSmin(VORDER(now), nv); 

    //gdp->clearAndDestroy();

    //// Create a NURBS surface.
    //surf = GU_PrimNURBSurf::build(gdp, nv, nu, uorder, vorder);

    ///// @see GEO_TPSurf for basis access methods.

    //for (u = 0; u < nu; u++)
    //{
    //    s = SYSfit((fpreal)u, u, (fpreal)nu-1, 0, 1);
    //    for (v = 0; v < nv; v++)
    //    {
    //        t = SYSfit((fpreal)v, v, (fpreal)nv-1, 0, 1);

    //        GEO_Vertex  &vtx = (*surf)(v, u);   // Rows/columns
    //        UT_Vector4  P( s, 0, t, 1);
    //        vtx.getPt()->setPos(P);
    //    }
    //}
//For example, to append a NURBS primitive you can use GU_PrimNURBSurf::build() 
//function which builds the primitive for you, adhering to the specified parameters, 
//and returns a pointer to the new NURB surface. You still need to fill in the points and the knot vector, however:
//
//GU_PrimNURBSurf *nurb = GU_PrimNURBSurf::build(gdp, num_rows, num_columns, u_order, v_order, do_wrap_u, do_wrap_v, do_interp_u, do_interp_v, GEO_PATCH_QUADS, do_append_points);
//        
//    // The knot vectors can be accessed as follows:
//    GB_Basis* curr_basis = nurb->getUBasis();
//    int knot_vector_len = curr_basis->getLength();
//    float* knot_vector = curr_basis->getData();
//
//    // For the points, you can call the standard method on a detail. Please note
//    // that the points may not necessarily belong to the NURB you have just
//    // created if the gdp contains other primitives.
//    UT_Vector4 temp_vec;
//    nurb->getParent()->points()[point_index]->setPos(temp_vec);
