#ifndef MT_STANDIN_H
#define MT_STANDIN_H

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
#include <math.h>

class StandinLocatorNode : public MPxLocatorNode
{
	public:
                      StandinLocatorNode();
    virtual           ~StandinLocatorNode();

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 

    static MStatus    initialize();

	private:
	// Inputs
    static MObject  bboxMin;	
    static MObject  bboxMax;	
    static MObject  proxyFile;	
    static MObject  displayType;	
    static MObject  percentDisplay;	
    static MObject  elementSize;	

	MPoint		bboxmin, bboxmax;
	MString		proxy_file;
	float		percent_display;
	int			element_size;

	// Outputs
	static MObject  dummyOutput;

	MFloatVectorArray points;
};

#endif