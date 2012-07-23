#ifndef MOUNTAIN_H	
#define MOUNTAIN_H

#define CUBE_SIDE 32
#include "utils.h"

#define filterwidth(x)  max(abs(Du(x)*du) + abs(Dv(x)*dv), 1e-6)

float filteredSize ( vector filterSize;
					 float filter;
					 float filterOffset)
{
	float size = (filterSize.x > filterSize.y) ? filterSize.x : filterSize.y;
	if (filter < 0.0f) 
		filter = 0.0f;
	size = size * filter + filterOffset;
	if (size < 0.0) 
		size = -size;
	return (size);
}

void Mountain(
		T_DEFAULTATTR;
		vector snowColor;
		vector rockColor;
		float amplitude;
		float snowRoughness;
		float rockRoughness;
		float boundary;
		float snowAltitude;
		float snowDropoff;
		float snowSlope;
		float depthMax;

		vector outColor;
		float outAlpha;

			  )
{
	float ss = uvCoord.x;
	float tt = uvCoord.y;
	float du = Du(ss), dv = Dv(tt);


	if(ISUVDEFINED(ss, tt))
	{
		int	snowPresent;  	// the snow flag
		float      fx;             // Frequencies       
		float      ofx, ofy;       // Offsets
		float      ah, ch, amph;   // Elevation for the snow calculation
		float      c, amp;         // Cumulative values       
		int   	   loop;           // Recursion counter   
		int  	   pfloor;         // Recursion level               
		vector     slope;
		float      slopex, slopey; // Cumulative slope of Procedural Texture
		float      ratio;          // Ratio value for the terrain bump map
		float		size, iterate, maxRange;
		float		rockFractal, border, a, alpha;
		
		//size = filteredSize(filterSize, filter, filterOffset); 
		size = min(filterwidth(tt), filterwidth(ss));
		size = (size < 1.0e-8f) ? 1.0e-8f : size;
		size   *= CUBE_SIDE;
		size    = (size > 0.5f) ? 0.5f : size;
		iterate = -log(size) / 0.6931f;
		maxRange = depthMax;
		iterate = (iterate > maxRange) ? maxRange : iterate;
		pfloor  = iterate;

		//    Initialize the frequencies and values   
		fx = 1.0;
		ofx = 0.0;
		ofy = 0.0;
		slopex = 0.0;
		slopey = 0.0;
		ch = 0.0;

		amp = amplitude;
		amph = amp;
		rockFractal = rockRoughness;
		border 		= boundary;
		vector nv = 0;
		for (loop = 0; loop < pfloor; loop++) 
		{
			nv = set( (ss * fx + ofx), (tt * fx + ofy), 0);
			slope = noise(nv);
			ofx += 0.1f;
			ofy += 0.03f;
			slopex += slope.x * amp;
			slopey += slope.y * amp;
			fx *= 2.0f;
			amp *= border;		
			ch += noise(nv) * amph;
			amph *= rockFractal;
		}

		amp *= (iterate - (float) pfloor);
		nv = set( (ss * fx + ofx), (tt * fx + ofy), 0);
		slope = noise(nv);
		slopex += (slope.x * amp);
		slopey += (slope.y * amp);
		amph *= (iterate - pfloor);
		nv = set( (ss * fx + ofx), (tt * fx + ofy), 0);
		ch += noise(nv);
		c = slopex * slopex + slopey * slopey;
		a = 0.5f * c;
		ah = snowAltitude * snowDropoff;
		ah = (ah < 0.0f) ? 0.0f : ah;        
		if (a > (ah + snowSlope))
		{
			snowPresent = 0;
		}else {
			snowPresent = 1;
		}
		// compute output color and store it into datablock
		if (!snowPresent) 
		{
			outColor = rockColor;
		}else{
			outColor = snowColor;
		}
		// need to compute the alpha output
		//    Define the ratio to use for the terrain based upon whether
		//    there is any snow on the ground

		if (snowPresent) 
		{
			ratio = snowRoughness;
		}else{
			ratio = rockFractal;
		}
		//    Initialize the frequencies and values   
		ofx = 0.0f; 
		ofy = 0.0f;
		fx = 1.0f;
		c = 0.0f;
		for (loop = 0; loop < pfloor; loop++) 
		{
			nv = set( (ss * fx + ofx), (tt * fx + ofy), 0);
			c += amplitude * noise(nv);
			ofx += 0.1f;
			ofy += 0.03f;
			fx *= 2.0f;
			amplitude *= ratio;
		}
		amplitude *= (iterate - (float) pfloor);
		nv = set( (ss * fx + ofx), (tt * fx + ofy), 0);
		c += amplitude * noise(nv);

		alpha = 0.5f * (1.0f + c);

	}else{
		outAlpha = 0.0;
		outColor = defaultColor;
	}

	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);
}

#endif