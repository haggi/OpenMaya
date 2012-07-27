/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/
#ifndef LAMBERT_H
#define LAMBERT_H

#define VOP_SHADING
#define VOP_SURFACE

#include "voptype.h"
#include "voplib.h"
#include "utils.h"

void Lambert(
	S_DEFAULTATTR;
	int mtm_orenNayar;
	vector outColor; 
	vector outTransparency; 
	vector outMatteOpacity
)
{
    VOPvector ii = normalize(I);
	VOPnormal nn = normalize(normalCamera);
	vector one = {1,1,1};
	
	vector trans = transparency;
    vector nf = vop_frontface(nn, ii);	

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
	
	// Cd = geo surface color
	if( luminance(Cd) > 0.0)
		inColor *=  Cd;

	vector uvw = {0,0,0};
	float urough = 0.0, vrough = 0.0;	
	int tstyle = 0;
	vector clr = {0,0,0};
	string diffuseType = "lambert";
	if( mtm_orenNayar > 0)
		diffuseType = "oren";
	//vop_lighting(string lmodel;vector nf, ii, uv, amb, diff, spec; float urough, vrough; int tstyle)
	vector specularColor = {0,0,0};
    //vector diffuseLighing = vop_lighting(diffuseType, nf, ii, uvw, ambientColor, clr, specularColor, urough, vrough, tstyle);
	
	vector diffuseLighting = diffuse(nf);

	trans *= (one - refractedOpacity); 
	
	// okay, so we sum up our colors.
	// first, the inColor will be attenuated by transparency
	inColor *= (one - trans);
	// then, the inColor will be attenuated by diffuse lighing (lambert style)
	inColor *= diffuseLighting;

	// experimental
	VOPfloat  Kf = 1.0 - vop_dot(nf,-ii);
	Kf =  (1.0 - mtm_fresnel) + Kf * mtm_fresnel;
	refractedColor *= Kf;
	
	outColor = inColor + refractedColor + translucenceColor + incandescence;
	outMatteOpacity = one - transparency;
	outTransparency = transparency;
}

#endif
