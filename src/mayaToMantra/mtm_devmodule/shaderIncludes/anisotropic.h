

#ifndef ANISOTROPIC_H
#define ANISOTROPIC_H

#include "utils.h"

void Anisotropic(
	S_DEFAULTATTR;

	float angle;
	float spreadX;
	float spreadY;
	float roughness;
	float fresnelIndex;
	vector specularColor;
	float reflectivity;
	vector reflectedColor;
    float mtm_reflBlur;
	int mtm_reflSamples;
	float anisotropicReflectivity;
	
	vector outColor; 
	vector outTransparency; 
	vector outMatteOpacity
)
{
    VOPvector ii = normalize(I);
	VOPnormal nn = normalize(normalCamera);
	vector one = {1,1,1};
	
    vector nf = vop_frontface(nn, ii);	
	vector reflColor = {1,1,1};
	
	if( reflectivity > 0.0)
	{
		getReflection( ii, nn, mtm_reflBlur, mtm_reflSamples, reflectivity, reflectedColor);
		// the more reflective a material is the less transparent it is, so if it is 100% reflective,
		// it is not transparent any more.
		transparency = transparency * (1.0 - reflectivity);
	}else{
		reflectedColor *= 0.0;
	}	

	vector refractedColor = {0,0,0};
	vector refractedOpacity = {1,1,1};
	if( (refractions > 0) && (refractiveIndex != 1.0) && (luminance(transparency) > 0.0))
	{
		getRefraction(ii, nn, mtm_refrBlur, mtm_refrSamples, refractiveIndex, refractedColor, refractedOpacity);
		refractedColor = refractedColor * transparency; // attenuate refracted color by transparency
	}
	
	vector translucenceColor = {0,0,0};
	if( translucence > 0.0)
	{
		vector outTranslucency = {0,0,0};
		getTranslucence(ii, translucence, translucenceDepth, translucenceFocus, outTranslucency);
		translucenceColor = inColor * outTranslucency;
	}	
	
	// Cf = geo surface color
	inColor *=  Cf;

	vector uvw = {0,0,0};
	float urough = spreadX, vrough = spreadY;	
	int tstyle = 0;
	vector clr = {0,0,0};
    vector anisoSpec = vop_lighting("anisotropic", nf, ii, uvw, ambientColor, clr, specularColor, eccentricity, vrough, tstyle);
	
	vector diffuseLighting = diffuse(nf);

	transparency *= (one - refractedOpacity); 
	
	// okay, so we sum up our colors.
	// first, the inColor will be attenuated by transparency
	inColor *= (one - transparency);
	// then, the inColor will be attenuated by diffuse lighing (lambert style)
	inColor *= diffuseLighting;
	// and last the more reflective a material is, the less surfacecolor it shows
	inColor *= (1.0 - reflectivity);

	VOPfloat  Kf = 1.0 - vop_dot(nf,-ii);
	Kf =  (1.0 - mtm_fresnel) + Kf * mtm_fresnel;
	reflectedColor *= Kf;
	refractedColor *= Kf;
	
	outColor = inColor + anisoSpec + reflectedColor + refractedColor + translucenceColor;
	outMatteOpacity = one - transparency;
	outTransparency = transparency;


}

#endif

