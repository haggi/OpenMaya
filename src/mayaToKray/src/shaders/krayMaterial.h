#ifndef mtkr_AO_SHADER_H
#define mtkr_AO_SHADER_H

#include <maya/MPxNode.h>

const MString KrayMaterialName = "KrayMaterial";

class KrayMaterial : public MPxNode
{
public:
					KrayMaterial();
	virtual			~KrayMaterial();

	static  void *	creator();
	virtual MStatus	compute( const MPlug&, MDataBlock& );
	static  MStatus	initialize();


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

	virtual void	postConstructor(); 

	static  MTypeId   id;  // The IFF type id

protected:

	static MObject backSide; // other material
	static MObject blurMaxRays; // double value
	static MObject minRays;
	static MObject maxRays;
	static MObject noiseTolerance;
	static MObject blurTolerance;
	static MObject bumpDepth;
//	static MObject color;
//	static MObject diffuse;
	static MObject diffuseMode;
	static MObject edgeDetectSlot;
	static MObject flags;
	static MObject forceColor;
	static MObject fresnel;
	static MObject glossiness;
	static MObject ior;
	static MObject light;
	static MObject lightGroup;
	static MObject passiveLem;
	static MObject reflection;
	static MObject reflectionBlur;
	static MObject reflectionBlurLights;
	static MObject refractionBlur;
	static MObject specularity;
	static MObject texture_bump;
	static MObject texture_color;
	static MObject texture_diffuse;
	static MObject texture_light;
	static MObject texture_reflection;
	static MObject texture_specularity;
	static MObject texture_translucency;
	static MObject texture_transparency;
	static MObject translucency;
//	static MObject transparency;
	static MObject transparencyAffects;

   // Diffuse Reflectivity
   static MObject  aDiffuseReflectivity;

   // Red component of surface color
   static MObject  aColorR;

   // Green component of surface color
   static MObject  aColorG;

   // Blue component of surface color
   static MObject  aColorB;

   // Surface color
   static MObject  aColor;

   // Red component of incandescence
   static MObject  aIncandescenceR;

   // Green component of incandescence
   static MObject  aIncandescenceG;

   // Blue component of incandescence
   static MObject  aIncandescenceB;

   // Incandescence
   static MObject  aIncandescence;

   // Red component of surface transparency
   static MObject  aInTransR;

   // Green component of surface transparency
   static MObject  aInTransG;

   // Blue component of surface transparency
   static MObject  aInTransB;

   // Surface transparency
   static MObject  aInTransparency;

   // Red component of output color
   static MObject  aOutColorR;

   // Green component of output color
   static MObject  aOutColorG;

   // Blue component of output color
   static MObject  aOutColorB;

   // Output color
   static MObject  aOutColor;

   // Red component of output transparency
   static MObject  aOutTransR;

   // Green component of output transparency
   static MObject  aOutTransG;

   // Blue component of output transparency
   static MObject  aOutTransB;

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
   static MObject	aLightShadowFraction;

   // Pre Shadow Intensity
   static MObject	aPreShadowIntensity;

   // Light blind data
   static MObject	aLightBlindData;

   // Light data array
   static MObject	aLightData;
 
};


#endif