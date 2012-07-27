#ifndef RAMP_SHDER_H
#define RAMP_SHDER_H

#include "voplib.h"
#include "utils.h"

void RampShader(
		vector color_values[];
		float color_positions[];
		int colorInput;
		vector transparency_values[];
		float transparency_positions[];
		vector incandescence_values[];
		float incandescence_positions[];
		float diffuse;
		vector normalCamera;
		float forwardScatter;
		float translucence;
		float translucenceFocus;
		float translucenceDepth;
		float specularity;
		float eccentricity;
		vector specularColor_values[];
		float specularColor_positions[];
		float specularRollOff_values[];
		float specularRollOff_positions[];
		float reflectivity_values[];
		float reflectivity_positions[];
		vector environment_values[];
		float environment_positions[];
		vector reflectedColor;
		int refractions;
		float refractiveIndex;
		int refractionLimit;
		float lightAbsorbance;
		float surfaceThickness;
		float shadowAttenuation;
		float chromaticAberration;
		int reflectionLimit;
		float reflectionSpecularity;
		int matteOpacityMode;
		float matteOpacity;

		vector outColor;
		vector outTransparency;
		vector outMatteOpacity;
		 )
{
	
	// diffuse lighting
	vector diffuseLighting = 0;
    vector ii = normalize(I);
	vector nn = normalize(normalCamera);
	vector one = {1,1,1};
    vector nf = vop_frontface(nn, ii);	

	vector lightSum = 0;
	float lightBrightness[];
	vector lightColors[];
	float lightAngles[];
	int count = 0;
	illuminance(P, nf)
	{
		shadow(Cl);
		// Here, Cl and L will be set to the value/direction for the
		// current light source.
		lightSum += Cl;
		// To force the shadow shader to be called, use:
		// shadow(Cl);
		push(lightAngles,clamp(dot(normalize(L), nf), 0, 1));
		push(lightBrightness, luminance(Cl));
		push(lightColors, Cl);
	}
	if( diffuse > 0.0)
	{
		diffuseLighting = diffuse(nf) * diffuse;
	}
	float colorRampPosition = 0.0;

	string ramp_basis_strings[];
	int numElements = 0;
	numElements = arraylength(color_values);
	resize(ramp_basis_strings, numElements);
	for( count = 0; count < numElements; count++)
	{
		ramp_basis_strings[count] = "linear";
	}
	float splinePos = 0.0;
	vector colorValue = 0;

	// light angle
	if( colorInput == 0)
	{
		numElements = arraylength(lightAngles);
		for( count = 0; count < numElements; count++)
		{
			splinePos = spline("solvelinear", lightAngles[count], color_positions);
			colorValue += spline(ramp_basis_strings, splinePos, color_values, "colorspace", "cspace:rgb");		
		}
	}
	// facing angle
	if( colorInput == 1)
	{
		colorRampPosition = clamp(dot(nf, -ii), 0, 1);
		splinePos = spline("solvelinear", colorRampPosition, color_positions);
		colorValue += spline(ramp_basis_strings, splinePos, color_values, "colorspace", "cspace:rgb");		
	}
	// brightness
	if( colorInput == 2)
	{
		numElements = arraylength(lightBrightness);
		for( count = 0; count < numElements; count++)
		{
			splinePos = spline("solvelinear", lightBrightness[count], color_positions);
			colorValue += spline(ramp_basis_strings, splinePos, color_values, "colorspace", "cspace:rgb");		
		}
	}
	// normalizedBrightness
	if( colorInput == 3)
	{
		numElements = arraylength(lightBrightness);
		for( count = 0; count < numElements; count++)
		{
			float normalizedBrightness = lightBrightness[count]/luminance(lightSum);
			splinePos = spline("solvelinear", normalizedBrightness, color_positions);
			colorValue += spline(ramp_basis_strings, splinePos, color_values, "colorspace", "cspace:rgb");		
		}
	}

	// transparency is always based on facing ratio
	colorRampPosition = clamp(dot(nf, -ii), 0, 1);
	splinePos = spline("solvelinear", colorRampPosition, transparency_positions);
	vector transparency = spline(ramp_basis_strings, splinePos, transparency_values, "colorspace", "cspace:rgb");		
	float opacity = 1.0 - luminance(transparency);

	// incandescence is always based on facing ratio
	colorRampPosition = clamp(dot(nf, -ii), 0, 1);
	splinePos = spline("solvelinear", colorRampPosition, incandescence_positions);
	vector incandescence = spline(ramp_basis_strings, splinePos, incandescence_values, "colorspace", "cspace:rgb");		

	// forward scatter
	diffuseLighting *= (1.0 - forwardScatter);

	// translucency
	vector translucenceColor = {0,0,0};
	if( translucence > 0.0)
	{
		vector outTranslucency = {0,0,0};
		getTranslucence(ii, translucence, translucenceDepth, translucenceFocus, outTranslucency);
		translucenceColor = colorValue * outTranslucency;
	}	
	
	// specularity
	vector uvw = {0,0,0};
	float urough = 0.0, vrough = 0.0;	
	int tstyle = 0;
	vector clr = {0,0,0};
	vector ambientColor = 0;
	vector specularColor = 1;
	float cosinePower = eccentricity;
    vector phongSpec = vop_lighting("blinn", nf, ii, uvw, ambientColor, clr, specularColor, cosinePower, vrough, tstyle);
	
	// specular color ramp is based on light angle
	specularColor = 0;
	numElements = arraylength(lightAngles);
	for( count = 0; count < numElements; count++)
	{
		splinePos = spline("solvelinear", lightAngles[count], specularColor_positions);
		specularColor += spline(ramp_basis_strings, splinePos, specularColor_values, "colorspace", "cspace:rgb");		
	}
	
	// specular rolloff ramp
	colorRampPosition = clamp(dot(nf, -ii), 0, 1);
	splinePos = spline("solvelinear", colorRampPosition, specularRollOff_positions);
	float specularRolloff = spline(ramp_basis_strings, splinePos, specularRollOff_values);		
	specularColor = phongSpec * specularColor * specularRolloff;
	
	// reflectivity is always based on facing ratio
	colorRampPosition = clamp(dot(nf, -ii), 0, 1);
	splinePos = spline("solvelinear", colorRampPosition, reflectivity_positions);
	float reflectivity = spline(ramp_basis_strings, splinePos, reflectivity_values);
	vector reflectColor = 1;
	if( reflectivity > 0.0)
	{
		float mtm_reflBlur = 0.0;
		int mtm_reflSamples = 1;
		getReflection( ii, nn, mtm_reflBlur, mtm_reflSamples, reflectivity, reflectColor);

		// the more reflective a material is the less transparent it is, so if it is 100% reflective,
		// it is not transparent any more.
		transparency = transparency * (1.0 - reflectivity);
	}else{
		reflectColor *= 0.0;
	}	
	// environment mapping
	vector Nw = normalize(vtransform("space:world", reflect( ii, nf )));
	float envRamp = clamp(dot(Nw,{0,1,0}) * 0.5 + 0.5, 0, 1);

	splinePos = spline("solvelinear", envRamp, environment_positions);
	vector envColor = spline(ramp_basis_strings, splinePos, environment_values, "colorspace", "cspace:rgb");	
	reflectColor += envColor * reflectivity;

	vector refractedColor = {0,0,0};
	vector refractedOpacity = {1,1,1};
	float mtm_refrBlur = 0.0;
	int mtm_refrSamples = 1;
	if( (refractions > 0) && (refractiveIndex != 1.0) && (luminance(transparency) > 0.0))
	{
		getRefraction(ii, nn, mtm_refrBlur, mtm_refrSamples, refractiveIndex, refractedColor, refractedOpacity);
		refractedColor = refractedColor * transparency; // attenuate refracted color by transparency
		transparency += (one - transparency) * refractedOpacity;
	}

	outColor = diffuseLighting * colorValue * opacity * (1.0 - reflectivity) + translucenceColor + incandescence + specularColor + reflectColor + refractedColor;
	//outColor = reflectColor;
	outTransparency = transparency;
	outMatteOpacity = one - transparency;
}

#endif