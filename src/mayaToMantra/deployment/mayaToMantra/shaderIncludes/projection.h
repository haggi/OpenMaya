/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef PROJECTION_H
#define PROJECTION_H

/*
	Okay, now we have a problem:
	
	Normally if we have some 2d texture, it may be a procedural or a filetexture, we first calculate the
	uv coordinates based on the uvs of the geometry. Then the texture is read and deformed by these uvs.

	The projection node is connected to the texture from the wrong side like this:
	place2dTexture --> fileTexture/Procedural --> projection --> shader
	The task of the projection node is to modify the uv's or s, t based on a special projection type.
	But the image is already read in before based on other uvs. In maya or mentalray thats no problem at all
	because there the uvs are defined and then the input of a node is calculated based on this new uvs.
	This does not work yet. The projection node has to modify the uv's before any place2dtexture node takes place.

	So we split this node into two, first the one which does the uvs that must be connected to any 2d/3d placement node
	that is defined before. The other part of the shader is a regular one wich modifies the color etc.

	This procedure requires that the parser knows about this problem and places the projection node at the right positions.

	With my current implementation there is at least one limitation:
		- because the projection placement is placed before any incoming connected placeTex2D node, it will not be possible to
		  use the same network as input for the image attribute of a projection node and as normal uv based input to any other
		  node at the same time. But this is done rarely I hope...
*/

#include "texture3dutils.h"
#include "utils.h"

void Projection(
				matrix placementMatrix;
				int projType;
				vector image;
				vector transparency;
				float uAngle;
				float vAngle;
				string linkedCamera;
				int fitType;
				int fitFill;
				float amplitudeX;
				float amplitudeY;
				float ratio;
				vector ripples;
				float depthMin;
				float depthMax;
				vector defaultTransparency;
				vector transparencyGain;
				vector transparencyOffset;
				vector defaultColor;
				vector colorGain;
				vector colorOffset;
				float alphaGain;
				float alphaOffset;
				float filter;
				float filterOffset;
				int invert;
				int local;
				int wrap;
				float blend;

				vector outUVCoord;
				vector outColor;
				float outAlpha;
			   )
{
	// I use the ratio to recognize if I have to calculate uv projections or if I have to do some color calculation
	float edgeDist;
	int outside;
	float u = 0, v = 0;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);
	
	if(outside == 0)
	{
		// planar projection
		if( projType == 1)
		{
			u = pp.x / 2.0 + 0.5;
			v = pp.y / 2.0 + 0.5;
		}
	}else{
		u = v = UNDEFINED_UV;
	}

	outUVCoord = set(u,v,-1000); // the -1000 will be used in the place2dtexture to recognize if it is connected or not
	if( ratio < 0.0)
	{
		return;
	}

	if( outside == 1)
	{
		outColor = defaultColor;
		outAlpha = 0.0;
		return;
	}
	vector inColor = image;
		
	outColor = inColor;
	outAlpha = 1.0;
	int alphaIsLuminance = 0;
	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
}

#endif