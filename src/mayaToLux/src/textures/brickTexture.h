#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class brick :public MPxNode
{
    public:
                    brick();
    virtual         ~brick();

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
	static    MObject brickbevel;
	static    MObject brickbond;
	static    MObject mortarsize;
	static    MObject brickwidth;
	static    MObject bricktex;
	static    MObject brickdepth;
	static    MObject coordinates;
	static    MObject brickmodtex;
	static    MObject rotate;
	static    MObject motartex;
	static    MObject brickrun;
	static    MObject translate;
	static    MObject brickheight;
	static    MObject luxOutFloat;
	static    MObject luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------

};
