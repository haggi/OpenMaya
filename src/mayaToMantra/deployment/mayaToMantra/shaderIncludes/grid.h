/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __grid_h
#define __grid_h

/*
begin inputs
	color defaultColor
	float alphaGain
	float alphaIsLuminance
	float alphaOffset
	color colorGain
	color colorOffset
	float invert
	color lineColor
	color fillerColor
	float uWidth
	float vWidth
	float contrast
	float filter
	float filterOffset
	init={ss,tt} float2 uvCoord
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"

#define CONTRAST(c,d,f) mix((c + d) / 2, c, f)

void
maya_grid(
	// Inputs
	//
	color i_defaultColor;
	float i_alphaGain;
	uniform float i_alphaIsLuminance;
	float i_alphaOffset;
	color i_colorGain;
	color i_colorOffset;
	uniform float i_invert;

	color i_lineColor;
	color i_fillerColor;
	float i_uWidth, i_vWidth;
	float i_contrast;

	float i_filter, i_filterOffset;

	float i_uvCoord[2];
	
	// Outputs
	//
	output float o_outAlpha;
	output color o_outColor;
	)
{
	color contrast(color c,d; float f) 
	{
		return mix((c + d) / 2, c, f);
	}

	extern float du, dv;
	
	float ss = i_uvCoord[0];
	float tt = 1 - i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);
    
		/* compute 'ss' and 'tt' filter widths */
		float dss = abs(Du(ss) * du) + abs(Dv(ss) * dv);
		float dtt = abs(Du(tt) * du) + abs(Dv(tt) * dv);

		/* Clamp the 'ss' and 'tt' filter widths to reasonable ranges
           (there could be discontinuities in parametrisation) and 
		   add in "Effects" filter values. We multiplie the i_filterOffset
		   variable by 2 to match Maya's look */ 
		dss = clamp( dss, 0, i_uWidth*0.5 ) * i_filter + i_filterOffset*2; 
		dtt = clamp( dtt, 0, i_vWidth*0.5 ) * i_filter + i_filterOffset*2; 
	
		float f = filteredpulsetrain (i_uWidth, 1, ss+i_uWidth*0.5, dss)
				* filteredpulsetrain (i_vWidth, 1, tt+i_vWidth*0.5, dtt);

		color lc = contrast(i_lineColor, i_fillerColor, i_contrast);
		color fc = contrast(i_fillerColor, i_lineColor, i_contrast);

		o_outColor = mix(lc, fc,f);
		o_outAlpha = 1-f; 
		
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

#endif /* __grid_h */

