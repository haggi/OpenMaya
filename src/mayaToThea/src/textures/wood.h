#ifndef mtth_wood
#define mtth_wood

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Wood :public MPxNode
{
public:
Wood();
virtual         ~Wood();

virtual MStatus compute( const MPlug&, MDataBlock& );
virtual void    postConstructor();

static void *  creator();
static MStatus initialize();

// Id tag for use with binary file format
static MTypeId id;

private:

static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject high;
	static    MObject low;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
