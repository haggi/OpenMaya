/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef MARBLE_H
#define MARBLE_H

#include "rman.h"
#include "noise_utils.h"
#include "texture3dutils.h"
#include "utils.h"

void Marble(
	T_DEFAULTATTR3D;
	vector fillerColor;
	vector veinColor;
	float veinWidth;
	float diffusion;
	float contrast;
	float amplitude;
	float ratio;
	vector ripples;
	float depthMin;
	float depthMax;
	int wrap;
	int local;
	float blend;

	vector outColor;
	float outAlpha;
	)
{
	vector depth = set(depthMin, depthMax, 0);
	float edgeDist;
	int outside;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);

	if(outside == 0)
	{
		pp += vfBm( pp, depth, 2, ratio, ripples ) * amplitude;

		float y = pp.y;
		float dy = filterwidth(y);

		float d = min(1, (y - floor(y)) / (1-veinWidth));
		if (d > 0.5)
			d = 1 - d;

		float mixf = (1- filteredpulsetrain(1 - veinWidth, 1, y, dy) / (1 - veinWidth)) * contrast + (1 - contrast) *  pow(2*d, diffusion) * (1 - diffusion * .3);

		outColor = mix(fillerColor, veinColor, 1.0 - mixf);
		outAlpha = luminance(outColor);
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
		
		if(blend > 0 && edgeDist >= 0)
		{
			outColor = blendDefaultColor(blend, defaultColor, edgeDist, outColor);
		}

	}else{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

#endif