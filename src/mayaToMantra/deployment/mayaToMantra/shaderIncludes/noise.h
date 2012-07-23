/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __noise_h
#define __noise_h

/*
begin inputs
	float threshold
	float amplitude
	float ratio
	float frequencyRatio
	float depthMax
	float inflection
	float time
	float frequency
	float implode
	float2 implodeCenter
	float noiseType
	float density
	float spottyness
	float sizeRand
	float randomness
	float falloff
	float numWaves
	color defaultColor
	color colorGain
	color colorOffset
	float alphaGain
	float alphaOffset
	float alphaIsLuminance
	float invert
	init={ss,tt} float2 uvCoord
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"
#include "noise_utils.h"

void
maya_noise(
	// Inputs
	float i_threshold;
	float i_amplitude;
	float i_ratio;
	float i_frequencyRatio;
	float i_depthMax;
	float i_inflection;
	uniform float i_time;
	float i_frequency;
	float i_implode;
	float i_implodeCenter[2];
	float i_noiseType;
	float i_density;
	float i_spottyness;
	float i_sizeRand;
	float i_randomness;
	float i_falloff;
	float i_numWaves;
	
	color i_defaultColor;
	color i_colorGain;
	color i_colorOffset;
	float i_alphaGain;
	float i_alphaOffset;
	uniform float i_alphaIsLuminance;
	uniform float i_invert;

	float i_uvCoord[2];

	// Outputs
	output float o_outAlpha;
	output color o_outColor;
	)
{
	float ss = i_uvCoord[0];
	float tt = i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		point pp = point(ss, tt, 0);

		/* Apply implosion. */
		if( i_implode != 0 )
		{
			point implodeCenter =
				point( i_implodeCenter[0], i_implodeCenter[1], 0 );

			vector dp = pp - implodeCenter;
			float l = length(dp);
			dp = dp / pow( l, i_implode );
			pp = dp + implodeCenter;
		}

		float depth[2] = {0, i_depthMax};
		uniform float ripples[3] = {2, 2, 2};
		point pn = pp * i_frequency;

		if( i_noiseType == 0 || i_noiseType == 3 || i_noiseType == 4 ) 
		{
			/* perlin, wispy and space time */

			if( i_noiseType == 3 )
			{
				/* wispy noise */
				pn += vector( noise( pn / 2 ) * 1.3 );
			}

			if( i_inflection > 0 )
			{
				o_outAlpha = i_amplitude * 
					fTurbulence(pn, i_time, 1, i_frequencyRatio, depth, i_ratio, ripples);
			} else { 
				o_outAlpha = 
					fBm(pn, i_time, i_amplitude, depth, 1, i_frequencyRatio, i_ratio);
			}
		}
		else if( i_noiseType == 1 )
		{
			/* "Billow" noise */
			float radius = sqrt( (0.5*0.5 + 0.5*0.5)*i_density );

			o_outAlpha = BillowNoise(
					pn, i_time, 2, radius, i_sizeRand, i_randomness, 
					i_falloff, i_spottyness, i_depthMax, 
					i_frequencyRatio, i_ratio, i_amplitude );
		}
		else if( i_noiseType == 2 )
		{
			/* "Wave" noise */
			float x =  ss * i_frequency;
			float y =  tt * i_frequency;
			float time = i_time;
			float timeRatio = sqrt(i_frequencyRatio);
			float amplitude = i_amplitude;
			
			float waveValue = 0;
			
			float i;
			for(i = 0; i < i_depthMax; i += 1)
			{
				float noiseValue = cos_waves(x, y, i_time, i_numWaves);

				x *= i_frequencyRatio;
				y *= i_frequencyRatio;
				time *= timeRatio;
				
				if(i_inflection > 0)
				{
					noiseValue = abs(noiseValue);
				}
				
				waveValue += amplitude * noiseValue;
				amplitude *= i_ratio;
			}
			
			if(i_inflection <= 0)
			{
				waveValue = 0.5 * waveValue + 0.5;
			}
			
			o_outAlpha = waveValue;
		}
      
		o_outAlpha += i_threshold;
		o_outAlpha = clamp(o_outAlpha,0,1);    
		o_outColor = o_outAlpha;

		colorBalance( o_outColor, 
			o_outAlpha, 
			i_alphaIsLuminance, 
			i_alphaGain, 
			i_alphaOffset, 
			i_colorGain, 
			i_colorOffset, 
			i_invert );
	}
	else
	{
		o_outColor = i_defaultColor;
		o_outAlpha = 0;
	}
}

#endif /* __noise_h */

