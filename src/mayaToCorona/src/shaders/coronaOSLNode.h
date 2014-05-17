#ifndef MAYATOCorona_OSLNode
#define MAYATOCorona_OSLNode

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class OSLNode :public MPxNode
{
    public:
                    OSLNode();
    virtual         ~OSLNode();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  color;
	static MObject  script;
	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------

};

#endif