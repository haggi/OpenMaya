#ifndef mtth_gradient
#define mtth_gradient

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Gradient :public MPxNode
{
public:
Gradient();
virtual         ~Gradient();

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
	static    MObject direction;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
