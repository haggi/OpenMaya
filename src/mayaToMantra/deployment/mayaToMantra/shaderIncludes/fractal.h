/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef FRACTAL_H
#define FRACTAL_H

#include "utils.h"
#include "noise_utils.h"

float get2DNoise(float x, y, px, py, inflection;)
{
	float noiseV;

	if( inflection == 0 ) 
		noiseV =  pnoise(x, y, (int)px, (int)py) - 0.5;
	else
		noiseV = abs(pnoise(x, y, (int)px, (int)py) - 0.5);
	return noiseV;
}

float get3DNoise(float x, y, z, px, py, pz, inflection;)
{
	float noiseV;  
	point p = set(x, y, z);
	point period = set(px, py, pz);

	if( inflection == 0 )
		noiseV = pnoise(p, period) - 0.5;
	else
		noiseV = abs(pnoise(p, period) - 0.5);

	return noiseV;
}

void Fractal(
	T_DEFAULTATTR;
	float amplitude;
	float threshold;
	float ratio;
	float frequencyRatio;
	float levelMin;
	float levelMax;
	float bias;
	float inflection;
	float animated;
	float time;
	float timeRatio;

	vector outColor;
	float outAlpha;
	)
{

	float ss = uvCoord[0];
	float tt = uvCoord[1];
	float du = Du(ss), dv = Dv(tt);

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);
    
		point pp = set( ss, tt, 0 ) * 15;

		float result = 0;
		float amp = amplitude*2; /* our noise is between -0.5 and 0.5 */
		float curFreq = 10.0; /* 10 = Maya fudge factor */

		float pixel_size = min(filterwidth(tt*curFreq), filterwidth(ss*curFreq));
		float nyquist = 2 * pixel_size;
		float pixel = 1.0;
	    
		if (animated == 0)
		{
		  time = 0.0;
		  timeRatio = 0.0;
		}
	    
		float curTimeFreq = 10.0;
    
		float noiseV = 0.0;
		float i=0.0;
		while( (i < levelMax && pixel > nyquist) || (i < levelMin) ) 
		{
			
			if( animated != 0 )
			{
				noiseV = get3DNoise(ss * curFreq, tt * curFreq, time * curTimeFreq,
													 curFreq, curFreq, curTimeFreq, inflection);
				
				curTimeFreq *= timeRatio;
			}
			else
			{
				noiseV = get2DNoise(ss * curFreq, tt * curFreq, curFreq, curFreq, inflection);
			}
			
			result += amp * noiseV;
			curFreq *= frequencyRatio;
			amp *= ratio;
			pixel /= frequencyRatio;      
			i+=1.0;
		}
		
		if( pixel>pixel_size && i <= levelMax)
		{
			noiseV = 0.0;
			if( animated != 0 )
			{
				noiseV = get3DNoise(ss * curFreq, tt * curFreq, time * curTimeFreq,
													 curFreq, curFreq, curTimeFreq, inflection);
			}
			else
			{
				noiseV = get2DNoise(ss * curFreq, tt * curFreq, curFreq, curFreq, inflection);
			}
			
			result += amp * noiseV;
		}

		/* center around 0.5 */
		if( inflection == 0.0 ) 
			result = (result*0.5 + 0.5);

		result += threshold;
		result = clamp(result, 0, 1);

		outColor = set(result, result, result);
		outAlpha = result;

		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);

	}
	else
	{
		outColor = defaultColor;
		outAlpha = 1.0;
	}
}
	
#endif

