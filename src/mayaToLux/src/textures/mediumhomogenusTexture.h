#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class mediumhomogenus :public MPxNode
{
    public:
                    mediumhomogenus();
    virtual         ~mediumhomogenus();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject g2;
	static    MObject g1;
	static    MObject sigma_s;
	static    MObject g;
	static    MObject sigma_a;
	static    MObject luxOutColor;
//---------------------------- automatically created attributes end ------------------------------------

};
