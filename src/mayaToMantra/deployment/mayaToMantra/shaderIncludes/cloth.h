/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __cloth_h
#define __cloth_h

/*
begin inputs
	float alphaGain
	float alphaOffset
	float brightSpread
	color colorGain
	color colorOffset
	color defaultColor
	color gapColor
	float invert
	float randomness
	color uColor
	init={ss,tt} float2 uvCoord
	float uWave
	float uWidth
	color vColor
	float vWave
	float vWidth
	float widthSpread
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#define SQR(i) ( (i) * (i) )

#include "utils.h"

void
maya_cloth(
	// Inputs
	//
	float i_alphaGain;
	float i_alphaOffset;
	float i_brightSpread;
	color i_colorGain;
	color i_colorOffset;
	color i_defaultColor;
	color i_gapColor;
	uniform float i_invert;
	float i_randomness;
	color i_uColor;
	float i_uvCoord[2];
	float i_uWave;
	float i_uWidth;
	color i_vColor;
	float i_vWave;
	float i_vWidth;
	float i_widthSpread;
	
	// Outputs
	//
	output float o_outAlpha;
	output color o_outColor;
	)
{
	float ss = i_uvCoord[0];
	float tt = i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);

		if(i_randomness > 0)
		{
			point nn = noise(point(i_uvCoord[0], i_uvCoord[1], 0));

			ss += xcomp(nn) * i_randomness * 2;
			tt += ycomp(nn) * i_randomness * 2;
		}

		float prewaved_ss = ss;
		
		if(i_uWave > 0.0)
		{
			ss -= i_uWave * sin(tt * 2 * PI);
		}

		if(i_vWave > 0.0)
		{
			tt += i_vWave * sin(prewaved_ss * 2 * PI);
		}


		float uWidth = i_uWidth;
		float vWidth = i_vWidth;

		if(i_widthSpread > 0)
		{
			float spread = noise(0.5 * point(i_uvCoord[0], 0.5 * i_uvCoord[1], 1));
			uWidth -= i_widthSpread * spread;

			spread = noise(0.5 * point(0.5 * i_uvCoord[0], i_uvCoord[1], 2));
			vWidth -= i_widthSpread * spread;
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
			
			tmp = uWidth;
			uWidth = vWidth;
			vWidth = tmp;
			
			thread_color = 1.0;
		}
		
		ss = mod(2 * ss, 1.0);
		tt = mod(2 * tt, 1.0);
				
		float cloth = 0.0;  // 0 = gap color, 1 = thread color
		float in_gap = 0;   
		
		if (tt < vWidth)
		{
			float cs = ss - 0.5 * uWidth;
			float ct = 2.0 * tt / vWidth - 1.0;
			cloth = 0.75 * (SQR(cs) + SQR(ct));
		}
		else if (ss < uWidth)
		{
			float cs = 2.0 * ss / uWidth - 1.0;
			float ct = tt - 0.5 * vWidth - 1.0;
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
			
			if (i_brightSpread > 0)
			{
				float spread = noise(2 * (thread_color ? i_uvCoord[0] : i_uvCoord[1]));
				cloth = mix(cloth, cloth * spread, i_brightSpread);
			}
		}
		
		cloth = clamp(cloth, 0.0, 1.0);
		
		o_outColor = (1 - cloth) * i_gapColor + 
			cloth * (thread_color ? i_uColor : i_vColor);
		
		colorBalance(o_outColor, 
			o_outAlpha,
			1.0,
			i_alphaGain,
			i_alphaOffset,
			i_colorGain,
			i_colorOffset,
			i_invert);
	}
	else
	{
		o_outColor = i_defaultColor;
		o_outAlpha = 1.0;
	}
}

#endif /* __cloth_h */

