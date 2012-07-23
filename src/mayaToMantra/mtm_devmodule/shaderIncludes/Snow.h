/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef SNOW_H
#define SNOW_H

#include "texture3dutils.h"
#include "utils.h"
#include "noise_utils.h"
#include "voplib.h"

void Snow(
	T_DEFAULTATTR3D;
	vector snowColor;
	vector surfaceColor;
	float threshold;
	float depthDecay;
	float thickness;
	int wrap;
	int local;
	float blend;

	vector outColor;
	float outAlpha;
	)
{

	// not sure why the normal has to point downwards...
	vector yAxis = set(0,-1,0);
	vector nn = normalize(N);
	float edgeDist = 0.0;
	int outside = 0;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);
	
	if( outside > 0)
	{
		outColor = defaultColor;
		outAlpha = 0.0;
		return;
	}
	
	vector n;
	if( local == 1 )
		n = vtransform("space:object", nn);
	else
		n = vtransform("space:world", nn);
	n = vtransform( n, placementMatrix );

	float angle = dot(normalize(n),yAxis);		

	if( angle > 0.0 )
	{
		if( angle > threshold )
		{
			vector snow = lerp(snowColor, surfaceColor, 1.0 - thickness );
			float mixv = (1.0 - angle) / (1.0 - threshold);
			mixv = pow( mixv, depthDecay );
			outColor = lerp(snow, surfaceColor, mixv);
		}else{
			outColor = surfaceColor;
		}
	}else{
		outColor = surfaceColor;
	}
	outAlpha = 1.0;
	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
}

#endif