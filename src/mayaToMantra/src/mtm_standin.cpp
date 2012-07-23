#include <maya/MFnStringData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MDoubleArray.h>
#include <maya/MSelectionList.h>
#include "mtm_standin.h"
#include "exporter/hsff.h"
#include "utilities/pystring.h"
#include "utilities/logging.h"

static Logging logger;

MTypeId mtmStandIn::id( 0x106EF2 );

MObject  mtmStandIn::bboxMin;
MObject  mtmStandIn::bboxMax;
MObject  mtmStandIn::geoFile;
MObject  mtmStandIn::showPoints;
MObject  mtmStandIn::pointPercent;
MObject  mtmStandIn::pointSize;

MObject  mtmStandIn::dummyOutput;

MStatus mtmStandIn::initialize()
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

	geoFile = tAttr.create( "geoFile", "geoFile", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);

    showPoints = nAttr.create( "showPoints", "showPoints", MFnNumericData::kBoolean);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(true) );

	pointPercent = nAttr.create( "pointPercent", "pointPercent", MFnNumericData::kFloat);
    CHECK_MSTATUS ( nAttr.setKeyable(true) );
    CHECK_MSTATUS ( nAttr.setStorable(true) );
    CHECK_MSTATUS ( nAttr.setHidden(false) );
    CHECK_MSTATUS ( nAttr.setDefault(0.1f) );

	pointSize = nAttr.create( "pointSize", "pointSize", MFnNumericData::kInt, 1);
	nAttr.setMin(1);
	nAttr.setSoftMax(10);

	dummyOutput = nAttr.create( "dummyOutput", "dummyOutput", MFnNumericData::kFloat);

    CHECK_MSTATUS ( addAttribute(dummyOutput) );
    CHECK_MSTATUS ( addAttribute(bboxMin) );
    CHECK_MSTATUS ( addAttribute(bboxMax) );
    CHECK_MSTATUS ( addAttribute(geoFile) );
    CHECK_MSTATUS ( addAttribute(showPoints) );
    CHECK_MSTATUS ( addAttribute(pointPercent) );
    CHECK_MSTATUS ( addAttribute(pointSize) );
    CHECK_MSTATUS ( attributeAffects (bboxMin, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (bboxMax, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (geoFile, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (showPoints, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (pointPercent, dummyOutput) );
    CHECK_MSTATUS ( attributeAffects (pointSize, dummyOutput) );

    return MS::kSuccess;
}


void mtmStandIn::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 

	MStatus stat;
	MDataBlock datablock = forceCache();
	MDataHandle handle = datablock.inputValue( dummyOutput, &stat );

	//MGlobal::displayInfo(MString("ptsize ") + pointsize + " density " + pointpercent);

	view.beginGL(); 

	glPushAttrib( GL_CURRENT_BIT );
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

	glPopAttrib();
		
	glPointSize((float)pointsize);
	glBegin( GL_POINTS );
	for( uint i = 0; i < pointData.length(); i++)
		glVertex3f(pointData[i].x, pointData[i].y, pointData[i].z);
	glEnd();
	

	view.endGL();
}

bool mtmStandIn::isBounded() const
{ 
	return true;
}

MBoundingBox mtmStandIn::boundingBox() const
{   
	// Get the size
	//
	MObject thisNode = thisMObject();
	MPlug plug( thisNode, dummyOutput );
	//MDistance sizeVal;
	//plug.getValue( sizeVal );

	//double multiplier = sizeVal.asCentimeters();
 
	MPoint corner1( -0.5, -0.5, -0.5 );
	MPoint corner2( 0.5, 0.5, 0.5 );

	return MBoundingBox( bboxmin, bboxmax );
}


void mtmStandIn::postConstructor( )
{
	setMPSafe(true);
	MFnDependencyNode nodefn(thisMObject());
	nodefn.setName("mtm_standinShape#");	
}


mtmStandIn::mtmStandIn()
{
}


mtmStandIn::~mtmStandIn()
{
}

void * mtmStandIn::creator()
{
    return new mtmStandIn();
}


bool mtmStandIn::readPtcFile()
{
	MStatus stat;
	pointData.clear(); // delete all old points
	
	std::string hsffpFile(geofile.asChar());
	if( pystring::endswith(hsffpFile, ".geo"))
	{
		hsffpFile = pystring::replace(hsffpFile, ".geo", ".hsffp");
	}
	if( pystring::endswith(hsffpFile, ".bgeo"))
	{
		hsffpFile = pystring::replace(hsffpFile, ".bgeo", ".hsffp");
	}

	// call python converter
	MString cmd = MString("import mtm_initialize as minit; reload(minit);");
	cmd += "minit.startHoudiniObjectTools(geoFile = \"" + geofile + "\"";
	
	if( showpoints )
		cmd += MString(", ptc = True, density = ") + this->pointpercent;
	cmd += ")";

	logger.info(cmd);
	MDoubleArray bboxArray;
	stat = MGlobal::executePythonCommand(cmd, bboxArray);
	if(!stat)
	{
		logger.error(MString("Problem execting python cmd"));
		return false;
	}
	if( bboxArray.length() != 6 )
	{
		logger.error(MString("Problem reading bbox"));
		return false;
	}
	bboxmin.x = bboxArray[0];
	bboxmin.y = bboxArray[1];
	bboxmin.z = bboxArray[2];
	bboxmax.x = bboxArray[3];
	bboxmax.y = bboxArray[4];
	bboxmax.z = bboxArray[5];

	if( showpoints )
	{
		Hsff ptcFile(hsffpFile.c_str(), true);
		if(!ptcFile.good())
		{
			MGlobal::displayError(MString("Could not read ptcd file: ") + hsffpFile.c_str());
			return false;
		}

		int type;
		ptcFile.read(type); // read type, should be 'PTC'
		//MGlobal::displayInfo(MString("Type PTC (") + PTC + ") : " + type);
		if( type != PTC)
		{
			MGlobal::displayError(MString("Type is not PTC (") + PTC + ") : " + type);
			return false;
		}
		
		MBoundingBox bbox;
		ptcFile.read(bbox);
		bboxmin = bbox.min();
		bboxmax = bbox.max();
		uint numpoints = 0;
		if( showpoints )
		{
			ptcFile.read(numpoints);
			MGlobal::displayInfo(MString("Reading ") + numpoints + " points");

			ptcFile.read(pointData, numpoints);
		}
		ptcFile.close();
	}
	return true;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mtmStandIn::compute(
const MPlug&      plug,
      MDataBlock& block ) 
{ 
	if( plug == dummyOutput )
	{
		MDataHandle outhandle = block.outputValue(dummyOutput);

		bool showp = block.inputValue( showPoints ).asBool();
		float ppc = block.inputValue( pointPercent ).asFloat();
		pointsize = block.inputValue( pointSize ).asInt();
		MFloatVector& bbmi  = block.inputValue( bboxMin ).asFloatVector();
		MFloatVector& bbma  = block.inputValue( bboxMax ).asFloatVector();
		bboxmin.x = bbmi.x;
		bboxmin.y = bbmi.y;
		bboxmin.z = bbmi.z;
		bboxmax.x = bbma.x;
		bboxmax.y = bbma.y;
		bboxmax.z = bbma.z;
		//bboxmax = block.inputValue( bboxMax ).asFloatVector();
		MString gfile = block.inputValue( geoFile ).asString();
		if( (geofile != gfile) || (showp != showpoints) || (ppc != pointpercent))
		{
			geofile = gfile;
			showpoints = showp;
			pointpercent = ppc;
			if( geofile.length() > 0 )
			{
				readPtcFile();
				bbmi.x = (float)bboxmin.x;
				bbmi.y = (float)bboxmin.y;
				bbmi.z = (float)bboxmin.z;
				bbma.x = (float)bboxmax.x;
				bbma.y = (float)bboxmax.y;
				bbma.z = (float)bboxmax.z;
			}
		}
		block.inputValue( bboxMin ).setClean();
		outhandle.setClean();
	}

	block.setClean(plug);

    return MS::kSuccess;
}
