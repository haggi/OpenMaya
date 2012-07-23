#ifndef CHECK_H
#define CHECK_H

#include "utils.h"

void Checker(vector uv; 
			 vector defaultColor; 
			 vector colorGain; 
			 vector colorOffset; 
			 float alphaGain; 
			 float alphaOffset; 
			 int alphaIsLuminance; 
			 int invert; 
			 vector uvCoord;
			 vector color1; 
			 vector color2; 
			 float contrast; 
			 vector outColor;
			 float outAlpha)
{
	float count = 2;
	float sTile = floor(uvCoord.x * count);
	float tTile = floor(uvCoord.y * count);
	float res = (sTile+tTile) % 2;

	if( contrast != 1.0)
	{
		vector cc = abs(color1 - color2);
		vector ma = max(color1, color2);
		vector mi = min(color1, color2);
		if( ma == color1)
		{
			color1 -= cc * .5;
			color2 += cc * .5;
		}else{
			color2 -= cc * .5;
			color1 += cc * .5;
		}		
	}

	if( res == 0)
		outColor = color1;
	else
		outColor = color2;
	outAlpha = luminance(outColor);
	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);

}

#endif