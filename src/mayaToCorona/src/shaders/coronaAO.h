#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaAO Shader Class //

class CoronaAO : public MPxNode
{
public:
                    CoronaAO();
    virtual         ~CoronaAO();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

protected:

	static MObject maxDist;
	static MObject maxSamplesCount;
	static MObject phongExponent;
	static MObject mixExponent;
	static MObject invert;
	static MObject excludeMode;
	static MObject useOffset;
	static MObject offset;

	static MObject outColor;
};
