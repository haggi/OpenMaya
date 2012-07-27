/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef BULGE_H
#define BULGE_H

#include "utils.h"
#include "rman.h"

void Bulge(
	T_DEFAULTATTR;
	vector uvFilterSize;
	float uWidth;
	float vWidth;
	float outAlpha;
	vector outColor;
	)
{
	float ss = uvCoord.x;
	float tt = uvCoord.y;

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);
    
		float a = smoothstep(uWidth / 2, .5, ss) * (1 - smoothstep(0.5, 1 - (uWidth / 2), ss));
		float b = smoothstep(vWidth / 2, .5, tt) * (1 - smoothstep(0.5, 1 - (vWidth / 2), tt));
		outAlpha = sqrt(abs(a * b));
		outColor = set(outAlpha,outAlpha,outAlpha);
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	}
	else
	{
		outColor = defaultColor;
		outAlpha = 1.0;
	}
}
   
#endif

