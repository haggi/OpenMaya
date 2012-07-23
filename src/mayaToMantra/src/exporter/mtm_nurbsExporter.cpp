#include <maya/MGlobal.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MBoundingBox.h>

#include <memory.h>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "exporter/hsff.h"

#include "mtm_nurbsExporter.h"

static Logging logger;

NurbsExporter::NurbsExporter(MObject &object, MString &fileName)
{
	this->mObject = object;
	this->fileName = fileName;
}

NurbsExporter::~NurbsExporter()
{}

MBoundingBox NurbsExporter::exportIt()
{
	MBoundingBox bbox;
	std::string fname(this->fileName.asChar());
	fname = pystring::replace(fname, ".geo", ".hsff"); // hsff == haggis super file format
	fname = pystring::replace(fname, ".bgeo", ".hsff"); // hsff == haggis super file format
	
	logger.debug(MString("Exporting nurbssurface to.") + fname.c_str());
	
	Hsff outFile(fname);
	
	if( !outFile.good() )
	{
		MGlobal::displayError(MString("Unable to open ") + MString(fname.c_str()) + " for export.");
		return bbox;
	}
	outFile.write(HSFF); // file identifier
	outFile.write(NURBS); // mesh identifier

	MFnNurbsSurface nurbsFn(this->mObject);
	MPointArray cvs;
	nurbsFn.getCVs(cvs);
	int numU = nurbsFn.numCVsInU();
	int numV = nurbsFn.numCVsInV();
	outFile.write(cvs.length());
	outFile.write(numU);
	outFile.write(numV);
	int numKnotsU = nurbsFn.numKnotsInU();
	int numKnotsV = nurbsFn.numKnotsInV();
	MFnNurbsSurface::Form formU = nurbsFn.formInU();
	MFnNurbsSurface::Form formV = nurbsFn.formInV();
	MDoubleArray knotsU;
	MDoubleArray knotsV;
	nurbsFn.getKnotsInU(knotsU);
	nurbsFn.getKnotsInV(knotsV);
	int degU = nurbsFn.degreeU();
	int degV = nurbsFn.degreeV();
	outFile.write(degU);
	outFile.write(degV);
	outFile.write(numKnotsU);
	outFile.write(numKnotsV);
	outFile.write(knotsU);
	outFile.write(knotsV);
	outFile.write(cvs);
	for( uint i = 0; i < cvs.length(); i++)
		bbox.expand(cvs[i]);
	if( formU == MFnNurbsSurface::kClosed || formU == MFnNurbsSurface::kPeriodic)
		outFile.write((uint)1);
	else
		outFile.write((uint)0);
	if( formV == MFnNurbsSurface::kClosed || formV == MFnNurbsSurface::kPeriodic)
		outFile.write((uint)1);
	else
		outFile.write((uint)0);
	outFile.close();


//	MFnNurbsSurface nurbsFn(this->mObject);
//	MPointArray cvs;
//	nurbsFn.getCVs(cvs);
//	int numU = nurbsFn.numCVsInU();
//	int numV = nurbsFn.numCVsInV();
//    int numKnotsU = nurbsFn.numKnotsInU();
//    int numKnotsV = nurbsFn.numKnotsInV();
//	MFnNurbsSurface::Form formU = nurbsFn.formInU();
//	MFnNurbsSurface::Form formV = nurbsFn.formInV();
//	MDoubleArray knotsU;
//	MDoubleArray knotsV;
//	nurbsFn.getKnotsInU(knotsU);
//	nurbsFn.getKnotsInV(knotsV);
//	int degU = nurbsFn.degreeU();
//	int degV = nurbsFn.degreeV();
//
//	for( int i = 0; i < cvs.length(); i++)
//		bbox.expand(cvs[i]);
//	float *ubasis = (float *)malloc(numKnotsU * sizeof(float));
//	float *vbasis = (float *)malloc(numKnotsV * sizeof(float));
//
//	// im Linearen fall
//	int knotStartU = degU - 1;
//	int knotEndU = numKnotsU - degU + 1;
//	int knotStartV = degV - 1;
//	int knotEndV = numKnotsV - degV + 1;
//
//	int numKU = knotEndU - knotStartU;
//	int numKV = knotEndV - knotStartV;
//	int realNumU = numU;
//	int realNumV = numV;
//
//	if( formU == MFnNurbsSurface::Form::kClosed || formU == MFnNurbsSurface::Form::kPeriodic)
//	{
//		knotStartU = 0;
//		knotEndU = numKnotsU - degU;
//		numKU = numKnotsU - degU;
//		realNumU -= degU;
//	}
//	if( formV == MFnNurbsSurface::Form::kClosed || formV == MFnNurbsSurface::Form::kPeriodic)
//	{
//		knotStartV = 0;
//		knotEndV = numKnotsV - degV;
//		numKV = numKnotsV - degV;
//		realNumV -= degV;
//	}
//
//	float *fstart = ubasis;
//	for( int i = knotStartU; i < knotEndU; i++)
//		*fstart++ = (float)knotsU[i];
//
//	fstart = vbasis;
//	for( int i = knotStartV; i < knotEndV; i++)
//		*fstart++ = (float)knotsV[i];
//
//    GPD_Detail	 	*gdp;
//    GPD_PrimNURBSurf	*surf;
//
//    gdp = new GPD_Detail;
//    surf = (GPD_PrimNURBSurf *)gdp->buildPrimitive(GPDPRIMNURBSURF);
//	
//	
//    surf->setSize(realNumV, realNumU);
//    surf->setUBasis(degU + 1, numKU, ubasis);
//    surf->setVBasis(degV + 1, numKV, vbasis);
//	
//	//MString info;
//	//info = MString("formU ") + formU + " formV " + formV + " knotsU " + numKnotsU + " knotsV " + numKnotsV;
//	//MGlobal::displayInfo(info);
//	//info = MString("numU ") + numU + " numV " + numV + " orderU " + degU + " degV " + degV;
//	//MGlobal::displayInfo(info);
//	
//	int orderU = degU + 1;
//	int orderV = degV + 1;
//	int numkuo = numU - orderU + 2; //open
//	int numkvo = numV - orderV + 2;
//	int numkuc = numU - orderU + 3; //close
//	int numkvc = numV - orderV + 3;
//	int numkup = numU + orderU - 2; //peri
//	int numkvp = numV + orderV - 2;
//
//	//info = MString("numkuo ") + numkuo + " numkuc " + numkuc + " numkvo " + numkvo + " numkvc " + numkvc + " numkup " + numkup + " numkvp " + numkvp;
//	//MGlobal::displayInfo(info);
//
////#open curve in houdini #Knots = #Vtx - Order + 2.
////#closed curve in houdini #Knots = #Vtx - Order + 3.
////#noend #K = #Vtx + Order - 2
//// In maya gibts bei offenen Nurbs an den Enden jeweils mehrere gleiche knots da sind das z.B.
//// bei 7 Vertices sieht das dann so aus:										0 0 0 1 2 3 4 4 4 
//// die werden offenbar in Houdini nicht benötigt. das müssen das nur die hier sein	0 1 2 3 4 
//// bzw bei linear 0-1 ist okay quadratisch 0 0 1 1 cubisch 0 0 0 1 1 1 linear->0 quadr->1 cub->2 ==> deg - 1
//
//// unschöner Weise überlappen sich die punkte/cvs in maya bei geschlossenen surfaces, das muss ich dann wohl oder
//// übel selbst ausgleichen, Also schreibe ich dann in der geschlossenen Richtung nur x - degree cvs raus
//
//    //surf->setSurfaceType(GPD_PATCH_ROWS);
//    surf->setSurfaceType(GPD_PATCH_QUADS);
//	int end = 1;
//	int noend = 2;
//	int endCondU = 1;
//	int endCondV = 1;
//
//	if( formU == MFnNurbsSurface::kClosed)
//	{		
//		surf->wrapU();
//		endCondU = 0;
//	}
//	if( formU == MFnNurbsSurface::kPeriodic)
//	{		
//		surf->wrapU();
//		endCondU = 0;
//	}
//	if( formU == MFnNurbsSurface::kOpen)
//		surf->openU();
//	if( formV == MFnNurbsSurface::kClosed)
//	{		
//		surf->wrapV();
//		endCondV = 0;
//	}
//	if( formV == MFnNurbsSurface::kOpen)
//		surf->openV();
//	if( formV == MFnNurbsSurface::kPeriodic)
//	{		
//		surf->wrapV();
//		endCondV = 0;
//	}
//
//    surf->setEndInterp(endCondU, endCondV);
//
//	for( int uId = 0; uId < realNumU; uId++)
//	{
//		for( int vId = 0; vId < realNumV; vId++)
//		{
//			int index = numV * uId + vId;
//			MPoint p = cvs[index];
//			//info = MString("cv ") + index + " : " + p.x + " " + p.y + " " + p.z;
//			//MGlobal::displayInfo(info);
//			for( int coord = 0; coord < 4; coord++)
//				surf->getVertex(vId, uId)->getPt()->pos[coord] = p[coord];
//		}
//	}
//    //setSphereCVs(surf);
//	MGlobal::displayInfo("Exporting nurbsSurface to " + this->fileName);
//	gdp->save(this->fileName.asChar());

	return bbox;
}

/*
#include "GPD_Detail.h"
#include "GPD_PrimNURBSurf.h"
#include "GPD_PrimType.h"
#include "GPD_Vertex.h"
#include "GPD_Point.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

void
setSphereCVs(GPD_PrimNURBSurf *surf)
{
    static float	cornerw = cos(float(3.1415*0.25));
    float		w;
    int			r, i;
    int			degree = 2;
    int			mr = 2;
    int			rows = 5;

    // set cvs for the first span manually (using order 3)
    // building it clockwise, as for a left-handed system:
    // adjust rationality at this point

    w = cornerw;
    surf->getVertex(mr,0)->getPt()->pos[0] = 1.0f;
    surf->getVertex(mr,0)->getPt()->pos[1] = 0.0f;
    surf->getVertex(mr,0)->getPt()->pos[2] = 0.0f;
    surf->getVertex(mr,0)->getPt()->pos[3] = 1.0f;

    surf->getVertex(mr,1)->getPt()->pos[0] = w;
    surf->getVertex(mr,1)->getPt()->pos[1] = w;
    surf->getVertex(mr,1)->getPt()->pos[2] = 0.0f;
    surf->getVertex(mr,1)->getPt()->pos[3] = w;

    surf->getVertex(mr,2)->getPt()->pos[0] = 0.0f;
    surf->getVertex(mr,2)->getPt()->pos[1] = 1.0f;
    surf->getVertex(mr,2)->getPt()->pos[2] = 0.0f;
    surf->getVertex(mr,2)->getPt()->pos[3] = 1.0f;

    // copy this span to the one section of the rows (assuming vorder = 3)
    for (i = 0; i<degree; i++)
    {
	float *p = surf->getVertex(mr, i)->getPt()->pos;
	float *d;

	d = surf->getVertex(mr-1, i)->getPt()->pos;
	
	d[0] = p[0] * w;
	d[1] = p[1] * w;
	d[2] = p[3] * w;
	d[3] = p[3] * w;
	
	d = surf->getVertex(mr-2, i)->getPt()->pos;

	d[0] = 0.0f;
	d[1] = 0.0f;
	d[2] = p[3];
	d[3] = p[3];
    }

    // copy and reflect octant points to other octants
    for (r=mr; r>=0; r--)
    {
	for (i=0; i<degree; i++)
	{
	    float 	*p1 = surf->getVertex(r, i+0*degree)->getPt()->pos;
	    float 	*p2 = surf->getVertex(r, i+1*degree)->getPt()->pos;
	    float 	*p3 = surf->getVertex(r, i+2*degree)->getPt()->pos;
	    float 	*p4 = surf->getVertex(r, i+3*degree)->getPt()->pos;

	    // readjust weight before copying
	    float       ws  = 1.0F / p1[3];

	    p1[0] *= ws;	p1[1] *= ws;	p1[2] *= ws;

	    p2[0] = -p1[1];	p2[1] = p1[0];	p2[2] = p1[2];	p2[3] = p1[3];
	    p3[0] = -p2[1];	p3[1] = p2[0];	p3[2] = p2[2];	p3[3] = p2[3];
	    p4[0] = -p3[1];	p4[1] = p3[0];	p4[2] = p3[2];	p4[3] = p3[3];

	    // reflect it to the other hemisphere as well
	    if (r<mr)
	    {
		int	nr = rows - r - 1;

		float	*p5 = surf->getVertex(nr, i+0*degree)->getPt()->pos;
		float	*p6 = surf->getVertex(nr, i+1*degree)->getPt()->pos;
		float	*p7 = surf->getVertex(nr, i+2*degree)->getPt()->pos;
		float	*p8 = surf->getVertex(nr, i+3*degree)->getPt()->pos;

		p5[0] = p1[0];	p5[1] = p1[1];	p5[2] = -p1[2];	p5[3] = p1[3];
		p6[0] = p2[0];	p6[1] = p2[1];	p6[2] = -p2[2];	p6[3] = p2[3];
		p7[0] = p3[0];	p7[1] = p3[1];	p7[2] = -p3[2];	p7[3] = p3[3];
		p8[0] = p4[0];	p8[1] = p4[1];	p8[2] = -p4[2];	p8[3] = p4[3];
	    }
	}
    }
}

int
main(int argc, char *argv[])
{
    GPD_Detail	 	*gdp;
    GPD_PrimNURBSurf	*surf;
    float		 ubasis[8] = { 0, 0.25, 0.25, 0.5, 0.5, 
				       0.75, 0.75, 1 };
    float		 vbasis[4] = { 0, 0.5, 0.5, 1 };

    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " <outfile>" << endl;
	exit(1);
    }

    gdp = new GPD_Detail;
    surf = (GPD_PrimNURBSurf *)gdp->buildPrimitive(GPDPRIMNURBSURF);
    surf->setSize(5, 8);
    surf->setUBasis(3, 8, ubasis);
    surf->setVBasis(3, 4, vbasis);
    surf->wrapU();
    surf->setSurfaceType(GPD_PATCH_QUADS);
    surf->setEndInterp(1, 1);

    setSphereCVs(surf);

    if (gdp->save(argv[1]) < 0)
    {
	cerr << "Unable to save " << argv[1] << endl;
	exit(1);
    }
    return 0;
}


*/