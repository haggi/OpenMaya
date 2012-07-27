#ifndef NOISE_H
#define NOISE_H

#include "utils.h"
#include "noise_utils.h"
#include "rman.h"

void Noise(
	T_DEFAULTATTR;
	float threshold;
	float amplitude;
	float ratio;
	float frequencyRatio;
	float depthMax;
	float inflection;
	float time;
	float frequency;
	float implode;
	vector implodeCenter;
	float noiseType;
	float density;
	float spottyness;
	float sizeRand;
	float randomness;
	float falloff;
	float numWaves;
	
	color outColor;
	float outAlpha;
	)
{
	float ss = uvCoord.x;
	float tt = uvCoord.y;

	if(ISUVDEFINED(ss, tt))
	{
		vector pp = set(ss, tt, 0);

		if( implode != 0 )
		{
			vector dp = pp - implodeCenter;
			float l = length(dp);
			dp = dp / pow( l, implode );
			pp = dp + implodeCenter;
		}

		vector depth = set(0, depthMax,0);
		vector ripples = {2, 2, 2};
		vector pn = pp * frequency;

		if( noiseType == 0 || noiseType == 3 || noiseType == 4 ) 
		{
			if( noiseType == 3 )
			{
				/* wispy noise */
				pn += noise( pn / 2 ) * 1.3;
			}

			if( inflection > 0 )
			{
				outAlpha = amplitude * fTurbulence(pn, time, 1, frequencyRatio, depth, ratio, ripples);
			} else { 
				outAlpha = fBm(pn, time, amplitude, depth, 1, frequencyRatio, ratio);
			}
		}
		else if( noiseType == 1 )
		{
			/* "Billow" noise */
			float radius = sqrt( 0.5 * density );

			outAlpha = BillowNoise(	pn, time, 2, radius, sizeRand, randomness, falloff, spottyness, depthMax, frequencyRatio, ratio, amplitude );
		}
		else if( noiseType == 2 )
		{
			/* "Wave" noise */
			float x =  ss * frequency;
			float y =  tt * frequency;
			float timeRatio = sqrt(frequencyRatio);
			
			float waveValue = 0;
			
			float i;
			for(i = 0; i < depthMax; i += 1)
			{
				float noiseValue = cos_waves(x, y, time, numWaves);

				x *= frequencyRatio;
				y *= frequencyRatio;
				time *= timeRatio;
				
				if(inflection > 0)
				{
					noiseValue = abs(noiseValue);
				}
				
				waveValue += amplitude * noiseValue;
				amplitude *= ratio;
			}
			
			if(inflection <= 0)
			{
				waveValue = 0.5 * waveValue + 0.5;
			}
			
			outAlpha = waveValue;
		}
      
		outAlpha += threshold;
		outAlpha = clamp(outAlpha,0,1);    
		outColor = outAlpha;
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	}else{
		outColor = defaultColor;
		outAlpha = 0;
	}
}

#endif 

