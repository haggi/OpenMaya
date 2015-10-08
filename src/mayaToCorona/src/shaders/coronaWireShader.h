#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaWire Shader Class //

class CoronaWire : public MPxNode
{
public:
                    CoronaWire();
    virtual         ~CoronaWire();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

	/// If true, then the distance in world space will be used, otherwise, projected distance in pixels is used by edgeWidth and
	/// vertexWidth
	static    MObject useWorldSpace;

	/// Enables showing vertices. Vertices have priority over edges
	static    MObject showVertices;

	/// Enables showing edges
	static    MObject showEdges;

	/// Enables showing all triangle edges. When false, only edges that were set to visible in the triangle input data are used
	static    MObject allEdges;

	/// Width of the edge detection, either in world units, or in projected pixels (see useWorldSpace)
	static    MObject edgeWidth;

	/// Width of the vertex detection, either in world units, or in projected pixels (see useWorldSpace)
	static    MObject vertexWidth;

	static MObject  outColor;

protected:

};
