#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin asLayeredShader  Shader Class //

class asLayeredShader : public MPxNode
{
public:
	asLayeredShader();
	virtual         ~asLayeredShader();

	static  void *  creator();
	virtual MStatus compute(const MPlug&, MDataBlock&);
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
