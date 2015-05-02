#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaLayered  Shader Class //

class CoronaLayered : public MPxNode
{
public:
                    CoronaLayered ();
    virtual         ~CoronaLayered ();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();
    virtual void    postConstructor();

    static  MTypeId   id;

protected:
	static    MObject baseMaterial;
	static	  MObject materialEntry;
	static    MObject materialEntryMtl;
	static    MObject materialEntryAmount;
	static    MObject outColor;
};
