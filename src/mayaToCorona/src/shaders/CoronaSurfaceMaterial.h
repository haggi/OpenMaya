#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaSurface Shader Class //

class CoronaSurface : public MPxNode
{
public:
                    CoronaSurface();
    virtual         ~CoronaSurface();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

protected:

	static    MObject opacity;
	static    MObject opacityMultiplier;
	static    MObject emissionMultiplier;
	static    MObject volumeScatteringAlbedo;
	static    MObject fresnelIor;
	static    MObject fresnelIorMap;
	static    MObject roundCornersSamples;
	static    MObject emissionSharpnessFakePoint;
	static    MObject glassMode;
	static    MObject attenuationColor;
	static    MObject emissionSharpnessFake;
	static    MObject reflectivity;
	static    MObject reflectivityMultiplier;
	static    MObject reflectionGlossiness;
	static    MObject refractionGlossiness;
	static    MObject refractionIndex;
	static    MObject refractionIndexMap;
	static    MObject castsShadows;
	static    MObject glassType;
	static    MObject translucency;
	static    MObject translucencyFraction;
	static    MObject volumeEmissionColor;
	static    MObject anisotropyRotation;
	static    MObject volumeEmissionDist;
	static    MObject roundCornersRadius;
	static    MObject roundCornersMultiplier;
	static    MObject bgOverride;
	static    MObject diffuse;
	static    MObject diffuseMultiplier;
	static    MObject refractivity;
	static    MObject refractivityMultiplier;
	static    MObject emissionColor;
	static    MObject shadowCatcherMode;
	static    MObject anisotropy;
	static    MObject volumeMeanCosine;
	static    MObject emissionDisableSampling;
	static    MObject alphaMode;
	static    MObject attenuationDist;
	static    MObject volumeSSSMode;
	static    MObject iesProfile;

   // Translucence coefficient
   static MObject  aTranslucenceCoeff;

   // Diffuse Reflectivity
   static MObject  aDiffuseReflectivity;

   // Surface color
   //static MObject  aColor;

   // Incandescence
   //static MObject  aIncandescence;

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
