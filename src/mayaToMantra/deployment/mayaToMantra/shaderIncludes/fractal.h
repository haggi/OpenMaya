/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __fractal_h
#define __fractal_h

/*
begin inputs
	float alphaGain
	float alphaIsLuminance
	float alphaOffset
	float amplitude
	float bias
	color colorGain
	color colorOffset
	color defaultColor
	float frequencyRatio
	float invert
	float levelMin
	float levelMax
	float ratio
	float threshold
	float inflection
  float animated
  float time
  float timeRatio
	float2 uvCoord
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"
#include "noise_utils.h"

float
get2DNoise(float x, y, px, py, inflection;)
{
	float noise;

	if( inflection == 0 ) 
		noise =  pnoise(x, y, px, py) - 0.5;
	else
		noise = abs(pnoise(x, y, px, py) - 0.5);

	return noise;
}

float
get3DNoise(float x, y, z, px, py, pz, inflection;)
{
	float noise;  
	point p = point(x, y, z);
	uniform point period = point(px, py, pz);

	if( inflection == 0 )
		noise = pnoise(p, period) - 0.5;
	else
		noise = abs(pnoise(p, period) - 0.5);

	return noise;
}

void
maya_fractal(
	// Inputs
	//
	float i_alphaGain;
	uniform float i_alphaIsLuminance;
	float i_alphaOffset;
	float i_amplitude;
	float i_bias;
	color i_colorGain;
	color i_colorOffset;
	color i_defaultColor;
	uniform float i_frequencyRatio;
	uniform float i_invert;
	uniform float i_levelMin;
	uniform float i_levelMax;
	uniform float i_ratio;
	uniform float i_threshold;
	uniform float i_inflection;
  uniform float i_animated;
  uniform float i_time;
  uniform float i_timeRatio;
	float i_uvCoord[2];
	
	// Outputs
	//
	output float o_outAlpha;
	output color o_outColor;
	)
{
	extern float du, dv;

	float ss = i_uvCoord[0];
	float tt = i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);
    
		point pp = point( ss, tt, 0 ) * 15;

		float result = 0;
		float amp = i_amplitude*2; /* our noise is between -0.5 and 0.5 */
		uniform float curFreq = 10.0; /* 10 = Maya fudge factor */

		float pixel_size = min(filterwidth(tt*curFreq), filterwidth(ss*curFreq));
		float nyquist = 2 * pixel_size;
		float pixel = 1.0;

    float time = 0;
    float timeRatio = 0;
    
    if (i_animated != 0)
    {
      time = i_time;
      timeRatio = i_timeRatio;
    }
    
    uniform float curTimeFreq = 10.0;
    
		uniform float i=0;
		while( (i<i_levelMax && pixel>nyquist) || (i<i_levelMin) ) 
		{
			float noise;
			
			if( i_animated != 0 )
			{
				noise = get3DNoise(ss * curFreq, tt * curFreq, time * curTimeFreq,
													 curFreq, curFreq, curTimeFreq, i_inflection);
				
				curTimeFreq *= i_timeRatio;
			}
			else
			{
				noise = get2DNoise(ss * curFreq, tt * curFreq, curFreq, curFreq, i_inflection);
			}
			
			result += amp * noise;
			curFreq *= i_frequencyRatio;
			amp *= i_ratio;
			pixel /= i_frequencyRatio;      
			i+=1.0;
		}

		if( pixel>pixel_size && i<=i_levelMax)
		{
			float noise;
			if( i_animated != 0 )
			{
				noise = get3DNoise(ss * curFreq, tt * curFreq, time * curTimeFreq,
													 curFreq, curFreq, curTimeFreq, i_inflection);
			}
			else
			{
				noise = get2DNoise(ss * curFreq, tt * curFreq, curFreq, curFreq, i_inflection);
			}
			
			result += amp * noise;
		}

		/* center around 0.5 */
		if( i_inflection == 0 )
			result = (result*0.5 + 0.5);

		result += i_threshold;
		result = clamp(result, 0, 1);

		o_outColor = color(result, result, result);
		o_outAlpha = result;

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
	
#endif /* __fractal_h */

