#ifndef mtth_checker
#define mtth_checker

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Checker :public MPxNode
{
public:
Checker();
virtual         ~Checker();

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
