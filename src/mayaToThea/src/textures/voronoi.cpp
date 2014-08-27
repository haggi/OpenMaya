#include "voronoi.h"
#include <math.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>

MTypeId Voronoi::id( 0x0011EF65 );

MObject Voronoi::outColor;

	
//---------------------------- automatically created static attributes start ------------------------------------
MObject	Voronoi::cellColor;
MObject	Voronoi::ridgeColor;
MObject	Voronoi::plateauColor;
MObject	Voronoi::ridge;
MObject	Voronoi::plateau;
//---------------------------- automatically created static attributes end ------------------------------------
	

#define MAKE_INPUT(attr)                                \
 CHECK_MSTATUS ( attr.setKeyable(true) );               \
 CHECK_MSTATUS ( attr.setStorable(true) );              \
 CHECK_MSTATUS ( attr.setReadable(true) );              \
 CHECK_MSTATUS ( attr.setWritable(true) );

#define MAKE_OUTPUT(attr)                               \
  CHECK_MSTATUS ( attr.setKeyable(false) );             \
  CHECK_MSTATUS ( attr.setStorable(false) );            \
  CHECK_MSTATUS ( attr.setReadable(true) );             \
  CHECK_MSTATUS ( attr.setWritable(false) );

//
// DESCRIPTION:
void Voronoi::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
Voronoi::Voronoi()
{
}

//
// DESCRIPTION:
Voronoi::~Voronoi()
{
}

//
// DESCRIPTION:
void *Voronoi::creator()
{
    return new Voronoi();
}

//
// DESCRIPTION:
MStatus Voronoi::initialize()
{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus status;

	outColor = nAttr.createColor("outColor", "outColor");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute( outColor ));

//---------------------------- automatically created attributes start ------------------------------------
	cellColor = nAttr.createColor("cellColor", "cellColor");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( cellColor ));


	ridgeColor = nAttr.createColor("ridgeColor", "ridgeColor");
	nAttr.setDefault(0.5,0.5,0.5);
	CHECK_MSTATUS(addAttribute( ridgeColor ));


	plateauColor = nAttr.createColor("plateauColor", "plateauColor");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( plateauColor ));


	ridge = nAttr.create("ridge", "ridge",  MFnNumericData::kFloat, 0.06);
	CHECK_MSTATUS(addAttribute( ridge ));


	plateau = nAttr.create("plateau", "plateau",  MFnNumericData::kFloat, 0.03);
	CHECK_MSTATUS(addAttribute( plateau ));


//---------------------------- automatically created attributes end ------------------------------------

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus Voronoi::compute(const MPlug &plug, MDataBlock &block)
{
    return MS::kSuccess;
}
