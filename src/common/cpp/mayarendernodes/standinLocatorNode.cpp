#include "standinLocatorNode.h"

//#include <maya/MFnStringData.h>
//#include <maya/MFnDependencyNode.h>
//#include <maya/MGlobal.h>
//#include <maya/MDoubleArray.h>
//#include <maya/MSelectionList.h>
//#include "utilities/pystring.h"

MObject  StandinLocatorNode::bboxMin;
MObject  StandinLocatorNode::bboxMax;
MObject  StandinLocatorNode::proxyFile;	
MObject  StandinLocatorNode::displayType;	
MObject  StandinLocatorNode::percentDisplay;	
MObject  StandinLocatorNode::elementSize;	

MObject  StandinLocatorNode::dummyOutput;

MStatus StandinLocatorNode::initialize()
{
    MFnTypedAttribute tAttr; 
    MFnNumericAttribute nAttr; 
    MFnLightDataAttribute lAttr;
	MFnEnumAttribute eAttr;
	MStatus stat;

    bboxMin = nAttr.createPoint( "bboxMin", "bboxMin" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setDefault(-0.5f, -0.5f, -0.5f) );

    bboxMax = nAttr.createPoint( "bboxMax", "bboxMax" );
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
	CHECK_MSTATUS ( nAttr.setDefault( 0.5f,  0.5f,  0.5f) );

	proxyFile = tAttr.create( "proxyFile", "proxyFile", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( proxyFile ));

	displayType = eAttr.create("displayType", "displayType", 0, &stat);
	stat = eAttr.addField( "BBox", 0 );
	stat = eAttr.addField( "Points", 1 );
	stat = eAttr.addField( "Polygons", 2 );
	CHECK_MSTATUS(addAttribute( displayType ));

	percentDisplay = nAttr.create( "percentDisplay", "percentDisplay", MFnNumericData::kFloat, 0.1f);
	CHECK_MSTATUS(addAttribute( percentDisplay ));

	elementSize = nAttr.create( "elementSize", "elementSize", MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( elementSize ));

	dummyOutput = nAttr.create( "dummyOutput", "dummyOutput", MFnNumericData::kFloat);
    CHECK_MSTATUS ( addAttribute(dummyOutput) );
    
	CHECK_MSTATUS ( attributeAffects (bboxMin, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (bboxMax, dummyOutput) );

    CHECK_MSTATUS ( attributeAffects (proxyFile, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (displayType, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (percentDisplay, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (elementSize, dummyOutput) );

    return MS::kSuccess;
}


void StandinLocatorNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 

	MStatus stat;
	MDataBlock datablock = forceCache();
	MDataHandle handle = datablock.inputValue( dummyOutput, &stat );

	view.beginGL(); 
	{
		glPushAttrib( GL_CURRENT_BIT );
		{
			if ( status == M3dView::kActive ) {
				view.setDrawColor( 13, M3dView::kActiveColors );
			} else {
				view.setDrawColor( 13, M3dView::kDormantColors );
			} 

			//// Draw the outline of the box
			// faceDown
			glBegin( GL_LINE_STRIP );
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmin.z);
			glEnd();
			// faceRoof
			glBegin( GL_LINE_STRIP );
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmin.z);
			glEnd();
			// 4 columns
			glBegin( GL_LINES );
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmin.z);
			glEnd();

			glBegin( GL_LINES );
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmin.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmax.z);
			glEnd();

			glBegin( GL_LINES );
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmax.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmax.z);
			glEnd();

			glBegin( GL_LINES );
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmin.y, (GLfloat)this->bboxmin.z);
			glVertex3f((GLfloat)this->bboxmax.x, (GLfloat)this->bboxmax.y, (GLfloat)this->bboxmin.z);
			glEnd();
		}
		glPopAttrib();
		
		//glPointSize((float)pointsize);
		//glBegin( GL_POINTS );
		//for( uint i = 0; i < pointData.length(); i++)
		//	glVertex3f(pointData[i].x, pointData[i].y, pointData[i].z);
		glEnd();		
	}

	view.endGL();
}

bool StandinLocatorNode::isBounded() const
{ 
	return true;
}

MBoundingBox StandinLocatorNode::boundingBox() const
{   
	MObject thisNode = thisMObject();
	MPlug plug( thisNode, dummyOutput );

	MPoint corner1( -0.5, -0.5, -0.5 );
	MPoint corner2( 0.5, 0.5, 0.5 );

	return MBoundingBox( corner1, corner2 );
}


StandinLocatorNode::StandinLocatorNode()
{
}


StandinLocatorNode::~StandinLocatorNode()
{
}

