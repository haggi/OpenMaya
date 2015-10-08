#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class CoronaFrontBack : public MPxNode
{
public:
                    CoronaFrontBack();
    virtual         ~CoronaFrontBack();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

	static MObject  frontMaterial;
	static MObject  backMaterial;
	static MObject  outColor;

protected:

};
