/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __bulge_h
#define __bulge_h

/*
begin inputs
	float alphaGain
	float alphaIsLuminance
	float alphaOffset
	color colorGain
	color colorOffset
	color defaultColor
	float invert
	float2 uvCoord
	float2 uvFilterSize
	float uWidth
	float vWidth
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"

void
maya_bulge(
	// Inputs
	//
	float i_alphaGain;
	uniform float i_alphaIsLuminance;
	float i_alphaOffset;
	color i_colorGain;
	color i_colorOffset;
	color i_defaultColor;
	uniform float i_invert;
	float i_uvCoord[2];
	float i_uvFilterSize[2];
	float i_uWidth;
	float i_vWidth;
	
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
    
		float a = smoothstep(i_uWidth / 2, .5, ss) * 
		(1 - smoothstep(0.5, 1 - (i_uWidth / 2), ss));
		float b = smoothstep(i_vWidth / 2, .5, tt) * 
		(1 - smoothstep(0.5, 1 - (i_vWidth / 2), tt));
		o_outAlpha = sqrt(abs(a * b));
		o_outColor = color(o_outAlpha);

		colorBalance(o_outColor, 
			o_outAlpha,
			i_alphaIsLuminance,
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
   
#endif /* __bulge_h */

