#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class dots :public MPxNode
{
    public:
                    dots();
    virtual         ~dots();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject outside;
	static    MObject vdelta;
	static    MObject udelta;
	static    MObject mapping;
	static    MObject v1;
	static    MObject v2;
	static    MObject uscale;
	static    MObject inside;
	static    MObject vscale;
	static    MObject luxOutFloat;
//---------------------------- automatically created attributes end ------------------------------------

};
