/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef TEXTURE3D_UTILS_H
#define TEXTURE3D_UTILS_H

#include "rman.h"

vector transformPoint(
	float blend;
	int local;
	int wrap;
	float edgeDist;
	int outside;
	matrix placementMatrix;)
{
	vector pp;

	edgeDist = 0.0;

	// if local is on, the current point will be transformed into object space
	if(local != 0)
		pp = ptransform("space:object", P);
	else
		pp = ptransform("space:world", P);

	vector pointTransformed = ptransform(pp, placementMatrix);
	
	// if the point is out of default space from 0-1, then if wrap is on
	// set value
	if( pointTransformed.x >  1 ||
	    pointTransformed.x < -1 ||
		pointTransformed.y >  1 ||
		pointTransformed.y < -1 ||
		pointTransformed.z >  1 ||
		pointTransformed.z < -1)
	{
		if(wrap != 0)
		{
			outside = 0;
		}
		else
		{
			outside = 1;
		}
	}
	else
	{
		outside = 0.0;
		if(wrap == 0 && blend > 0.0)
		{
			edgeDist = min(abs(pointTransformed.x + 1) / 2, abs(pointTransformed.x - 1) / 2, 
				abs(pointTransformed.y + 1) / 2, abs(pointTransformed.y - 1) / 2, 
				abs(pointTransformed.z + 1) / 2, abs(pointTransformed.z - 1) / 2);
		}
	}
	return pointTransformed;
}


vector blendDefaultColor(
	float blend;
	vector defaultColor;
	float edgeDist;
	vector outColor;)
{
	return  mix(defaultColor, outColor, smoothstep(0.1 * blend, 0.5 * blend, edgeDist));
}                                               

#endif

