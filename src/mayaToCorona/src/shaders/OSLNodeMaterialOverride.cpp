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

#include "OSLNodeOverride.h"

MHWRender::MPxSurfaceShadingNodeOverride* OSLNodeOverride::creator(const MObject& obj)
{
	return new OSLNodeOverride(obj);
}

OSLNodeOverride::OSLNodeOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
}

OSLNodeOverride::~OSLNodeOverride()
{
}

MHWRender::DrawAPI OSLNodeOverride::supportedDrawAPIs() const
{
	// works in both gl and dx
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString OSLNodeOverride::fragmentName() const
{
	// Just reuse Maya's lambert surface shader
	return "mayaLambertSurface";
}

void OSLNodeOverride::getCustomMappings(
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

MString OSLNodeOverride::primaryColorParameter() const
{
	// Use the color parameter from the lambert fragment as the primary color
	return "color";
}

MString OSLNodeOverride::transparencyParameter() const
{
	// Use the "transparency" parameter from the lambert fragment for transparency
	return "transparency";
}

MString OSLNodeOverride::bumpAttribute() const
{
	// Use the "normalCamera" attribute to recognize bump connections
	return "normalCamera";
}

#endif