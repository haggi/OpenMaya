//-
// ===========================================================================
// Copyright 2012 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
//+

#ifdef HAS_OVERRIDE

#include "asFast_sss_surface_shaderOverride.h"

MHWRender::MPxSurfaceShadingNodeOverride* asFast_sss_surface_shaderOverride::creator(const MObject& obj)
{
	return new asFast_sss_surface_shaderOverride(obj);
}

asFast_sss_surface_shaderOverride::asFast_sss_surface_shaderOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
}

asFast_sss_surface_shaderOverride::~asFast_sss_surface_shaderOverride()
{
}

MHWRender::DrawAPI asFast_sss_surface_shaderOverride::supportedDrawAPIs() const
{
	// works in both gl and dx
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString asFast_sss_surface_shaderOverride::fragmentName() const
{
	// Just reuse Maya's lambert surface shader
	return "mayaLambertSurface";
}

void asFast_sss_surface_shaderOverride::getCustomMappings(
	MHWRender::MAttributeParameterMappingList& mappings)
{
	// The "color", "transparency" and "incandescence" attributes are all
	// named the same as the corresponding parameters on the fragment so they
	// will map automatically. Need to remap diffuseReflectivity and
	// translucence though
	MHWRender::MAttributeParameterMapping diffuseMapping(
		"diffuse", "diffuseReflectivity", true, true);
	mappings.append(diffuseMapping);

	MHWRender::MAttributeParameterMapping translucenceMapping(
		"translucence", "translucenceCoeff", true, true);
	mappings.append(translucenceMapping);
}

MString asFast_sss_surface_shaderOverride::primaryColorParameter() const
{
	// Use the color parameter from the lambert fragment as the primary color
	return "color";
}

MString asFast_sss_surface_shaderOverride::transparencyParameter() const
{
	// Use the "transparency" parameter from the lambert fragment for transparency
	return "transparency";
}

MString asFast_sss_surface_shaderOverride::bumpAttribute() const
{
	// Use the "normalCamera" attribute to recognize bump connections
	return "normalCamera";
}

#endif