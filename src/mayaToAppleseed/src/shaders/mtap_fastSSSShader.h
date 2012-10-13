#ifndef MTAP_FASTSSSSURFACE_SHADER_H
#define MTAP_FASTSSSSURFACE_SHADER_H

#include <maya/MPxNode.h>

const MString mtap_fastSSSShaderName = "mtap_fastSSSShader";

class mtap_fastSSSShader : public MPxNode
{
public:
					mtap_fastSSSShader();
	virtual			~mtap_fastSSSShader();

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
   // SSS
   static MObject  albedo;
   static MObject  ambient_sss;
   static MObject  diffuse;
   static MObject  distortion;
   static MObject  light_samples;
   static MObject  occlusion_samples;
   static MObject  power;
   static MObject  scale;
   static MObject  view_dep_sss;

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