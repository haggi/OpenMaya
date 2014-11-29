#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// Plugin CoronaLight Shader Class //

class CoronaLight : public MPxNode
{
public:
                    CoronaLight();
    virtual         ~CoronaLight();

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
	static    MObject roundCornersSamples;
	static    MObject emissionSharpnessFakePoint;
	static    MObject glassMode;
	static    MObject attenuationColor;
	static    MObject emissionSharpnessFake;
	static    MObject reflectivity;
	static    MObject reflectivityMultiplier;
	static    MObject castsShadows;
	static    MObject translucency;
	static    MObject translucencyFraction;
	static    MObject volumeEmissionColor;
	static    MObject anisotropyRotation;
	static    MObject reflectionGlossiness;
	static    MObject volumeEmissionDist;
	static    MObject roundCornersRadius;
	static    MObject bgOverride;
	static    MObject refractionGlossiness;
	static    MObject diffuse;
	static    MObject diffuseMultiplier;
	static    MObject refractivity;
	static    MObject refractivityMultiplier;
//	static    MObject brdfType;
	static    MObject emissionColor;
	static    MObject shadowCatcherMode;
	static    MObject anisotropy;
	static    MObject volumeMeanCosine;
	static    MObject refractionIndex;
	static    MObject emissionDisableSampling;
	static    MObject alphaMode;
	static    MObject attenuationDist;
	static    MObject volumeSSSMode;
	static    MObject emissionGlossyness;
	static    MObject emitLight;
	
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
