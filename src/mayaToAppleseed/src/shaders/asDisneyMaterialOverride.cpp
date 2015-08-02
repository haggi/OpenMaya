#include <maya/MGlobal.h>
#include "asDisneyMaterialOverride.h"
#include <maya/MFragmentManager.h>
#include "world.h"

MHWRender::MPxSurfaceShadingNodeOverride* asDisneyMaterialOverride::creator(const MObject& obj)
{
	return new asDisneyMaterialOverride(obj);
}

asDisneyMaterialOverride::asDisneyMaterialOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
	//MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	//if (theRenderer)
	//{
	//	MHWRender::MFragmentManager* fragmentMgr = theRenderer->getFragmentManager();
	//	if (fragmentMgr)
	//	{
	//		for (uint i = 0; i < MayaTo::getWorldPtr()->shaderSearchPath.length(); i++)
	//			fragmentMgr->addFragmentPath(MayaTo::getWorldPtr()->shaderSearchPath[i]);

	//		MString fragment = "asDisneyMaterial";
	//		bool fragAdded = fragmentMgr->hasFragment(fragment);
	//		if (!fragAdded)
	//		{
	//			fragAdded = (fragment == fragmentMgr->addShadeFragmentFromFile(fragment + ".xml", false));
	//			if (fragAdded)
	//			{
	//				MGlobal::displayInfo(MString("Successfully loaded fragment ") + fragment + ".xml");
	//			}
	//			else{
	//				MGlobal::displayInfo(MString("Could not load fragment ") + fragment + ".xml");
	//			}
	//		}
	//	}
	//}
}

asDisneyMaterialOverride::~asDisneyMaterialOverride()
{
}

MHWRender::DrawAPI asDisneyMaterialOverride::supportedDrawAPIs() const
{
	// works in both gl and dx
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString asDisneyMaterialOverride::fragmentName() const
{
	// Just reuse Maya's lambert surface shader
	return "mayaLambertSurface";
}

void asDisneyMaterialOverride::getCustomMappings(
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

MString asDisneyMaterialOverride::primaryColorParameter() const
{
	// Use the color parameter from the lambert fragment as the primary color
	return "color";
}

MString asDisneyMaterialOverride::transparencyParameter() const
{
	// Use the "transparency" parameter from the lambert fragment for transparency
	return "transparency";
}

MString asDisneyMaterialOverride::bumpAttribute() const
{
	// Use the "normalCamera" attribute to recognize bump connections
	return "normalCamera";
}

//#endif