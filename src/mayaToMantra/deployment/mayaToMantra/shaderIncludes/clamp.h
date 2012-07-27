#ifndef CLAMP_H
#define CLAMP_H


void Clamp(
	vector cmin;
	vector cmax;
	vector inputColor;
	vector output )
{
	output = clamp( inputColor, cmin, cmax );
}

#endif
