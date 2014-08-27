#ifndef mtth_voronoi
#define mtth_voronoi

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class Voronoi :public MPxNode
{
public:
Voronoi();
virtual         ~Voronoi();

virtual MStatus compute( const MPlug&, MDataBlock& );
virtual void    postConstructor();

static void *  creator();
static MStatus initialize();

// Id tag for use with binary file format
static MTypeId id;

private:

static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject cellColor;
	static    MObject ridgeColor;
	static    MObject plateauColor;
	static    MObject ridge;
	static    MObject plateau;
//---------------------------- automatically created attributes end ------------------------------------

};

#endif
