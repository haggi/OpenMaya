/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef BROWN_H
#define BROWN_H

#include "texture3dutils.h"
#include "utils.h"
#include "noise_utils.h"

void Brownian(
	T_DEFAULTATTR3D;
	float increment;
	float lacunarity;
	int octaves;
	vector weight3d;
	int wrap;
	int local;
	float blend;

	vector outColor;
	float outAlpha;)
{
	float edgeDist;
	int outside;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);

	if(outside < 1)
	{
		vector pn = pp * weight3d;

		float i = 1.0;
		float amp = 1;
		float sum = 0;
		vector pq = pn;
		float filterWidth = area(pq);
		float inc = (increment < 1.0 ? 1.0 - increment : increment);
		float ooctaves = octaves;
		for(i = 0.99; i < ooctaves; i += inc)
		{
			sum += amp * filteredsnoise(pq, filterWidth);
			amp *= inc;
			pq *= lacunarity;
			filterWidth *= lacunarity;
		}

		outAlpha = sum;
		outColor = clamp((outAlpha + 1.0) / 2.0, 0.0, 1.0);
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);

		if(blend > 0 && edgeDist >= 0)
		{
			outColor = blendDefaultColor(blend, defaultColor, edgeDist, outColor);
		}
	}
	else
	{
		outColor = defaultColor;
		outAlpha = 0;
	}
}
	
#endif

