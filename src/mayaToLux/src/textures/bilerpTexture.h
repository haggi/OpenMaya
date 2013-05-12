#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class bilerp :public MPxNode
{
    public:
                    bilerp();
    virtual         ~bilerp();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject v11;
	static    MObject v01;
	static    MObject v00;
	static    MObject vdelta;
	static    MObject udelta;
	static    MObject mapping;
	static    MObject v1;
	static    MObject v2;
	static    MObject uscale;
	static    MObject v10;
	static    MObject vscale;
	static    MObject luxOutFloat;
	static    MObject luxOutColor;
	static    MObject luxOutFresnel;
//---------------------------- automatically created attributes end ------------------------------------

};
