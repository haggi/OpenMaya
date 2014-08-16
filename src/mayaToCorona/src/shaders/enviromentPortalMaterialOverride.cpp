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

#include "enviromentPortalOverride.h"

MHWRender::MPxSurfaceShadingNodeOverride* enviromentPortalOverride::creator(const MObject& obj)
{
	return new enviromentPortalOverride(obj);
}

enviromentPortalOverride::enviromentPortalOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
}

enviromentPortalOverride::~enviromentPortalOverride()
{
}

MHWRender::DrawAPI enviromentPortalOverride::supportedDrawAPIs() const
{
	// works in both gl and dx
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString enviromentPortalOverride::fragmentName() const
{
	// Just reuse Maya's lambert surface shader
	return "mayaLambertSurface";
}

void enviromentPortalOverride::getCustomMappings(
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

MString enviromentPortalOverride::primaryColorParameter() const
{
	// Use the color parameter from the lambert fragment as the primary color
	return "color";
}

MString enviromentPortalOverride::transparencyParameter() const
{
	// Use the "transparency" parameter from the lambert fragment for transparency
	return "transparency";
}

MString enviromentPortalOverride::bumpAttribute() const
{
	// Use the "normalCamera" attribute to recognize bump connections
	return "normalCamera";
}

#endif