#ifndef MAYATOCorona_TextureBase
#define MAYATOCorona_TextureBase

#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>


class TextureBase :public MPxNode
{
    public:
                    TextureBase();
    virtual         ~TextureBase();

    virtual MStatus compute( const MPlug&, MDataBlock& );
    virtual void    postConstructor();

    static void *  creator();
    static MStatus initialize();

    // Id tag for use with binary file format
    static MTypeId id;

    private:

	static MObject  outColor;

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------

};

#endif