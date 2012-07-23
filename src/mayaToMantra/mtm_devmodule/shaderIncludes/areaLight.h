
#include "utils.h"

void AreaLight(
    L_DEFAULTATTR;
	vector outLightColor;
					  )
{
	string shape;
	renderstate("light:areashape", shape);

	__nondiffuse = (int)(emitDiffuse == 0);
	__nonspecular = (int)(emitSpecular == 0);
	
	outLightColor = inColor * intensity;
}

void AreaLightShadow(
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
