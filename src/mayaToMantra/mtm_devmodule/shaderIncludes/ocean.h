#ifndef OCEAN_H
#define OCEAN_H

#include "utils.h"

void Ocean(
    T_DEFAULTATTR;
    float scale;
	float time;
	vector windUV;
	float observerSpeed;
	float numFrequencies;
	float waveDirSpread;
	float waveLengthMin;
	float waveLengthMax;
	float waveHeight_values[];
	float waveHeight_positions[];
	float waveTurbulence_values[];
	float waveTurbulence_positions[];
	float wavePeaking_values[];
	float wavePeaking_positions[];
	float foamEmission;
	float foamThreshold;
	int colorMode;

	vector outColor;
	float outAlpha;
		   )
{
	float ss = uvCoord.x;
	float tt = uvCoord.y;

	if(ISUVDEFINED(ss, tt))
	{
		outColor = set(1,0,0);
		outAlpha = 1.0;
	}else{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

#endif
