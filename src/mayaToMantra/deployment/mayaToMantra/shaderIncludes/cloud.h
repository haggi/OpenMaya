/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef CLOUD_H
#define CLOUD_H

#include "texture3dutils.h"

void Cloud(
		T_DEFAULTATTR3D;
		vector color1;
		vector color2;
		float contrast;
		float amplitude;
		float depthMin;
		float depthMax;
		vector ripples;
		int softEdges;
		float edgeThesh;
		float centerThres;
		float transpRange;
		float ratio;
		int wrap;
		int local;
		float blend;
		
		vector outColor;
		float outAlpha;
		   )
{

	float edgeDist;
	int outside;
	vector pp = transformPoint( blend, local, wrap, edgeDist, outside, placementMatrix);

	if(outside == 0)
	{
		float maxRipples = (abs(ripples.x) > abs(ripples.y)) ? abs(ripples.x) : abs(ripples.y);
		if (abs(ripples.z) > maxRipples) maxRipples = abs(ripples.z);

		// calculate filter
		vector pp1 = pp * maxRipples / 2.0;
		float pixel_size = sqrt( area(pp1) );
		float nyquist = 2 * pixel_size;
		float pixel = 1.0;

		//float iterate = -log(pixel_size) / 0.6931;
		//iterate = (iterate < depthMin) ? depthMin : iterate;
		//iterate = (iterate > depthMax) ? depthMax : iterate;
		//float pfloor  = floor (iterate);
		float pfloor = nyquist;

		float fx = ripples.x / 1.0;
		float fy = ripples.y / 1.0;
		float fz = ripples.z / 1.0;
		float ofx = 0.0;
		float ofy = 0.0;
		float ofz = 0.0;
		float loop = 0.0; 
		vector pos;
		float c = 0.0;
		for (loop = 0.0; loop < pfloor; loop++) 
		{
			ofx += 0.1;
			ofy += 0.03;
			ofz += 0.13;
			pos = set(pp.x * fx + ofx, pp.y * fy + ofy, pp.z * fz + ofz); 
			c += amplitude * noise(pos);
			fx *= 2.0;
			fy *= 2.0;
			fz *= 2.0;
			amplitude *= ratio;
		}

		//amplitude *= (nyquist - pfloor);
		ofx += 0.1;
		ofy += 0.03;
		ofz += 0.13;
		pos = set(pp.x * fx + ofx, pp.y * fy + ofy, pp.z * fz + ofz); 
		c += amplitude * noise(pos);
		float a = 1.0 - (1.0 - 0.5 * (1.0 + c)) * contrast;
		a = (a < 1.0) ? a : 1.0;
		a = (a > 0.0) ? a : 0.0;

		outColor = color1 + a * (color2 - color1);
		outColor = set(c,c,c);
		outAlpha = 1.0;

		colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
	}else{
		outColor = defaultColor;
		outAlpha = 0.0;
	}
}

///*
//begin inputs
//	float alphaGain
//	float alphaIsLuminance
//	float alphaOffset
//	float amplitude
//	float blend
//	float centerThresh
//	color color1
//	color color2
//	color colorGain
//	color colorOffset
//	float contrast
//	color defaultColor
//	uniform float2 depth
//	float edgeThresh
//	float invert
//	float local
//	normal normalCamera
//	matrix placementMatrix
//	float ratio
//	float3 ripples
//	float softEdges
//	float transpRange
//	float wrap
//end inputs
//
//begin outputs
//	float outAlpha
//	color outColor
//end outputs
//
//
//begin shader_extra_parameters Pref_param
//	varying point Pref = 0;
//end shader_extra_parameters
//*/
//
//#include "texture3d.h"
//#include "utils.h"
//#include "noise_utils.h"
//
//void
//maya_cloud(
//	// Inputs
//	//
//	float i_alphaGain;
//	uniform float i_alphaIsLuminance;
//	float i_alphaOffset;
//	uniform float i_amplitude;
//	float i_blend;
//	float i_centerThreshold;
//	color i_color1;
//	color i_color2;
//	color i_colorGain;
//	color i_colorOffset;
//	float i_contrast;
//	color i_defaultColor;
//	uniform float i_depth[2];
//	float i_edgeThreshold;
//	uniform float i_invert;
//	float i_local;
//	normal i_normalCamera;
//	matrix i_placementMatrix;
//	uniform float i_ratio;
//	float i_ripples[3];
//	float i_softEdges;
//	float i_transparentRange;
//	float i_wrap;
//
//	// Outputs
//	//
//	output float o_outAlpha;
//	output color o_outColor;)
//{
//	float edgeDist;
//	float outside;
//	varying point pp = transformP(i_blend, 
//		i_local, 
//		i_placementMatrix, 
//		i_wrap, edgeDist, 
//		outside);
//
//	if(outside < 1)
//	{
//		float mix = fBrownianNoise(pp, 0, i_amplitude, 1, 2, i_depth, i_ratio, i_ripples);
//		color c1 = i_color1;
//
//		o_outAlpha = mix;
//		c1 = mix(i_color2, c1, i_contrast);
//		o_outColor = mix(i_color2, c1, 1 - mix);
//
//		if(i_softEdges > 0)
//		{
//			o_outColor *= 1 - mix;
//		}
//		
//		colorBalance(o_outColor, 
//			o_outAlpha, 
//			i_alphaIsLuminance, 
//			i_alphaGain, 
//			i_alphaOffset, 
//			i_colorGain, 
//			i_colorOffset, 
//			i_invert);
//	}
//	else
//	{
//		o_outColor = i_defaultColor;
//		o_outAlpha = 0;
//	}
//}

#endif