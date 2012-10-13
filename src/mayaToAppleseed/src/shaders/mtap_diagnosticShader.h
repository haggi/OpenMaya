#ifndef MTAP_DIAGNOSTIC_SHADER_H
#define MTAP_DIAGNOSTIC_SHADER_H

#include <maya/MPxNode.h>

const MString mtap_diagnosticShaderName = "mtap_diagnosticShader";

class mtap_diagnosticShader : public MPxNode
{
public:
					mtap_diagnosticShader();
	virtual			~mtap_diagnosticShader();

	static  void *	creator();
	virtual MStatus	compute( const MPlug&, MDataBlock& );
	static  MStatus	initialize();

	virtual void	postConstructor(); 

	static  MTypeId   id;  // The IFF type id

protected:
   static MObject  mode;

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