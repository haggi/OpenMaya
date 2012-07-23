#ifndef MTM_STANDIN_H
#define MTM_STANDIN_H

#include <math.h>
#include <maya/MPxLocatorNode.h> 
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MVector.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>

const MString mtmStandInName = "mtm_standin";

class mtmStandIn : public MPxLocatorNode
{
	public:
                      mtmStandIn();
    virtual           ~mtmStandIn();

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 

    virtual MStatus   compute( const MPlug&, MDataBlock& );
	virtual void      postConstructor();

    static void *     creator();
    static MStatus    initialize();
	bool	readPtcFile();

    static MTypeId    id;

	private:
	// Inputs
    static MObject  bboxMin;	
    static MObject  bboxMax;	
    static MObject  geoFile;	
    static MObject  showPoints;	
    static MObject  pointPercent;	
    static MObject  pointSize;	

	MPoint bboxmin, bboxmax;
	MString geofile;
	bool showpoints;
	float pointpercent;
	int pointsize;
	// Outputs
	static MObject  dummyOutput;

	MFloatVectorArray pointData;
};

#endif