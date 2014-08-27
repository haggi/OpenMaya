#ifndef mtth_blackBody
#define mtth_blackBody

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Blackbody :public MPxNode
{
public:
Blackbody();
virtual         ~Blackbody();

virtual MStatus compute( const MPlug&, MDataBlock& );
virtual void    postConstructor();

static void *  creator();
static MStatus initialize();

// Id tag for use with binary file format
static MTypeId id;

private:

static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject temperature;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
