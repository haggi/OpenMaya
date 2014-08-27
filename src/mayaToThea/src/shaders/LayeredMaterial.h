#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin Layered Shader Class //


// This class will create a new shader. Shaders are custom dependency
// graph objects so we will derive this class from the basic DG node
// type MPxNode
//

class Layered : public MPxNode
{
public:
                    Layered();
    virtual         ~Layered();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();


    // postConstructor:
    // The postConstructor method allows us to call MPxNode member
    // functions during initialization. Internally maya creates two
    // objects when a user defined node is created, the internal MObject
    // and the user derived object. The association between the these
    // two objects is not made until after the MPxNode constructor is
    // called. This implies that no MPxNode member function can be called
    // from the MPxNode constructor. The postConstructor will get called
    // immediately after the constructor when it is safe to call any
    // MPxNode member function.
    //

    virtual void    postConstructor();

    static  MTypeId   id;  // The IFF type id

protected:

	static MObject  baseShader;
	static MObject  layerWeight;
	static MObject  layerTexture;
	static MObject  layerShader;
	static MObject  outColor;

	static MObject  aTranslucenceCoeff;
	static MObject  aDiffuseReflectivity;
	static MObject  aColor;
	static MObject  aIncandescence;
	static MObject  aInTransparency;
	static MObject  aOutTransparency;
	static MObject  aNormalCameraX;
	static MObject  aNormalCameraY;
	static MObject  aNormalCameraZ;
	static MObject  aNormalCamera;
	static MObject  aLightDirectionX;
	static MObject  aLightDirectionY;
	static MObject  aLightDirectionZ;
	static MObject  aLightDirection;
	static MObject  aLightIntensityR;
	static MObject  aLightIntensityG;
	static MObject  aLightIntensityB;
	static MObject  aLightIntensity;
	static MObject  aLightAmbient;
	static MObject  aLightDiffuse;
	static MObject  aLightSpecular;
	static MObject   aLightShadowFraction;
	static MObject   aPreShadowIntensity;
	static MObject   aLightBlindData;
	static MObject   aLightData;

//---------------------------- automatically created attributes start ------------------------------------


//---------------------------- automatically created attributes end ------------------------------------


};
