#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class wrinkled :public MPxNode
{
    public:
                    wrinkled();
    virtual         ~wrinkled();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject scale;
	static    MObject octaves;
	static    MObject coordinates;
	static    MObject roughness;
	static    MObject rotate;
	static    MObject translate;
	static    MObject luxOutFloat;
//---------------------------- automatically created attributes end ------------------------------------

};
