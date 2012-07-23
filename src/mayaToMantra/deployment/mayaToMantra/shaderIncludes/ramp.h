/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __ramp_h
#define __ramp_h

/*
begin inputs
	float alphaGain
	float alphaOffset
	float[] colorEntryList[].position
	color[] colorEntryList[].color
	color colorGain
	color colorOffset
	color defaultColor
	float hueNoise
	float hueNoiseFreq
	float interpolation
	float invert
	float noise
	float noiseFreq
	float satNoise
	float satNoiseFreq
	float type
	float uWave
	init={ss,tt} float2 uvCoord
	float vWave
	float valNoise
	float valNoiseFreq
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"
#include "noise_utils.h"
#include "ramp_utils.h"

void
maya_ramp(
	// Inputs
	//
	float i_alphaGain;
	float i_alphaOffset;
	float i_colorPositions[];
	color i_colors[];
	color i_colorGain;
	color i_colorOffset;
	color i_defaultColor;
	float i_hueNoise;
	float i_hueNoiseFreq;
	uniform float i_interpolation;
	uniform float i_invert;
	float i_noise;
	float i_noiseFreq;
	float i_satNoise;
	float i_satNoiseFreq;
	uniform float i_type;
	float i_uWave;
	float i_uvCoord[2];
	float i_vWave;
	float i_valNoise;
	float i_valNoiseFreq;
	
	// Outputs
	//
	output float o_outAlpha;
	output color o_outColor;)
{
	varying float ss = i_uvCoord[0];
	varying float tt = i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		float ns = 0;

		if(i_noise > 0)
		{
			ns = i_noise * snoisexy(ss * i_noiseFreq * 18,	tt * i_noiseFreq * 18);
		}

		ss = mod(ss + ns, WRAPMAX);
		tt = mod(tt + ns, WRAPMAX);
    
		float preWaved_tt = tt;
		
		if(i_vWave > 0)
		{
			tt += i_vWave * sin(ss * 2 * PI);
		}
		
		if(i_uWave > 0)
		{
			ss -= i_uWave * sin(tt * 2 * PI);
		}
				
		
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);

		float index = 0;
		uniform float interp[] = { i_interpolation };
		
		if(i_type == 0)
		{
			// v ramp
			index = tt;
			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 1)
		{
			// u ramp
			index = ss;
			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 2)
		{
			// diagonal
			index = (ss + tt) / 2;
			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 3)
		{
			// radial
			index = atan(ss - 0.5, tt - 0.5);
			index = index / (2 * PI) - 0.5;
			index = mod(index, 1);

			if(index < 0)
			{
				index += 1;
			}

			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 4)
		{
			// circular
			index = 2 * length(vector(ss, tt, 0) - vector(0.5, 0.5, 0)) / 1.41421;
			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 5)
		{
			// box
			index = max(2 * abs(ss - 0.5), 2 * abs(tt - 0.5));
			o_outColor = colorRamp(index, i_colorPositions, interp, i_colors);	
		}
		else if(i_type == 6)
		{
			// UV ramp
			index = 2 * abs(ss - 0.5);
			color s_col = colorRamp(index, i_colorPositions, interp, i_colors);

			index = 2 * abs(tt - 0.5);
			color t_col = colorRamp(index, i_colorPositions, interp, i_colors);
			
			o_outColor = s_col * t_col;
		}
		else if(i_type == 7)
		{
			// 4 corner ramp
			float numPositions = arraylength(i_colorPositions);
			
			if(numPositions < 1)
			{
				o_outColor = 0;
			}
			
			o_outColor = (1 - ss) * (1 - tt) * i_colors[0];
			
			if(numPositions > 1)
			{
				o_outColor += ss * (1 - tt) * i_colors[1];
			}
			if(numPositions > 2)
			{
				o_outColor += (1 - ss) * tt * i_colors[2];
			}
			if(numPositions > 3)
			{
				o_outColor += ss * tt * i_colors[3];
			}
		}
		else
		{
			// Tartan ramp
			index = 2 * abs(ss - 0.5);
			color s_col = colorRamp(index, i_colorPositions, interp, i_colors);

			index = 2 * abs(tt - 0.5);
			color t_col = colorRamp(index, i_colorPositions, interp, i_colors);
		
			o_outColor = (s_col + t_col) / 2;
		}

		if(i_hueNoise > 0 || i_satNoise > 0 || i_valNoise > 0)
		{
			color hsv = ctransform("rgb", "hsv", o_outColor);
			float hue = comp(hsv, 0);
			float sat = comp(hsv, 1);
			float val = comp(hsv, 2);
		
			if(i_hueNoise > 0)
			{
				// unclamped hue values seem closer to Maya
				hue += 1.25 * i_hueNoise * snoisexy(ss * i_hueNoiseFreq * 18,	
					tt * i_hueNoiseFreq * 18);				
			}
			
			if(i_satNoise > 0)
			{
				sat += 1.25 * i_satNoise * snoisexy(ss * i_satNoiseFreq * 18,	
					tt * i_satNoiseFreq * 18);
					
				sat = clamp(sat, 0, 1);
			}
			
			if(i_valNoise > 0)
			{
				val += 1.25 * i_valNoise * snoisexy(ss * i_valNoiseFreq * 18,
					tt * i_valNoiseFreq * 18);
				
				val = clamp(val, 0, 1);
			}

			o_outColor = ctransform("hsv", "rgb", color(hue, sat, val));
		}

		o_outAlpha = 1.0;
	
		// It seems that the alpha channel is always computed from the luminance of
		// color by Maya for this shader
		colorBalance(o_outColor, 
			o_outAlpha,
			1,
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

#endif /* __ramp_h */

