#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaRaytype Shader Class //

class CoronaRaytype : public MPxNode
{
public:
                    CoronaRaytype();
    virtual         ~CoronaRaytype();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();
    virtual void    postConstructor();

    static  MTypeId   id;

protected:
	static    MObject base;
	static    MObject reflect;
	static    MObject refract;
	static    MObject direct;
	static    MObject outColor;
};
