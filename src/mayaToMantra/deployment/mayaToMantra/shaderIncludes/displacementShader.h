#ifndef DISPLACE_H
#define DISPLACE_H

#include <voptype.h>
#include <voplib.h>

#define VOP_DISPLACE

void DisplacementShader(vector uv; 
						float inDisplacement; 
						float scale; 
						vector vectorDisplacement; 
						int vectorSpace; 
						vector tangent; 
						float outDisplacement;
						vector outPoint;
						vector outNormal)
{

	// at the moment use surface normal, no vector displacement yet
	vector tstN;
	vop_displaceAlongNormal( P, normalize(N), inDisplacement, scale, 0, 0, 1, 0, 0, outPoint, tstN);
	// maybe do a computenormal(outPoint); this seems to be done automatically if #define VOP_DISPLACE is defined
	tstN = normalize(tstN);
	P = outPoint;
	N = tstN;
}

#endif