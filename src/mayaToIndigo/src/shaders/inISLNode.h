#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class inISLNode : public MPxNode
{
public:
                    inISLNode();
    virtual         ~inISLNode();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    //virtual void    postConstructor();

    static  MTypeId   id;

protected:

	static    MObject islData;
};
