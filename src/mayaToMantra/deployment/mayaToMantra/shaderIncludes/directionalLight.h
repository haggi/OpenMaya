
#include "utils.h"

void DirectionalLight(
    L_DEFAULTATTR;
		vector outLightColor;
		 )
{
	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);

	float ldist = length(L);
	vector LL = -normalize(Lz);
    L = ldist * LL;
	outLightColor = inColor * intensity;
}

void DirectionalLightShadow(
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