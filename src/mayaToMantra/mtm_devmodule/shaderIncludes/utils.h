/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/


#ifndef UTILS_H
#define UTILS_H

#define VOP_SHADING
#define VOP_CAST_FLOAT(x)	((float)(x))

#define LOG05 -0.693147180559945
#define UNDEFINED_UV -10000000

#define ISUVDEFINED(U, V) ( abs(U) < 10000 && abs(V) < 10000 )

#define WRAPMAX (1.000001)
#define EPSILON 1e-6

#define point vector
#define SQR(i) ( (i) * (i) )

#define T_DEFAULTATTR vector defaultColor; vector colorGain; vector colorOffset; float alphaGain; float alphaOffset; int alphaIsLuminance; int invert; vector uvCoord; float filter; float filterOffset
// 3d does not need uv's
#define T_DEFAULTATTR3D vector defaultColor; vector colorGain; vector colorOffset; float alphaGain; float alphaOffset; int alphaIsLuminance; int invert; float filter; float filterOffset; vector filterSize; matrix placementMatrix

#define S_DEFAULTATTR vector Cd;\
vector inColor;\
vector transparency;\
vector ambientColor;\
vector incandescence;\
vector normalCamera;\
float diffuse;\
float translucence;\
float translucenceDepth;\
float translucenceFocus;\
int matteOpacityMode;\
float matteOpacity;\
int refractions;\
float refractiveIndex;\
int refractionLimit;\
float lightAbsorbance;\
float surfaceThickness;\
float shadowAttenuation;\
int chromaticAberration;\
float mtm_refrBlur;\
int mtm_refrSamples;\
float mtm_fresnel

#define L_DEFAULTATTR vector inColor;\
		float intensity;\
		int emitDiffuse;\
		int emitSpecular;\
		int decayRate;\
		vector shadowColor;\
		int useRayTraceShadows;\
		float lightRadius;\
		int shadowRays;\
		int useDepthMapShadows;\
		float dmapResolution;\
		float mtm_samplingQuality;\
		float mtm_activeRadius; \
		int mtm_fastShadows; \
		vector outLightColor; \
		export int __nondiffuse; \
		export int __nonspecular


#include "voplib.h"

float lightDecayFactor(int decayRate; float ldist)
{
	// no decay
	if( decayRate == 0)
	{
		return 1.0;
	}
	// linear
	if( decayRate == 1)
	{
		return 1.0/ldist;
	}
	// quadratic
	if( decayRate == 2)
	{
		return 1.0/(ldist * ldist);
	}
	// cubic
	if( decayRate == 3)
	{
		return 1.0/(ldist * ldist * ldist);
	}
	return 1.0;
}

void cabs( vector in_color; vector out_color )
{
	out_color = set(abs(in_color.x), abs(in_color.y), abs(in_color.z));
}

float filteredpulse (float edge0, edge1, x, dx)
{
    float x0 = x - dx/2;
    float x1 = x0 + dx;
    return max (0, (min(x1,edge1)-max(x0,edge0)) / dx);
}

float integral (float t, nedge) { 
   return ((1-nedge)*floor(t) + max(0,t-floor(t)-nedge));
}

float filteredpulsetrain (float edge, period, x, dx)
{
	/* First, normalize so period == 1 and our domain of interest is > 0 */
    float w = dx/period;
    float x0 = x/period - w/2;
    float x1 = x0+w;
    float nedge = edge / period;

	/* Definite integral of normalized pulsetrain from 0 to t */

	float result;
	if( x0 == x1 )
	{
		/* Trap the unfiltered case so it doesn't return 0 (when dx << x). */
		result = (x0 - floor(x0) < nedge) ? 0 : 1;
	}
	else
	{
    	result = (integral(x1,nedge) - integral(x0,nedge)) / w;
	}

	return result;
}


float bias(float b, x)
{
    return pow(x, log(b)/LOG05);
}


void colorBalance(
	int alphaIsLuminance;
	float alphaGain;
	float alphaOffset;
	vector colorGain;
	vector colorOffset;
	int invert;
	vector outColor;
	float outAlpha)
{
	vector one = {1,1,1};
	if(invert != 0)
	{
		outColor = one - outColor;
		outAlpha = one - outAlpha;
	}

	if(alphaIsLuminance != 0)
	{
		outAlpha = luminance( outColor );
	}

	outColor *= colorGain;
	outColor += colorOffset;

	outAlpha *= alphaGain;
	outAlpha += alphaOffset;
}	

void getReflectionA(vector ii; 
					vector nn; 
					float blurDegree; 
					int reflSamples; 
					float reflectivity; 
					vector outReflectedColor; 
					int maxDepth;
					float attenuationDist; 
					vector attenuationColor;
					int doAttenuation)
{
	if( getraylevel() > maxDepth)
	{
		outReflectedColor = set(1,0,0);
		return;
	}
	string rmap = "";
	vector reflRay = reflect(ii,nn);
	vector nf = vop_frontface(nn, ii);
	
	float Kf = 1.0;
	vector bgColor = {0,0,0};
	float rayBias = 0.005;
	float angle_rad = radians(blurDegree);
	float jitter = 1.0;
	int nhit = 0; // number of rays hit
	int areaSamples = reflSamples + 1;
	// from vex source 
	int scount = clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
	vector tint = {1,1,1};
	float thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
	int samples = scount;

	if( doAttenuation == 0)
		attenuationDist = -1.0;
	
	vector hitCf = 0;
	float hitOf = 0;
	vector reflColor = 0;
	float density = 1.0;
	vector hitP;
	//float thresh = 1.0;
	//vector reflColor = vop_trace(nf, nn, P, reflRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "reflect", scount, nhit, "space:world");
    gather( P, 
			reflRay, 
			"samples", samples, 
			"angle", angle_rad,
			"Cf", hitCf,
			"P",  hitP,
			"raystyle", "reflect", 
			"maxdist", attenuationDist,
			"bias", rayBias,
			"rayweight", thresh,
			"variancevar", "Cf",
			"samplebase", jitter)
	{
		if( (attenuationDist > 0.0) && (doAttenuation > 0))
		{
			vector distV = hitP - P;
			float dist = length(distV);
			float att = smooth(0.0, attenuationDist, dist); 
			float atten_interp = exp(-dist*density);
			reflColor += hitCf * (1.0 - att) + attenuationColor * att;
		}else{
			reflColor += hitCf;	
		}
		nhit++;
	}else{
		// no object has been hit, so return environment
		reflColor = reflColor;
	}
	
	reflColor *= (Kf * tint)/max(1,nhit);
	outReflectedColor *= reflColor * reflectivity;
}


void getRefractionA(vector ii; 
					vector nn; 
					float refractionIndex;
					float blurDegree; 
					int refrSamples; 
					int maxDepth;
					float attenuationDist; 
					vector attenuationColor;
					int doAttenuation;
					vector outRefractedColor; 
					float outAlpha;
					)
{
	if( getraylevel() > maxDepth )
	{
		outRefractedColor = set(0,0,1);
		outAlpha = 0.0;
		return;
	}
	string rmap = "";
	vector refrRay = refract(ii,nn, refractionIndex);
	vector nf = vop_frontface(nn, ii);
	
	float Kf = 1.0;
	vector bgColor = {0,0,0};
	float rayBias = 0.005;
	float angle_rad = radians(blurDegree);
	float jitter = 1.0;
	int nhit = 0; // number of rays hit
	int areaSamples = refrSamples + 1;
	// from vex source 
	int scount = clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
	vector tint = {1,1,1};
	float thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
	int samples = scount;
	
	if( doAttenuation == 0)
		attenuationDist = -1.0;

	vector hitCf = 0;
	vector hitOf = 0;
	vector refrColor = 0;
	vector alpha = 0;
	float density = 1.0;
	vector hitP;
	//float thresh = 1.0;
	//vector reflColor = vop_trace(nf, nn, P, reflRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "reflect", scount, nhit, "space:world");
    gather( P, 
			refrRay,
			"samples", samples, 
			"angle", angle_rad,
			"Cf", hitCf,
			"Of", hitOf,
			"P",  hitP,
			"raystyle", "refract", 
			"maxdist", attenuationDist,
			"bias", rayBias,
			"rayweight", thresh,
			"variancevar", "Cf",
			"samplebase", jitter)
	{
		if( (attenuationDist > 0.0) && (doAttenuation > 0))
		{
			float dist = length(hitP-P);
			float att = smooth(0.0, attenuationDist, dist); 
			float atten_interp = exp(-dist*density);
			refrColor += hitCf * (1.0 - att) + attenuationColor * att;
			alpha += hitOf * (1.0 - att);
		}else{
			refrColor += hitCf;	
			alpha += hitOf;
		}
		nhit++;
	}else{
		hitOf = 0.0;
		// no object has been hit, so return environment
	}
	// divide by number of hits
	refrColor *= (Kf * tint)/max(1,nhit);
	outRefractedColor *= refrColor;
	outAlpha = luminance(hitOf)/max(1,nhit);
}

void getReflection(vector ii; vector nn; float blurDegree; int reflSamples; float reflectivity; vector outReflectedColor)
{
	string rmap = "";
	vector reflRay = reflect(ii,nn);
	vector nf = vop_frontface(nn, ii);
	
	// fresnel attenuation - to do if I can add mantra attributes to shader nodes
	//VOPfloat  Kf = 1.0 - vop_dot(nf,-ii);
	float Kf = 1.0;
	vector bgColor = {0,0,0};
	float rayBias = 0.005;
	//float blurDegree = mtm_reflBlur;
	float angle_rad = radians(blurDegree);
	float jitter = 1.0;
	float nhit = 1.0;
	int areaSamples = reflSamples + 1;
	// from vex source 
	int scount = clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
	vector tint = {1,1,1};
	float thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
	int samples = scount;
	
	//float thresh = 1.0;
	vector reflColor = vop_trace(nf, nn, P, reflRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "reflect", scount, nhit, "space:world");
	
	reflColor *= (Kf * tint)/max(1,nhit);
	if( length(outReflectedColor) > 0.0)
		outReflectedColor *= reflColor * reflectivity;
	else
		outReflectedColor = reflColor * reflectivity;
}

void mtm_trace(vector Nf, nN, PP, dir, atten, bg; string envmap;
	float bias, angle, thresh, jitter, density; string style;
	int samples; float nhit; string envobj; vector hitColor; vector hitOpacity )
{
    nhit = 0;
    vector sum = 0;
    vector raydir = 0;
    float  atten_interp, dist;
    vector hitCf, hitOf, hitP, hitOfSum = 0;

    gather( PP, dir,
	    "Cf", hitCf,
	    "Of", hitOf,
	    "P",  hitP,
	    "ray:direction", raydir,
	    "samples", samples,
	    "bias", bias,
	    "angle", angle,
	    "raystyle", style,
	    "rayweight", thresh,
	    "samplebase", jitter,
	    "variancevar", "Cf")
    {
        // Attenuation
        atten_interp = 1;
        if ((dot(Nf, nN) > 0) && (dot(Nf, raydir) < 0) && (density > 0))
        {
            dist = length(hitP-PP);
            atten_interp = exp(-dist*density);
        }
        sum += lerp(atten, hitCf, atten_interp);

        if (vop_maxcomp(hitOf) < 1)
        {
            sum += (set(1,1,1) - hitOf) * vop_trace_environment(envmap, envobj, raydir, bg);
        }
		hitOfSum += hitOf;	
        nhit += 1;
    }
    else
    {
        // Resolve background color
        sum += vop_trace_environment(envmap, envobj, raydir, bg);
        nhit += 1;
    }
    hitColor = sum;
	hitOpacity = hitOfSum;
}


void getRefraction(vector ii, nn; float blurDegree; int refrSamples; float refractiveIndex; vector outRefractedColor; vector outRefractedOpacity)
{        
	string rmap = "";
	ii = normalize(ii);
	nn = normalize(nn);        
	vector nf = vop_frontface(nn, ii);
    vector refrRay = refract(ii, nn, refractiveIndex);
	vector bgColor = {0,0,0};
	float rayBias = 0.005;
	float angle_rad = radians(blurDegree);
	float jitter = 1.0;
	float Kf = 1.0; // attenuation... later
	float nhit = 0.0; // number of hits
	int areaSamples = refrSamples + 1;
	// from vex source 
	int scount = (int)clamp( areaSamples * vop_getrayweight(), 1.0, VOP_CAST_FLOAT(areaSamples));
	vector tint = {1,1,1};
	float thresh = Kf * vop_maxcomp(tint) / VOP_CAST_FLOAT(scount);
	int samples = scount;	
	
	//trace(color1, opacity, parm, myP, { 0, 0, 1 }, 0.00499999989, 1, "scope", "*", "angle", 0, "samples", 0);	
	vector hitColor = set(0,0,0);
	vector hitOpacity = set(0,0,0);
	mtm_trace(nf, nn, P, refrRay, { 0, 0, 0 }, bgColor, rmap, rayBias, angle_rad, thresh, jitter, 0.0, "refract", samples, nhit, "space:world", hitColor, hitOpacity);
	
	hitColor *= (Kf * tint)/max(1,nhit);
	outRefractedColor = hitColor;
	outRefractedOpacity = hitOpacity/max(1,nhit);
}

void getTranslucence(vector  ii; float translucence, translucenceDepth, translucenceFocus; vector outTranslucency)
{
 	float focus = min( translucenceFocus, 0.99999 );
	vector tr = {0,0,0};
	if( translucence > 0.0 )
	{
		illuminance( P )
		{
			float nondiffuse = 0.0;
			// todo
			//lightsource( "__nondiffuse", nondiffuse );

			if( nondiffuse < 1.0 )
			{
				float costheta = dot(normalize(L),normalize(ii));
 				float a = (1.0 + costheta) * 0.5;

 				float trs = pow( pow(a, focus), 1.0/(1.0-focus) );
 
  				tr += Cl * trs * (1.0-nondiffuse);
			}
		}
	}

	outTranslucency = tr * translucence;
}

#endif


#endif