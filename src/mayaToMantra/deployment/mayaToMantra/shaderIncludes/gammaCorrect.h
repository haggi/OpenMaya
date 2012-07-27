#ifndef GAMMA_H
#define GAMMA_H

void CammaCorrect(
	vector value;
	vector gamma;
	vector value;)
{
	float r =  pow(value.x, 1.0 / gamma.x);
	float g =  pow(value.y, 1.0 / gamma.y);
	float b =  pow(value.y, 1.0 / gamma.y);
	value = set(r,g,b);
}

#endif