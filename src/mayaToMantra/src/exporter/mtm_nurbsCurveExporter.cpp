#include <maya/MGlobal.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MBoundingBox.h>

#include <memory.h>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "exporter/hsff.h"

#include "mtm_nurbsCurveExporter.h"

static Logging logger;

NurbsCurveExporter::NurbsCurveExporter(MObject &object, MString &fileName)
{
	this->mObject = object;
	this->fileName = fileName;
}

NurbsCurveExporter::~NurbsCurveExporter()
{}

MBoundingBox NurbsCurveExporter::exportIt()
{
	MBoundingBox bbox;

	std::string fname(this->fileName.asChar());
	fname = pystring::replace(fname, ".geo", ".hsff"); // hsff == haggis super file format
	fname = pystring::replace(fname, ".bgeo", ".hsff"); // hsff == haggis super file format
	
	logger.debug(MString("Exporting nurbsCurve to.") + fname.c_str());
	
	Hsff outFile(fname);
	
	if( !outFile.good() )
	{
		MGlobal::displayError(MString("Unable to open ") + MString(fname.c_str()) + " for export.");
		return bbox;
	}
	outFile.write(HSFF); // file identifier
	outFile.write(CURVE); // mesh identifier

	MFnNurbsCurve nurbsFn(this->mObject);
	MPointArray cvs;
	nurbsFn.getCVs(cvs);
	int numU = nurbsFn.numCVs();
	outFile.write(cvs.length());
	outFile.write(numU);
	int numKnotsU = nurbsFn.numKnots();
	MFnNurbsCurve::Form formU = nurbsFn.form();
	MDoubleArray knotsU;
	nurbsFn.getKnots(knotsU);
	int degU = nurbsFn.degree();
	outFile.write(degU);
	outFile.write(numKnotsU);
	outFile.write(knotsU);
	outFile.write(cvs);
	for( uint i = 0; i < cvs.length(); i++)
		bbox.expand(cvs[i]);
	if( formU == MFnNurbsCurve::kClosed || formU == MFnNurbsCurve::kPeriodic)
		outFile.write((uint)1);
	else
		outFile.write((uint)0);
	outFile.close();

//	MFnNurbsCurve nurbsCurveFn(this->mObject);
//	MPointArray cvs;
//	nurbsCurveFn.getCVs(cvs);
//	int numCV = nurbsCurveFn.numCVs();
//	int numKnots = nurbsCurveFn.numKnots();
//	MFnNurbsCurve::Form form = nurbsCurveFn.form();
//	MDoubleArray knots;
//	nurbsCurveFn.getKnots(knots);
//	int deg = nurbsCurveFn.degree();
//
//	for( int i = 0; i < cvs.length(); i++)
//		bbox.expand(cvs[i]);
//	float *basis = (float *)malloc(numKnots * sizeof(float));
//
//	// im Linearen fall
//	int knotStart = deg - 1;
//	int knotEnd = numKnots - deg + 1;
//	int numK = knotEnd - knotStart;
//	int realNum = numCV;
//
//	if( form == MFnNurbsCurve::kClosed || form == MFnNurbsCurve::kPeriodic)
//	{
//		knotStart = 0;
//		knotEnd = numKnots - deg;
//		numK = numKnots - deg;
//		realNum -= deg;
//	}
//
//	float *fstart = basis;
//	for( int i = knotStart; i < knotEnd; i++)
//		*fstart++ = (float)knots[i];
//
//
//    GPD_Detail	 	*gdp;
//    GPD_PrimNURBCurve	*curv;
//
//    gdp = new GPD_Detail;
//	curv = (GPD_PrimNURBCurve *)gdp->buildPrimitive(GPDPRIMNURBCURVE);
//
// //	curv->
// //   surf->setSize(realNum);
//	//surf->set
// //   surf->setUBasis(degU + 1, numKU, ubasis);
// //   surf->setVBasis(degV + 1, numKV, vbasis);
//	
//	//MString info;
//	//info = MString("formU ") + formU + " formV " + formV + " knotsU " + numKnotsU + " knotsV " + numKnotsV;
//	//MGlobal::displayInfo(info);
//	//info = MString("numU ") + numU + " numV " + numV + " orderU " + degU + " degV " + degV;
//	//MGlobal::displayInfo(info);
//	
//	int order = deg + 1;
//	int numko = numCV - order + 2; //open
//	int numkc = numCV - order + 3; //close
//	int numkp = numCV + order - 2; //peri
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
//    //surf->setSurfaceType(GPD_PATCH_QUADS);
//	int end = 1;
//	int noend = 2;
//	int endCond = 1;
//
//	if( form == MFnNurbsCurve::kClosed)
//	{		
//		curv->close();
//		endCond = 0;
//	}
//	if( form == MFnNurbsCurve::kPeriodic)
//	{		
//		curv->close();
//		endCond = 0;
//	}
//	if( form == MFnNurbsCurve::kOpen)
//		curv->open();
//
//	curv->setEndCondition(endCond);
//
//	float *pts = (float *)malloc(sizeof(float) * 3 * realNum);
//	float *st = pts;
//	for( int uId = 0; uId < realNum; uId++)
//	{
//		MPoint p = cvs[uId];
//		*st++ = p.x;
//		*st++ = p.y;
//		*st++ = p.z;
//	}
//	
//	gdp->createPoints(pts, realNum);
//
//	for( int uId = 0; uId < realNum; uId++)
//	{
//		GPD_Vertex *vert = new GPD_Vertex(gdp->point(uId));
//		//curv->addPoint(pt);
//		curv->addVertex(vert);
//	}
//
//
//	for( int uId = 0; uId < realNum; uId++)
//	{
//		MPoint p = cvs[uId];
//		for( int coord = 0; coord < 4; coord++)
//		{	
//			//curv->getVertex(uId)->getPt()->pos[coord] = p[coord];
//		}
//	}
//    //setSphereCVs(surf);
//	MGlobal::displayInfo("Exporting nurbsCurve to " + this->fileName);
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