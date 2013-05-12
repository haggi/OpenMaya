#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class harlequin :public MPxNode
{
    public:
                    harlequin();
    virtual         ~harlequin();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------

};
