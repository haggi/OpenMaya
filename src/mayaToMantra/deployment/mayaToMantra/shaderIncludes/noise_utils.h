/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.

	Some modifications to adapt to mayaToMantray by Haggi Krey	
*/

#ifndef NOISE_UTILS_H
#define NOISE_UTILS_H

#include "utils.h"
#include "rman.h"

//#define snoise(p) (2 * (float noise(p)) - 1)
#define psnoise(x, y, xperiod, yperiod) (2.0*(pnoise(x, y, xperiod, yperiod)-1.0))
//#define snoisexy(x,y) (2.0 * ((float)noise(x,y)) - 1.0)
#define vsnoise(p) (2.0 * (noise(p)) - 1.0)
#define filteredsnoise(p,width) (snoise(p) * (1-smoothstep (0.2,0.75,width)))
//#define filterwidth(x)  max(abs(Du(x)*du) + abs(Dv(x)*dv), 1e-6)

float filterwidth( float x)
{
	float du = Du(s);
	float dv = Du(t);
	float a = abs(Du(x)*du);
	float b = abs(Dv(x)*dv);
	float c = a + b;
	float result = max(c, 1e-6);
	return result;
}

float snoisexy( float x; float y)
{
	float nv = noise(x,y);
	return (2.0 * (nv - 1.0)); 
}

float snoisepy( vector x; float y)
{
	vector4 xyzt = set(x.x, x.y, x.z, y);
	float nv = noise(xyzt);
	return (2.0 * (nv - 0.5)); 
}

float fBrownianNoise(
	vector i_point;
	float i_time;
	float i_amplitude;
	float i_lacunarity;
	float i_frequencyRatio;
	vector i_octaves;
	float i_ratio;
	vector i_ripples; )
{
	float i = 0.0;
	float lacunarity = i_lacunarity;
	float mixv = 0.0;
	float amp = i_amplitude;
	point pp = i_point * i_ripples / 2.0;

	float pixel_size = sqrt( area(pp) );
	float nyquist = 2 * pixel_size;
	float pixel = 1.0;

	while( (i < i_octaves.y && pixel>nyquist) || i < i_octaves.x )
	{
		mixv += amp * snoisepy((pp + noise(lacunarity)) * lacunarity, i_time);
		lacunarity *= i_frequencyRatio;
		amp *= i_ratio;
		pixel /= i_frequencyRatio;

		i+= 1.0;
	}
	//return mix;

	if( pixel>pixel_size && i<=i_octaves[1])
	{
		float weight = clamp(pixel/pixel_size - 1, 0, 1);
		mixv += weight * amp *
			snoisepy((pp + noise(lacunarity)) * lacunarity, i_time);
	}

	return mixv * 0.5 + 0.5;
}

float fBm( 
	point p; 
	float i_time;
	float i_initialAmplitude;
	vector i_octaves;
	float i_lacunarity, frequencyRatio, ratio)
{
	float amp = i_initialAmplitude;
	point pp = p;
	float sum = 0;
	float i = 0;

	float lacunarity = i_lacunarity;

	float pixel_size = sqrt( area(pp) );
	float nyquist = 2 * pixel_size;
	float pixel = 1.0;

	while( (i<i_octaves[1] && pixel>nyquist) || (i<i_octaves[0]) )
	{
		sum += amp * snoisepy( pp * lacunarity, i_time );
		amp *= ratio; 
		lacunarity *= frequencyRatio;
		pixel /= frequencyRatio;	

		i += 1.0;
	}

	if( pixel>pixel_size && i<=i_octaves[1] )
	{
		float weight = clamp(pixel/pixel_size - 1, 0, 1);
		sum += weight * amp * snoisepy(pp * lacunarity, i_time );
	}

	return sum * 0.5 + 0.5;
}

/* A vector-valued fBm. */
vector vfBm( point p; vector octaves; float lacunarity; float ratio; vector i_ripples )
{
	vector sum = 0;
	float i = 0;
	float amp = 1;

	point pp = p * i_ripples / 2.0;

	float pixel_size = sqrt( area(pp) );
	float nyquist = 2 * pixel_size;
	float pixel = 1.0;

	while( (i<octaves[1] && pixel>pixel_size) || i<octaves[0] )
	{
		sum += amp * vsnoise( pp );
		amp *= ratio;
		pp *= lacunarity;

		i += 1.0;
		pixel /= lacunarity;
	}

	if( pixel>pixel_size && i<=octaves[1])
	{
		float weight = clamp(pixel/pixel_size - 1, 0, 1);
		sum += weight * amp * vsnoise(pp);
	}

	return sum;
}


float
fTurbulence(
	vector i_point;
	float i_time;
	float i_lacunarity;
	float i_frequencyRatio;
	vector i_octaves;
	float i_ratio;
	vector i_ripples;)
{
	float i = 0.0;
	float lacunarity = i_lacunarity;
	float mixv = 0.0;
	float amp = 1.0;
	point pp = i_point * i_ripples / 2.0;

	float pixel_size = sqrt( area(pp) );
	float nyquist = 2 * pixel_size;
	float pixel = 1.0;

	float noiseVal = noise(lacunarity);
	while( (i<i_octaves[1] && pixel>nyquist) || i<i_octaves[0] )
	{
		vector transP = (pp + noiseVal) * lacunarity;
		mixv += amp * abs(snoisepy(transP, i_time));
		lacunarity *= i_frequencyRatio;
		amp *= i_ratio;
		pixel /= i_frequencyRatio;

		i+=1.0;
	}

	if( pixel>pixel_size && i<=i_octaves[1] )
	{
		float weight = clamp(pixel/pixel_size - 1, 0, 1);
		mixv += weight * amp * abs(snoisepy((pp + noiseVal) * lacunarity, i_time));
	}

	return mixv;
}

/*
	animatedcellnoise

	This is a mix of cell noise and regular noise. It is cell noise by its
	point parameter but smooth noise by its float parameter. So you can think
	of it as a 3D cell noise which changes smoothly in the 4th dimension.
*/
point animatedcellnoise( point i_p; float i_t )
{
	/* This is so each cell changes direction at a different time. */
	float t = cellnoise(i_p) + i_t;

	float tbase = floor(t);
	vector4 pp = set(i_p.x, i_p.y, i_p.z, tbase);
	point n1 = cellnoise( pp );
	float d = t - tbase;
	pp = set(i_p.x, i_p.y, i_p.z, tbase + 1);
	point n2 = cellnoise( pp );
	n1 += d * (n2 - n1);

	return n1;
}

/*
	SuspendedParticles

	simulates a medium filled with particles of a given radius.

	RETURNS
	total number of particles enclosing "Pn".
	
	NOTES
	- if particle radius is "infinit", this will give a "worley" noise.
	- the 'octave' parameter is needed to make jitter different for all octaves
*/
float
SuspendedParticles(
	point Pn; float i_time;
	float particleradius;
	float jitter;
	int octave;
	float f1;  
	point pos1;
	float f2;  
	point pos2;
	point particlesPos[];
    )
{
	point thiscell = set( floor(xcomp(Pn))+0.5, floor(ycomp(Pn))+0.5,floor(zcomp(Pn))+0.5);

	f1 = f2 = 1e36;
	float i, j, k;

	int curr_particle = 0;
	for (i = -1;  i <= 1;  i += 1)
	{
		for (j = -1;  j <= 1;  j += 1)
		{
			for (k = -1;  k <= 1;  k += 1)
			{
				point testcell = thiscell + set(i,j,k);
				if( jitter > 0 )
				{
					vector jit = animatedcellnoise(	testcell, i_time + 1000 * octave) - 0.5;
					testcell += jitter * jit;
				}
				float dist = distance( testcell, Pn );
				if( dist < particleradius )
				{
					particlesPos[curr_particle] = testcell;
					curr_particle += 1;
					if (dist < f1)
					{
						f2 = f1;  pos2 = pos1;
						f1 = dist;  pos1 = testcell;
					}
					else if (dist < f2)
					{
						f2 = dist;  pos2 = testcell;
					}
				}
			}
		}
	}
	
	return curr_particle;
}

float
SuspendedParticles2d(
	point Pn; float i_time;
	float particleradius;
	float jitter;
	int octave;
	point particlesPos[];
    )
{
	point thiscell = set( floor(xcomp(Pn))+0.5, floor(ycomp(Pn))+0.5, 0.0 );

	float i, j, k;

	int curr_particle = 0;
	for (i = -1;  i <= 1;  i += 1)
	{
		for (j = -1;  j <= 1;  j += 1)
		{
			point testcell = thiscell + set(i,j,0);
			if( jitter > 0 )
			{
				vector vjit = animatedcellnoise(testcell, i_time + 1000 * octave) - 0.5;
				vjit *= jitter;
				setzcomp( vjit, 0 );
				testcell += vjit;
			}
			float dist = distance( testcell, Pn );
			if( dist < particleradius )
			{
				particlesPos[curr_particle] = testcell;
				curr_particle += 1;
			}
		}
	}
	
	return curr_particle;
}

/*
	ParticleDensity

	computes particles density at some given point in 
	the particle	
*/
float
ParticleDensity(
	float i_falloff;
	point i_particleCenter, i_P;
	float i_radius; )
{
	float distanceToCenter = distance( i_particleCenter, i_P );
	float fadeout = 0;

	if( i_falloff == 0 ) /* linear */
	{
		fadeout = distanceToCenter / i_radius;
	}
	else if( i_falloff == 1 ) /* smooth */
	{
		fadeout = smoothstep( 0, i_radius, distanceToCenter );
	}
	else
	{
		/* fast and bubble */
		fadeout = distanceToCenter / i_radius;
		fadeout = bias( 0.25, fadeout ); 

		if( i_falloff == 3 ) /* bubble */
		{
			fadeout = clamp( 1 - fadeout, 0, 1 );
			fadeout += 1 - smoothstep( 0, 0.1, fadeout );
		}
	}

	return 1-fadeout;
}

float BillowNoise( 
	point p; float time;
	float dim;
	float radius, sizeRand, jitter, falloff, spottyness;
	float ooctaves, frequencyRatio, ratio, amplitude )
{
	point pp = p + set(0.425, 0.6, dim == 3 ? 0.215 : 0);
	int i = 0, j = 0;
	int octaves = ooctaves;
	float lacunarity = 1;
	point particles[];
	float f1 = 0, f2 = 0;
	point p1 = 0, p2 = 0;
	int numParticles = 0;
	float sum = 0;
	float amp = 1;
	float ampSum = 0;
	
	resize(particles, 27);

	for (i = 0; i < octaves; i++ ) 
	{
		if( dim == 3 )
		{
			numParticles = SuspendedParticles( pp * lacunarity, time, radius, jitter, i, f1, p2, f2, p2, particles );
		} else {
			numParticles = SuspendedParticles2d( pp * lacunarity, time, radius, jitter, i, particles );
		}

		if( numParticles > 0 )
		{
			for( j=0; j<numParticles; j++ )
			{
				float radiusScale = 1;

				if( sizeRand != 0 )
				{
					vector cp = particles[j];
					float cpf = float(noise( cp )) * 0.75 - 0.25;
					radiusScale = (0.5 - clamp(cpf, 0, 0.5 ) * sizeRand) * 2;
				}

				float density = ParticleDensity( falloff, particles[j],	pp * lacunarity, radius * radiusScale );

				if( spottyness > 0 )
				{
					float l = spottyness * (cellnoise(particles[j]+set(1, 7, 1023))*2-1);
					density += density * l;
					density = clamp( density, 0, 1); 
				}

				sum += amp * density;
			}
		}
		ampSum += amp;
		amp *= ratio; 
		lacunarity *= frequencyRatio;
	}

	return amplitude * sum / ampSum;
}

/* 
  cos_waves
  
  A simple sum of cosine waves
*/
float
cos_waves(
	float i_x;
	float i_y;
	float i_time;
	float i_numWaves;)
{
	float x = i_x * 2 * PI;
	float y = i_y * 2 * PI;
	float time = i_time * 2 * PI;

	float dirX = 0;
	float dirY = 0;
	float norm = 0;
	float noiseValue = 0;
	float i = 0;

	for(i = 1; i <= i_numWaves; i += 1)
	{
		point noisePoint = noise(i * PI / i_numWaves);
				
		dirX = 2 * xcomp(noisePoint) - 1;
		dirY = 2 * ycomp(noisePoint) - 1;
		float offset = 2 * zcomp(noisePoint) - 1;
		float freqNoise = noise(50 * dirX * dirY);

		norm = sqrt(dirX * dirX + dirY * dirY);
		
		if(norm > EPSILON)
		{
			dirX /= norm;
			dirY /= norm;
			noiseValue += cos((dirX * x + dirY * y)  * (PI) / (6 * freqNoise) + 
				offset * time);
		}
	}

	return noiseValue /= i_numWaves;      
}

#endif


//	float vlfractal ( point P; float freq, distortion )
//{
//float out = 0, octave, tmpFreq = freq, tmpAmp = 1, octaves = 8;
//for (octave=0; octave < octaves; octave += 1)
//{
//vector offset;
//point Pp = P*tmpFreq;
//offset = Pp + vector (.5, .5, .5);
//offset = vector noise( offset ); /* get a random vector */
//offset *= distortion; /* scale the randomization */
//Pp += offset;
//float tmp = float noise( Pp );
//if (octave &gt; 0) tmp = (tmp*2 – 1)*tmpAmp;
//out += tmp; /* distorted-domain noise */
//tmpFreq *= 2;
//tmpAmp *= .5;
//}
//return out;
//}
//
//float vlnoise( point P; float freq, distortion )
//{
//vector offset;
//point Pp = P*freq;
//offset = Pp + vector (.5, .5, .5);
//offset = vector noise( offset ); /* get a random vector */
//offset *= distortion; /* scale the randomization */
//Pp += offset;
//return float noise( Pp ); /* distorted-domain noise */
//}
//
//float worley (point p; string spacename; float freq)
//{
//point pp = transform(spacename, p) * freq;
//point thiscell = point(floor(xcomp(pp))+0.5,
//floor(ycomp(pp))+0.5,
//floor(zcomp(pp))+0.5);
//float dist2nearest = 1000;
//uniform float i,j,k;
//for(i = -1; i < = 1; i+= 1)
//for(j = -1; j <= 1; j+= 1)
//for(k = -1; k <= 1; k+= 1)
//{
//point testcell = thiscell + vector(i,j,k);
//point pos = testcell + vector noise(testcell);
//float dist = distance(pos,pp);
//if(dist < dist2nearest)
//dist2nearest = dist;
//}
//return 1 – dist2nearest;
//}
//
//float worley (point p; string spacename; float freq)
//{
//point pp = transform(spacename, p) * freq;
//point thiscell = point(floor(xcomp(pp))+0.5,
//floor(ycomp(pp))+0.5,
//floor(zcomp(pp))+0.5);
//float dist2nearest = 1000;
//uniform float i,j,k;
//for(i = -1; i < = 1; i+= 1)
//for(j = -1; j <= 1; j+= 1)
//for(k = -1; k <= 1; k+= 1)
//{
//point testcell = thiscell + vector(i,j,k);
//point pos = testcell + vector cellnoise(testcell)-0.5;
//float dist = distance(pos,pp);
//if(dist < dist2nearest)
//dist2nearest = dist;
//}
//return 1 – dist2nearest;
//}
//
//float fBm (point P; float amp, freq, octaves)
//{
//float out = float noise(P*freq);
//float octave, tmpFreq = freq, tmpAmp = amp;
//for (octave=0; octave<octaves; octave += 1)
//{
//tmpFreq *= 2;
//tmpAmp *= .5;
//float tmp = float noise(P*tmpFreq);
//tmp = tmp * 2 – 1;
//out -= tmp*tmpAmp;
//}
//return out;
//}
