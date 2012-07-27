#ifndef CHECK_H
#define CHECK_H

#include "utils.h"

void Checker(T_DEFAULTATTR;
			 vector color1; 
			 vector color2; 
			 float contrast; 
			 vector outColor;
			 float outAlpha)
{
	float ss = uvCoord[0];
	float tt = uvCoord[1];
	float du = Du(ss), dv = Dv(tt);

	if(ISUVDEFINED(ss, tt))
	{
		float count = 2;
		float filterWidth = filter;
		float result = uvCoord.x;
		float scaled = count * 0.5;
		float product = uvCoord.x * scaled;
		float added = product + 0.5;
		float delta = vop_FilterWidth(added) + filterOffset * 2.0;
		float amount1 = vop_PulseTrain(0.5, added, delta * filter);
		float negated = -amount1;
		float added1 = negated + 1;
		float result1 = uvCoord.y;
		float scaled1 = count * 0.5;
		float product1 = result1 * scaled1;
		float delta1 = vop_FilterWidth(product1) + filterOffset * 2.0;
		float amount2 = vop_PulseTrain(0.5, product1, delta1 * filter);
		float blend;
		#ifdef __vex
		  blend = lerp(amount1, added1, amount2);
		#else
		  blend = mix(amount1, added1, amount2);
		#endif

		outAlpha = blend;
		outColor = color1 + (color2 - color1) * blend;
		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	}else{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

#endif