#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class gaussian :public MPxNode
{
    public:
                    gaussian();
    virtual         ~gaussian();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject wavelength;
	static    MObject energy;
	static    MObject width;
	static    MObject luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------

};
