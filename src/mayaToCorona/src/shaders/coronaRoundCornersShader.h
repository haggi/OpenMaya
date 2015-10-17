#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaAO Shader Class //

class CoronaRoundCornersShader : public MPxNode
{
public:
	CoronaRoundCornersShader();
	virtual         ~CoronaRoundCornersShader();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();
    virtual void    postConstructor();
    static  MTypeId   id;

protected:

	static MObject samples;
	static MObject radius;
	static MObject outNormal;
	static MObject normalCamera;
};
