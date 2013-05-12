#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class blender_musgrave :public MPxNode
{
    public:
                    blender_musgrave();
    virtual         ~blender_musgrave();

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
	static    MObject outscale;
	static    MObject scale;
	static    MObject h;
	static    MObject octs;
	static    MObject coordinates;
	static    MObject bright;
	static    MObject rotate;
	static    MObject lacu;
	static    MObject gain;
	static    MObject offset;
	static    MObject noisebasis;
	static    MObject translate;
	static    MObject type;
	static    MObject contrast;
	static    MObject luxOutFloat;
//---------------------------- automatically created attributes end ------------------------------------

};
