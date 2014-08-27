#ifndef mtth_windy
#define mtth_windy

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Windy :public MPxNode
{
public:
Windy();
virtual         ~Windy();

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
	static    MObject strengthOctaves;
	static    MObject strengthOmega;
	static    MObject heightOctaves;
	static    MObject heightOmega;
	static    MObject symmetric;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
