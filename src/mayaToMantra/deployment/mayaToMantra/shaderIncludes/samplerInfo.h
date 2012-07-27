#ifndef SAMPLERINFO_H
#define SAMPLERINFO_H

#include "voplib.h"

void SamplerInfo(
				vector uv;
				vector pointWorld;
				vector pointObj;
				vector pointCamera;
				vector normalCamera;
				vector uvCoord;
				vector rayDirection;
				vector tangentUCamera;
				vector tangentVCamera;
				vector pixelCenter;
				float facingRatio;
				int flippedNormal;
				)
{
	vector pWorld = ptransform("space:world", P);
	vector pCam = ptransform("space:camera", P);
	vector pObj = ptransform("space:object", P);
	pointWorld = pWorld;
	// in maya we have a negative z axis
	pointCamera = set(pCam.x, pCam.y, -pCam.z);
	pointObj = pObj;

	vector nn = normalize(N);
	vector ii = normalize(I);
	vector nCam = vtransform("space:camera", nn);
	normalCamera = nCam;

    int bound_uv = isbound("uv");
    if( bound_uv ) 
	{
        uvCoord = set(uv.x, uv.y, 0.0);
    } else {
        uvCoord = set(s, t, 0);
    }	
	// this has switched sides, don't know why
	vector iCam = vtransform("space:camera", I);
	rayDirection = -normalize(iCam);
	
	vector utan = normalize(dPds);
	vector vtan = normalize(dPdt);
	utan = vtransform("space:camera", utan);
	vtan = vtransform("space:camera", vtan);
	tangentUCamera = normalize(utan);
	tangentUCamera.z = -tangentUCamera.z;
	tangentVCamera = normalize(vtan);
	tangentVCamera.z = -tangentVCamera.z;
	
	vector nf = vop_frontface(nn, ii);
	facingRatio = abs(dot(nf, ii));

	float fr = dot(nn, ii);
	flippedNormal = 0;
	if( fr < 0.0)
		flippedNormal = 1;


}


#endif