#include <math.h>
#include <maya/MPxNode.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxShadingNodeOverride.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MFragmentManager.h>


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

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------


    // Input attributes
    static MObject aColor;                  // Input color
    static MObject aGamma;                  // R, G, B correction

    // Output attributes
    static MObject aOutColor;
};

//
// Override declaration
class TextureBaseOverride : public MHWRender::MPxShadingNodeOverride
{
public:
    static MHWRender::MPxShadingNodeOverride* creator(const MObject& obj);

    virtual ~TextureBaseOverride();

    virtual MHWRender::DrawAPI supportedDrawAPIs() const;

    virtual MString fragmentName() const;

private:
    TextureBaseOverride(const MObject& obj);

    MString fFragmentName;
};
