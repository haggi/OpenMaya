/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/
#ifndef PLACE2DTEX_H
#define PLACE2DTEX_H

#include "utils.h"
#include "rman.h"

void Place2dTexture(
		vector uv; 
		vector uvCoord;
		float noiseU; 
		float noiseV;
		float rotateFrame;
		float coverageU;
		float coverageV;
		float repeatU;
		float repeatV;
		float translateFrameU;
		float translateFrameV;
		int wrapU;
		int wrapV;
		float offsetU;
		float offsetV;
		int stagger;
		int mirrorU;
		int mirrorV;
		float rotateUV;
		vector repeatUVW; 
		
		vector outUVW)
{
	vector uvw = {0,0,0};
	//vector uv = {0,0,0};

    int bound_uv = isbound("uv");
    if( bound_uv ) 
	{
        uvw = uv;
    } else {
        uvw = set(s, t, 0);
    }
	// if a projection transform node is connected, we get a value of -1000 in the uvCoord
	// so we use the uvCoord as uvs
	if( uvCoord.z < -999)
	{
		uvw = uvCoord;
		uvw.z = 0.0;
	}

	float ss = uvw.x;
	float tt = uvw.y;
  
	float outU = ss;
	float outV = tt;
	
	vector p1, p2, P1;
	float axis;
	p1 = set(ss * 18, tt * 18, 0);
	p2 = set(18, 18, 18);
	if(noiseU > 0.0)
	{
		outU += 1.25 * noiseU * (pnoise(p1, p2) * 2 - 1);
	}
  
	if(noiseV > 0.0)
	{
		outV += 1.25 * noiseV * (pnoise(p1, p2) * 2 - 1);
	}

	outUVW = set(outU, outV, 1.0);
  
	if(rotateFrame != 0.0)
	{
		P1 = set(0.5, 0.5, 0.0);
		vector R = set(0,0,degrees(rotateFrame));
		outUVW = outUVW * invert(maketransform(0,0,{0,0,0},R,{1,1,1},P1));
		outU = outUVW.x;
		outV = outUVW.y;
	}
	
	outU -= translateFrameU;
	outV -= translateFrameV;

	if(mod(outU, WRAPMAX) - coverageU > EPSILON|| 
	   mod(outV, WRAPMAX) - coverageV > EPSILON ||
		((wrapU == 0) && (outU < 0 || (outU * repeatU) - repeatU > EPSILON)) ||
	   ((wrapV == 0) && (outV < 0 || (outV * repeatV) - repeatV > EPSILON)))
	{
		outUVW.x = UNDEFINED_UV;
		outUVW.y = UNDEFINED_UV;
	}else{
		if(coverageU < 1.0)
		{
			outU = mod(outU, 1);
		}
		if(coverageV < 1.0)
		{
			outV = mod(outV, 1);
		}
		outU = outU * repeatU / coverageU + offsetU;
		outV = outV * repeatV / coverageV + offsetV;	   
		if(mod(outV, 2) >= 1)
		{
			if(stagger != 0)
			{
				outU += 0.5;
			}
      
			if(mirrorV != 0)
			{
				axis = floor(outV) + 0.5;
				outV = axis - (outV - axis);
			}
		}
    
		if(mirrorU != 0)
		{
			if((outU % 2) >= 1)
			{
			axis = floor(outU) + 0.5;
			outU = axis - (outU - axis);
			}
		}
		if(rotateUV != 0.0)
		{
			P1 = set(0.5, 0.5, 0.0);
			vector R = set(0,0,degrees(rotateUV));
			outUVW = outUVW * invert(maketransform(0,0,{0,0,0},R,{1,1,1},P1));
			outU = outUVW.x;
			outV = outUVW.y;

			outU = mod(outU, repeatU);
			outV = mod(outV, repeatV);
		}
   
		outUVW.x = outU;
		outUVW.y = outV;
	}
}

#endif
