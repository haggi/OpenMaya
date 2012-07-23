#ifndef UTILS_H
#define UTILS_H
// utilities

#define LOG05 -0.693147180559945  /* log(0.5) */
#define UNDEFINED_UV -10000000

#define ISUVDEFINED(U, V) ( abs(U) < 10000 && abs(V) < 10000 )

#define WRAPMAX (1.000001)
#define EPSILON 1e-6

void cabs( vector in_color; vector out_color )
{
	out_color = set(abs(in_color.x), abs(in_color.y), abs(in_color.z));
}

float filteredpulse (float edge0, edge1, x, dx)
{
    float x0 = x - dx/2;
    float x1 = x0 + dx;
    return max (0, (min(x1,edge1)-max(x0,edge0)) / dx);
}

float integral (float t, nedge) { 
   return ((1-nedge)*floor(t) + max(0,t-floor(t)-nedge));
}

float filteredpulsetrain (float edge, period, x, dx)
{
	/* First, normalize so period == 1 and our domain of interest is > 0 */
    float w = dx/period;
    float x0 = x/period - w/2;
    float x1 = x0+w;
    float nedge = edge / period;

	/* Definite integral of normalized pulsetrain from 0 to t */

	float result;
	if( x0 == x1 )
	{
		/* Trap the unfiltered case so it doesn't return 0 (when dx << x). */
		result = (x0 - floor(x0) < nedge) ? 0 : 1;
	}
	else
	{
    	result = (integral(x1) - integral(x0)) / w;
	}

	return result;
}


float bias(float b, x)
{
    return pow(x, log(b)/LOG05);
}


void colorBalance(
	int i_alphaIsLuminance;
	float i_alphaGain;
	float i_alphaOffset;
	vector i_colorGain;
	vector i_colorOffset;
	int i_invert;
	vector io_outColor;
	float io_outAlpha;
	 )
{
	vector one = {1,1,1};
	if(i_invert != 0)
	{
		io_outColor = one - io_outColor;
		io_outAlpha = one - io_outAlpha;
	}

	if(i_alphaIsLuminance != 0)
	{
		io_outAlpha = luminance( io_outColor );
	}

	io_outColor *= i_colorGain;
	io_outColor += i_colorOffset;

	io_outAlpha *= i_alphaGain;
	io_outAlpha += i_alphaOffset;
}	

#endif