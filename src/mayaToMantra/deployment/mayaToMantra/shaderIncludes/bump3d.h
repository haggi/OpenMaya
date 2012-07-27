/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef BUMP3D_H
#define BUMP3D_H

#include "utils.h"

void Bump3d(
	float bumpValue;
	float bumpDepth;
	float bumpFilter;
	float bumpFilterOffset;
	vector normalCamera;
	vector outNormal;)
{
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

