/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef CLOTH_H
#define CLOTH_H

#include "utils.h"
#include "rman.h"

void Cloth(
    T_DEFAULTATTR;
	float brightSpread;
	vector gapColor;
	float randomness;
	vector uColor;
	float uWave;
	float uWidth;
	vector vColor;
	float vWave;
	float vWidth;
	float widthSpread;
	
	vector outColor;
	float outAlpha;
	)
{
	float ss = uvCoord.x;
	float tt = uvCoord.y;

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);

		if(randomness > 0)
		{
			vector nvec = set(uvCoord.x, uvCoord.y, 0);
			point nn = noise(nvec);

			ss += xcomp(nn) * randomness * 2;
			tt += ycomp(nn) * randomness * 2;
		}

		float prewaved_ss = ss;
		
		if(uWave > 0.0)
		{
			ss -= uWave * sin(tt * 2 * PI);
		}

		if(vWave > 0.0)
		{
			tt += vWave * sin(prewaved_ss * 2 * PI);
		}


		float uuWidth = uWidth;
		float vvWidth = vWidth;
		float spread;
		if(widthSpread > 0)
		{
			vector np = set(uvCoord.x, 0.5 * uvCoord.y, 1);
			np *= 0.5;
			spread = noise(np);
			uuWidth -= widthSpread * spread;
			
			np = set(uvCoord.x, 0.5 * uvCoord.y, 2);
			np *= 0.5;
			spread = noise(np);
			vvWidth -= widthSpread * spread;
		}
		
		ss = mod(ss, 1);
		tt = mod(tt, 1);

		float thread_color = 0; // 1 = uColor; 0 = vColor
		
		// Swap s-t info if we are in the 2nd or 3rd quadrant
		if ((ss >= 0.5 && tt < 0.5) || (ss < 0.5 && tt >= 0.5))
		{
			float tmp = ss;
			ss = tt;
			tt = tmp;
			
			tmp = uuWidth;
			uuWidth = vvWidth;
			vvWidth = tmp;
			
			thread_color = 1.0;
		}
		
		ss = mod(2 * ss, 1.0);
		tt = mod(2 * tt, 1.0);
				
		float cloth = 0.0;  // 0 = gap color, 1 = thread color
		float in_gap = 0;   
		
		float cs, ct;
		if (tt < vvWidth)
		{
			cs = ss - 0.5 * uuWidth;
			ct = 2.0 * tt / vvWidth - 1.0;
			cloth = 0.75 * (SQR(cs) + SQR(ct));
		}
		else if (ss < uuWidth)
		{
			cs = 2.0 * ss / uuWidth - 1.0;
			ct = tt - 0.5 * vvWidth - 1.0;
			cloth = 0.75 * (SQR(cs) + SQR(ct));
			thread_color = 1 - thread_color;
		}
		else
		{
			in_gap = 1.0;
		}
		
		if (in_gap < 1.0)
		{
			cloth = 1.0 - cloth;
			
			if (brightSpread > 0)
			{
				float tc = thread_color;
				if( tc > 0)
					tc = uvCoord.x;
				else
					tc = uvCoord.y;
				spread = noise(2.0 * tc);
				cloth = mix(cloth, cloth * spread, brightSpread);
			}
		}
		
		cloth = clamp(cloth, 0.0, 1.0);
		float ttc = thread_color;
		vector tcol = uColor;
		if( ttc == 0)
			tcol = vColor;

		outColor = (1 - cloth) * gapColor + cloth * tcol;
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	}else{
		outColor = defaultColor;
		outAlpha = 1.0;
	}
}

#endif
