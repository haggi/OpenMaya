/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef GRID_H
#define GRID_H

#include "utils.h"
#include "rman.h"

vector contrast(vector c,d; float f) 
{
	vector sum  = c + d;
	sum = sum / 2.0;
	return mix(sum, c, f);
}


void Grid(
		  T_DEFAULTATTR;
		  vector lineColor;
		  vector fillerColor;
		  float uWidth;
		  float vWidth;
		  float contrast;
		  vector outColor;
		  float outAlpha;
		  )
{
	float ss = uvCoord[0];
	float tt = uvCoord[1];
	float du = Du(ss), dv = Dv(tt);

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);

		float dss = abs(Du(ss) * du) + abs(Dv(ss) * dv);
		float dtt = abs(Du(tt) * du) + abs(Dv(tt) * dv);

		dss = clamp( dss, 0, uWidth*0.5 ) * filter + filterOffset*2; 
		dtt = clamp( dtt, 0, vWidth*0.5 ) * filter + filterOffset*2; 
	
		float f = filteredpulsetrain(uWidth, 1, ss + uWidth*0.5, dss) * filteredpulsetrain(vWidth, 1, tt + vWidth*0.5, dtt);

		vector lc = contrast(lineColor, fillerColor, contrast);
		vector fc = contrast(fillerColor, lineColor, contrast);

		outColor = mix(lc, fc,f);
		outAlpha = 1-f; 
	}else{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

#endif