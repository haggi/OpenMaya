#ifndef CONTRAST_H
#define CONTRAST_H

#include "utils.h"

float contr(
	float value;
	float contrast;
	float bias)
{
	float val = pow(value + EPSILON, log(0.5) / log(bias));
	float outValue;
	
	if (value < 0.5)
	{
		outValue = 0.5 * pow(2 * value, contrast);
	}else{
		outValue = 1.0 - 0.5 * pow(2 * (1 - value), contrast);
	}

	return outValue;
}

void Contrast(
	vector value;
	vector contrast;
	vector bias;
	vector outputValue;)
{
	float r = contr(value.x, contrast.x, bias.x);
	float g = contr(value.y, contrast.y, bias.y);
	float b = contr(value.z, contrast.z, bias.z);

	outputValue = set(r, g, b);
}

#endif
