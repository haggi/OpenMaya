#ifndef PHONG_H
#define PHONG_H

#include <voptype.h>
#include <voplib.h>


// the parameter is called inColor because color seems to be a reserved word
void Phong(vector uv; 
		   vector inColor; 
		   vector transparency; 
		   vector ambientColor; 
		   vector incandescence;
		   float diffuseValue;
		   float translucence;
		   float cosinePower;
		   vector specularColor;
		   float reflectivity;
		   vector reflectedColor;
		   int refractions;
		   //int reflections;
		   float refractiveIndex;
		   float mtm_refrBlur;
		   float mtm_reflBlur;
		   float mtm_refrSamples;
		   float mtm_reflSamples;
		   //float refractionLimit;
		   //float lightAbsorbance;
		   //float surfaceThickness;
		   //float shadowAttenuation;
		   //int chromaticAberration;
		   //int reflectionLimit;
		   //float reflectionSpecularity;
		   vector outColor;
		   vector outTransparency;
		   vector outMatteOpacity)
{

    VOPvector ii = normalize(I);
    VOPnormal nn = normalize(N);
	vector	translucIllum = {0,0,0};
    
	if( translucence > 0.0)
	{
		VOPnormal neg = -nn;
		translucIllum = diffuse(neg) * translucence * Cf * inColor;
	}	
	// face forward
    VOPnormal nf = vop_frontface(nn, ii);
	// calculate normal lambert diffuse
	vector lambDiff = diffuse(nf);
	// in this version I use the classic phong
	vector phongColor = phong( nf, -ii, cosinePower);
	// phong lighting model, lighting node includes diffuse as well
    // vector clr = vop_lighting("phong", nf, ii, {0.0, 0.0, 0.0}, { 0.5, 0.5, 0.5 }, { 0.5, 0.5, 0.5 }, { 1, 1, 1 }, 0.0890000015, 0, 0);
	
	vector reflColor = {0,0,0};
	vector refrColor = {0,0,0};

	string rmap = "";

	VOPfloat  Kf = 1.0;

	vector bgColor = {0,0,0};
	float rayBias = 0.005;

	// for blurry reflections implement blurDegree 
	float blurDegree = 0.0;
	float angle_rad = radians(blurDegree);
	float jitter = 1.0;
	float nhit = 1.0;
	int areaSamples = 1;
	// from vex source 
	VOPint scount;
	vector tint = {1,1,1};
	VOPfloat thresh;
	int samples = 1;

	// lets do reflections
	if( reflectivity > 0.0)
	{
		rmap = "";
		VOPvector reflRay = reflect(ii,nn);

		// fresnel attenuation - to do if I can add mantra attributes to shader nodes
		//VOPfloat  Kf = 1.0 - vop_dot(nf,-ii);
		Kf = 1.0;

		bgColor = {0,0,0};
		rayBias = 0.005;

		// for blurry reflections implement blurDegree 
		blurDegree = mtm_reflBlur;
		angle_rad = radians(blurDegree);
		jitter = 1.0;
		nhit = 1.0; // ?
		areaSamples = mtm_reflSamples + 1;
		// from vex source 
		scount = clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
		tint = {1,1,1};
		thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
		samples = scount;
		
		//float thresh = 1.0;
		reflColor = vop_trace(nf, nn, P, reflRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "reflect", scount, nhit, "space:world");
		
		reflColor *= (Kf * tint)/max(1,nhit);
		reflColor *= reflectivity;
	}
	if( (refractions > 0) && (refractiveIndex != 1.0))
	{
		rmap = "";
		VOPvector refrRay = refract(ii, nn, refractiveIndex);

		// fresnel attenuation - to do if I can add mantra attributes to shader nodes
		//VOPfloat  Kf = 1.0 - vop_dot(nf,-ii);
		Kf = 1.0;

		bgColor = {0,0,0};
		rayBias = 0.005;

		// for blurry reflections implement blurDegree 
		blurDegree = mtm_refrBlur;
		angle_rad = radians(blurDegree);
		jitter = 1.0;
		nhit = 1.0; // ?
		areaSamples = mtm_refrSamples + 1;
		// from vex source 
		scount = clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
		tint = {1,1,1};
		thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
		samples = scount;
		
		//float thresh = 1.0;
		refrColor = vop_trace(nf, nn, P, refrRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "refract", samples, nhit, "space:world");
		
		refrColor *= (Kf * tint)/max(1,nhit);
	}

	// Cf = geo surface color
	vector diffuseColorResult =  inColor * Cf * lambDiff * diffuseValue;
	if( luminance(transparency) > 0.0)
		diffuseColorResult *= ({1,1,1} - transparency);

	outColor = diffuseColorResult + phongColor * specularColor + incandescence + reflColor + refrColor * transparency + translucIllum;
	outTransparency = transparency;
	outMatteOpacity = transparency;
}

//setAttr "phong1.color" -type double3 0.450004 0.450004 0.450004 ;
//setAttr "phong1.transparency" -type double3 0.0666667 0.0666667 0.0666667 ;
//setAttr "phong1.ambientColor" -type double3 0.072221 0.072221 0.072221 ;
//setAttr "phong1.incandescence" -type double3 0.0666667 0.0666667 0.0666667 ;
//setAttr "phong1.diffuse" 0.75;
//setAttr "phong1.translucence" 0.05;
//setAttr "phong1.translucenceDepth" 0.694444;
//setAttr "phong1.translucenceFocus" 0.561111;
//setAttr "phong1.cosinePower" 24.322222;
//setAttr "phong1.specularColor" -type double3 0.438895 0.438895 0.438895 ;
//setAttr "phong1.reflectivity" 0.561111;
//setAttr "phong1.reflectedColor" -type double3 0.0611124 0.0611124 0.0611124 ;
//setAttr "phong1.refractions" 1;
//setAttr "phong1.refractiveIndex" 1.089722;
//setAttr "phong1.refractionLimit" 6.222222;
//setAttr "phong1.refractionLimit" 7;
//setAttr "phong1.lightAbsorbance" 0.833333;
//setAttr "phong1.surfaceThickness" 0.0333333;
//setAttr "phong1.shadowAttenuation" 0.538889;
//setAttr "phong1.chromaticAberration" 1;
//setAttr "phong1.reflectionLimit" 2;
//setAttr "phong1.reflectionSpecularity" 0.227778;

#endif