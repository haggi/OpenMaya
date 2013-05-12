#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class tabulateddata :public MPxNode
{
    public:
                    tabulateddata();
    virtual         ~tabulateddata();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject filename;
	static    MObject luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------

};
