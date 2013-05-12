#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class fresnelcolor :public MPxNode
{
    public:
                    fresnelcolor();
    virtual         ~fresnelcolor();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject Kr;
	static    MObject luxOutFresnel;
//---------------------------- automatically created attributes end ------------------------------------

};
