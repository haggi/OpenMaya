
#include "utils.h"

float getConeCoverage(vector LL; float cmin, cmax, croll)
{
    float       cone;
    if (cmax < -0.999) 
		cone = 1;
    //else cone = smooth(cmin, cmax, -dot(LL, Lz), croll);
    else 
		cone = smooth(cmin, cmax, -dot(LL, Lz));
    return cone;
}


void SpotLight(
    L_DEFAULTATTR;
	float coneAngle;
	float penumbraAngle;
	float dropoff;
					  )
{
	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);

	float ldist = length(L);
	vector LL = L / ldist;

    float angle = cos(min(coneAngle+penumbraAngle/2.0, M_PI));
    float delta = cos(min(coneAngle-penumbraAngle/2.0, M_PI));
    float conemin = min(angle, delta);
    float conemax = max(angle, delta);
    vector light = getConeCoverage(LL, conemin, conemax, 1.0);
    export float __illuminateangle = min(coneAngle+penumbraAngle, M_PI);

	float drop = 1.0;
	if( dropoff > 0.0)
	{
		drop = (1.0 - (-dot(LL, Lz)));
		if( drop > 1.0)
			drop = 1.0;
		if( drop < 0.0)
			drop = 0.0;
		drop = pow(1.0 - drop, dropoff);
	}
	vector lcolor = inColor * intensity;
	float decayFactor = lightDecayFactor(decayRate, ldist);
	lcolor *= decayFactor;
	outLightColor = light * lcolor * drop;
}

void SpotLightShadow(
    L_DEFAULTATTR;
	float coneAngle;
	float penumbraAngle;
	float dropoff;
					  )
{
	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);

    vector shad = 1.0;
	
	if( mtm_fastShadows == 1)
	{	
		shad = fastshadow(Ps, L, -1);
	}else{
		shad = filtershadow(Ps, L, -1);
		trace(Ps, normalize(L), Time, "maxdist", length(L), "raystyle", "shadow", "Of", shad);
		shad = {1,1,1} - shad;
	}
	if( length(shadowColor) > 0.0)
		shad *= shadowColor;
	outLightColor = shad;
}

