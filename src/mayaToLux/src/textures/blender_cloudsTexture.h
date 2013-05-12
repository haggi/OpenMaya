#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class blender_clouds :public MPxNode
{
    public:
                    blender_clouds();
    virtual         ~blender_clouds();

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
	static    MObject noisebasis;
	static    MObject scale;
	static    MObject noisedepth;
	static    MObject coordinates;
	static    MObject bright;
	static    MObject rotate;
	static    MObject translate;
	static    MObject luxOutFloat;
	static    MObject contrast;
	static    MObject noisetype;
//---------------------------- automatically created attributes end ------------------------------------

};
