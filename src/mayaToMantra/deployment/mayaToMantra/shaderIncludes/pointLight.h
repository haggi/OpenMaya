#include "utils.h"

void PointLight(
		L_DEFAULTATTR;
		vector outLightColor;
				 )
{
	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);


	float ldist = length(L);
	vector lcolor = inColor * intensity;

	float decayFactor = lightDecayFactor(decayRate, ldist)
	lcolor *= decayFactor;

	outLightColor = lcolor;
}

void PointLightShadow(
	L_DEFAULTATTR;
	vector outLightColor;
				 )
{
	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);

    float shad = fastshadow(Ps, L, -1);
	if( shad == 0 )
		outLightColor = shadowColor;
	else
		outLightColor = shad;
}