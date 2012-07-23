#ifndef LAMBERT_H
#define LAMBERT_H

#include <voptype.h>
#include <voplib.h>

void Lambert(vector uv; vector inColor; vector transparency; vector ambientColor; vector incandescence; float diffuseValue; float translucence; vector outColor; vector outTransparency; vector outMatteOpacity)
{
	outColor = inColor;
	
	vector      clr;
	vector      illum;
	vector		translucIllum = {0,0,0};
	bsdf        f;
	
	VOPvector ii = normalize(I);
	VOPnormal nf = normalize(N);
	if( translucence > 0.0)
	{
		VOPnormal neg = -nf;
		translucIllum = diffuse(neg) * translucence * Cf * inColor;
	}
	nf = vop_frontface(nf, ii);
	illum = diffuse(nf);
	clr = inColor * illum * diffuseValue + incandescence + translucIllum;
	#if defined(__vex)
	f = inColor * diffuseValue * bsdf(diffuse(nf));
	#endif
	
	outTransparency = transparency;
	outMatteOpacity = set(1,1,1);

	outColor = clr;
	F = f;
}

#endif
