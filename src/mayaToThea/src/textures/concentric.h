#ifndef mtth_concentric
#define mtth_concentric

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Concentric :public MPxNode
{
public:
Concentric();
virtual         ~Concentric();

virtual MStatus compute( const MPlug&, MDataBlock& );
virtual void    postConstructor();

static void *  creator();
static MStatus initialize();

// Id tag for use with binary file format
static MTypeId id;

private:

static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
