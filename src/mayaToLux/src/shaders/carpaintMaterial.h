#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

#include "carpaintMaterialOverride.h"

// Plugin carpaint Shader Class //


// This class will create a new shader. Shaders are custom dependency
// graph objects so we will derive this class from the basic DG node
// type MPxNode
//

class carpaint : public MPxNode
{
public:
                    carpaint();
    virtual         ~carpaint();

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

//---------------------------- automatically created attributes start ------------------------------------
	static    MObject compo_visible_indirect_material;
	static    MObject carpaint_M3;
	static    MObject carpaint_Ks1;
	static    MObject carpaint_Ks3;
	static    MObject carpaint_Ks2;
	static    MObject compo_visible_emission;
	static    MObject compo_override_alpha;
	static    MObject carpaint_name;
	static    MObject compo_visible_material;
	static    MObject carpaint_M1;
	static    MObject carpaint_R1;
	static    MObject carpaint_R2;
	static    MObject carpaint_R3;
	static    MObject carpaint_M2;
	static    MObject compo_visible_indirect_emission;
	static    MObject carpaint_Kd;
	static    MObject bumpmap;
	static    MObject compo_override_alpha_value;
//---------------------------- automatically created attributes end ------------------------------------

   // Translucence coefficient
   static MObject  aTranslucenceCoeff;

   // Diffuse Reflectivity
   static MObject  aDiffuseReflectivity;

   // Surface color
   static MObject  aColor;

   // Incandescence
   static MObject  aIncandescence;

   // Surface transparency
   static MObject  aInTransparency;

   // Output color
   static MObject  aOutColor;

   // Output transparency
   static MObject  aOutTransparency;

   // X component of surface normal
   static MObject  aNormalCameraX;

   // Y component of surface normal
   static MObject  aNormalCameraY;

   // Z component of surface normal
   static MObject  aNormalCameraZ;

   // Surface normal
   static MObject  aNormalCamera;

   // X component of light direction vector
   static MObject  aLightDirectionX;

   // Y component of light direction vector
   static MObject  aLightDirectionY;

   // Z component of light direction vector
   static MObject  aLightDirectionZ;

   // Light direction vector
   static MObject  aLightDirection;

   // Red component of light intensity
   static MObject  aLightIntensityR;

   // Green component of light intensity
   static MObject  aLightIntensityG;

   // Blue component of light intensity
   static MObject  aLightIntensityB;

   // Light Intensity vector
   static MObject  aLightIntensity;

   // Ambient flag
   static MObject  aLightAmbient;

   // Diffuse flag
   static MObject  aLightDiffuse;

   // Specular flag
   static MObject  aLightSpecular;

   // Shadow Fraction flag
   static MObject   aLightShadowFraction;

   // Pre Shadow Intensity
   static MObject   aPreShadowIntensity;

   // Light blind data
   static MObject   aLightBlindData;

   // Light data array
   static MObject   aLightData;

};
