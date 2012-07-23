#ifndef RMAN_H 
#define RMAN_H

// some translation utilities for rsl shaders

#include <math.h>

#define volume	fog

#define color	vector
#define	normal	vector
#define point	vector

#define uniform
#define varying
#define output	export

#define Os			{1,1,1}
#define Cs			{1,1,1}

#define	faceforward		frontface
#define mix(aa,bb,cc)		lerp(aa,bb,cc)
#define step(min,value)		((value) > (min))
#define smoothstep(aa,bb,cc)	smooth(aa,bb,cc)
#define round(aa)		rint(aa)
#define spline			cspline
#define illuminate(xx,yy,zz)	
#define solar(xx,yy)		{ L = -Lz; }
#define xcomp(aa)		(aa.x)
#define ycomp(aa)		(aa.y)
#define zcomp(aa)		(aa.z)
#define setzcomp(v, c)   v.z = c
#define	cellnoise(aa)		random(aa)

#define dPdu			(dPds/Du(s))
#define dPdv			(dPdt/Dv(t))
#define calculatenormal		computenormal

float rman_mod(float x; float y)
{
	float rem = x % y;
    if(rem < 0.0) 
		rem += y;
    return rem;
}

#define mod(x, y) rman_mod(x, y)

float sign(float x)
{
    float s;
	if(x == 0.0)	
		s = 0.0;
    else if (x > 0)	
		s = 1.0;
    else
		s = -1.0;
    return s;
}

#endif
