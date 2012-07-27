/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef SOLIDFRACTAL_H
#define SOLIDFRACTAL_H

#include "texture3dutils.h"
#include "utils.h"
#include "noise_utils.h"

void SolidFractal(
    T_DEFAULTATTR3D;
	float threshold;
	float amplitude;
	float ratio;
	float frequencyRatio;
	vector ripples;
	vector depth;
	float bias;
	int inflection;
	int animated;
	float time;
	float timeRatio;
	int wrap;
	int local;
	float blend;

	vector outColor;
	float outAlpha;)
{

	float edgeDist;
	int outside;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);

	if(outside == 0)
	{
		float timeP = 0;
		if(animated == 1)
			timeP = time * timeRatio;

		float fractal;
		if(inflection == 1)
			fractal = fTurbulence( pp, timeP, 1.0, frequencyRatio, depth, ratio, ripples);
		else
			//vector point; float i_time; float i_amplitude; float i_lacunarity; float i_frequencyRatio; vector i_octaves; float i_ratio; vector i_ripples; 
			fractal = fBrownianNoise( pp, timeP, 1.0, 1.0, frequencyRatio, depth, ratio, ripples );

		fractal += threshold;
		
		float sn = sign(fractal);

		if(bias > 0.0)
		{
			fractal = sn * pow(abs(fractal), 1.0 - bias * 0.5);
		}
		else if(bias < 0.0)
		{
			fractal = sn * pow(abs(fractal), 1.0 / (1.0 + bias));
		}

		fractal *= amplitude;
		outAlpha = clamp(fractal + threshold, 0.0, 1.0);
		outColor = lerp(0.0, 1.0, outAlpha);

		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
		
	}
	else
	{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

#endif

