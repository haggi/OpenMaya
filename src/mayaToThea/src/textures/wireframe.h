#ifndef mtth_wireframe
#define mtth_wireframe

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Wireframe :public MPxNode
{
public:
Wireframe();
virtual         ~Wireframe();

virtual MStatus compute( const MPlug&, MDataBlock& );
virtual void    postConstructor();

static void *  creator();
static MStatus initialize();

// Id tag for use with binary file format
static MTypeId id;

private:

static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject innerColor;
	static    MObject wireColor;
	static    MObject thickness;
	static    MObject fadeout;
	static    MObject angle;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
