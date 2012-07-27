/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef PLACE3DTEXTURE_H
#define PLACE3DTEXTURE_H


void Place3dTexture(
	matrix worldMatrix;
	matrix worldInverseMatrix;)
{
	worldInverseMatrix = invert(worldMatrix);
}

#endif

