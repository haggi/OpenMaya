/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __ocean_h
#define __ocean_h

/*
begin inputs
	float alphaGain
	float alphaIsLuminance
	float alphaOffset
	color colorGain
	float colorMode
	color colorOffset
	color defaultColor
	float foamEmission
	float foamThreshold
	float invert
	float numFrequencies
	float observerSpeed
	float outFoam
	float scale
	float time
	init={ss,tt} float2 uvCoord
	float waveDirSpread
	float[] waveHeight[].waveHeight_Position
	float[] waveHeight[].waveHeight_FloatValue
	uniform float[] waveHeight[].waveHeight_Interp
	float waveLengthMin
	float waveLengthMax
	float[] wavePeaking[].wavePeaking_Position
	float[] wavePeaking[].wavePeaking_FloatValue
	uniform float[] wavePeaking[].wavePeaking_Interp
	float[] waveTurbulence[].waveTurbulence_Position
	float[] waveTurbulence[].waveTurbulence_FloatValue
	uniform float[] waveTurbulence[].waveTurbulence_Interp
	float2 windUV
end inputs

begin outputs
	float outAlpha
	color outColor
end outputs
*/

#include "utils.h"
#include "ocean_utils.h"

void
maya_ocean(
	// Inputs
	//
	float i_alphaGain;
	uniform float i_alphaIsLuminance;
	float i_alphaOffset;
	color i_colorGain;
	float i_colorMode;
	color i_colorOffset;
	color i_defaultColor;
	float i_foamEmission;
	float i_foamThreshold;
	uniform float i_invert;
	uniform float i_numFrequencies;
	float i_observerSpeed;
	float i_outFoam;
	float i_scale;
	float i_time;
	float i_uvCoord[2];
	float i_waveDirSpread;
	float i_waveHeightPositions[];
	float i_waveHeightFloatValues[];
	uniform float i_waveHeightInterps[];
	uniform float i_waveLengthMin;
	uniform float i_waveLengthMax;
	float i_wavePeakingPositions[];
	float i_wavePeakingFloatValues[];
	uniform float i_wavePeakingInterps[];
	float i_waveTurbulencePositions[];
	float i_waveTurbulenceFloatValues[];
	uniform float i_waveTurbulenceInterps[];
	float i_windUV[2];
	
	// Outputs
	//
	output float o_outAlpha;
	output color o_outColor;)
{
	varying float ss = i_uvCoord[0];
	varying float tt = i_uvCoord[1];

	if(ISUVDEFINED(ss, tt))
	{
		ss = mod(ss, WRAPMAX);
		tt = mod(tt, WRAPMAX);

		float waveLengthMin  = i_waveLengthMin;
		float waveLengthMax = i_waveLengthMax;
	 
		if(waveLengthMax < waveLengthMin)
		{
			waveLengthMax = waveLengthMin;
			waveLengthMin = i_waveLengthMax;
		}
	  
		varying float ocean;
		varying float foam;
    
		getOcean(ss, tt, i_time, i_scale, i_windUV, i_observerSpeed,
			i_waveDirSpread, i_numFrequencies, waveLengthMin, waveLengthMax,
			i_waveHeightPositions, i_waveHeightFloatValues, i_waveHeightInterps,
			i_waveTurbulencePositions, i_waveTurbulenceFloatValues,
			i_waveTurbulenceInterps, i_wavePeakingPositions, i_wavePeakingFloatValues,
			i_wavePeakingInterps, i_foamEmission, i_foamThreshold,
			ocean, foam);
	 
		float normalizedWaves = 2 * ocean / (i_scale * waveLengthMax) + 0.5;
	 
		if(i_colorMode == 0)
		{
			// WaveHeight mode
			o_outColor = normalizedWaves;
		}
		else if(i_colorMode == 1)
		{
			// Foam mode
			o_outColor = foam;
		}
		else
		{
			// FoamOnWaves mode
			o_outColor = color(
				foam, 
				foam + 0.5 * normalizedWaves * (1 - foam),
				foam + 0.6 * normalizedWaves * (1 - foam));
		}

		o_outAlpha = ocean;
 
		colorBalance(
			o_outColor, 
			o_outAlpha,
			1,
			i_alphaGain,
			i_alphaOffset,
			i_colorGain,
			i_colorOffset,
			i_invert);
	}
	else
	{
		o_outColor = i_defaultColor;
		o_outAlpha = 1.0;
	}
}

#endif /* __ocean_h */

