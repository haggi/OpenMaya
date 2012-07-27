/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef RAMP_H
#define RAMP_H

#include "utils.h"

void Ramp(
  		 T_DEFAULTATTR;
		 int rampType;
		 int interpolationType;
		 float uWave;
		 float vWave;
		 float noise;
		 float noiseFreq;
		 vector colorEntries[];
		 float colorPositions[];
		 vector outColor;
		 float outAlpha)
{
	float u = uvCoord[0];
	float v = uvCoord[1];
	vector rampResult;
	string ramp_basis_strings[];
	int numElements = arraylength(colorEntries);
	resize(ramp_basis_strings, numElements);
	int count;
	for( count = 0; count < numElements; count++)
	{
		ramp_basis_strings[count] = "linear";
	}
	// get coordinate from rampType
	// 0 = V ramp
	// 1 = U ramp
	float coord;
	if( rampType == 0)
		coord = uvCoord[1];
	if( rampType == 1)
		coord = uvCoord[0];
	if( rampType == 2)
		coord = (u + v) / 2;
	if( rampType == 3)
	{
		coord = atan(uvCoord[0] - 0.5, uvCoord[1] - 0.5);
		coord = coord / (2 * PI) - 0.5;
		coord = coord % 1;

		if(coord < 0)
		{
			coord += 1;
		}
	}
	if( rampType == 4)
	{
		vector half = set(0.5, 0.5, 0);
		coord = 2 * length(uvCoord - half) / 1.41421;
	}
	if( rampType == 5)
	{
		coord = max(2 * abs(u - 0.5), 2 * abs(v - 0.5));
	}
	if( rampType > 5)
	{
		if( rampType == 6)
		{
			coord = 2 * abs(u - 0.5);
			vector rampResultA = spline(ramp_basis_strings, spline("solvelinear", coord, colorPositions), colorEntries, "colorspace", "cspace:rgb");
			coord = 2 * abs(v - 0.5);
			vector rampResultB = spline(ramp_basis_strings, spline("solvelinear", coord, colorPositions), colorEntries, "colorspace", "cspace:rgb");
			rampResult = rampResultA * rampResultB;
		}
		if( rampType == 7)
		{
			// 4 corner ramp
			int numPositions = arraylength(colorEntries);
			
			if(numPositions < 1)
			{
				rampResult = set(0,0,0);
			}
			
			rampResult = (1 - u) * (1 - v) * colorPositions[0];
			
			if(numPositions > 1)
			{
				rampResult += u * (1 - v) * colorPositions[1];
			}
			if(numPositions > 2)
			{
				rampResult += (1 - u) * v * colorPositions[2];
			}
			if(numPositions > 3)
			{
				rampResult += u * v * colorPositions[3];
			}
		}
		if( rampType == 8)
		{
			// Tartan ramp
			coord = 2 * abs(u - 0.5);
			rampResultA = spline(ramp_basis_strings, spline("solvelinear", coord, colorPositions), colorEntries, "colorspace", "cspace:rgb");

			coord = 2 * abs(v - 0.5);
			rampResultB = spline(ramp_basis_strings, spline("solvelinear", coord, colorPositions), colorEntries, "colorspace", "cspace:rgb");
		
			rampResult = (rampResultA + rampResultB) / 2;
		}
	}else{
		rampResult = spline(ramp_basis_strings, spline("solvelinear", coord, colorPositions), colorEntries, "colorspace", "cspace:rgb");
	}
	
	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	
	outColor = rampResult;
	outAlpha = 1.0;
}

#endif