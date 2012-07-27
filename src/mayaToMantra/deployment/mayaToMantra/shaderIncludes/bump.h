#ifndef BUMP2D_H
#define BUMP2D_H

#define VOP_SHADING

#include <voptype.h>
#include <voplib.h>


// the node is called bump2d but the internal name ist kBump only
void Bump(
	float bumpValue;
	float bumpDepth;
	float bumpInterp;
	float bumpFilter;
	float bumpFilterOffset;
	vector normalCamera;
	vector outNormal)
{
	// it seems that in vex the bump is exactly the same as a displacement but in a surface context without 
	// modifying the P 
	vector displacedN;
	float sshear = 0;
	float tshear = 0;
	int for_poly = 1;
	int obj_space = 0;
	int bump_only = 1;
	vector outPoint;
	vop_displaceAlongNormal( P, normalize(normalCamera), bumpValue, bumpDepth, sshear, tshear, for_poly, obj_space, bump_only, outPoint, displacedN);
	outNormal = normalize(displacedN);
}

#endif
