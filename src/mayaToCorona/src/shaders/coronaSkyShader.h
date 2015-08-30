#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class CoronaSkyShader : public MPxNode
{
public:
                    CoronaSkyShader();
    virtual         ~CoronaSkyShader();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

protected:

};
