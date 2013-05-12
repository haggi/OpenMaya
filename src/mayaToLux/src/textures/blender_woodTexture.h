#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class blender_wood :public MPxNode
{
    public:
                    blender_wood();
    virtual         ~blender_wood();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject noisesize;
	static    MObject noisebasis2;
	static    MObject noisebasis;
	static    MObject scale;
	static    MObject coordinates;
	static    MObject bright;
	static    MObject rotate;
	static    MObject noisetype;
	static    MObject turbulence;
	static    MObject translate;
	static    MObject type;
	static    MObject contrast;
	static    MObject luxOutFloat;
//---------------------------- automatically created attributes end ------------------------------------

};
