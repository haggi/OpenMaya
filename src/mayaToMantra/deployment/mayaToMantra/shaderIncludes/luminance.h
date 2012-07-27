#ifndef LUMINANCE_H
#define LUMINANCE_H

void Luminance(
	vector value;
	float outValue;)
{
	outValue = 0.3 * value.x + 0.59 * value.y + 0.11 * value.z;
}

#endif


