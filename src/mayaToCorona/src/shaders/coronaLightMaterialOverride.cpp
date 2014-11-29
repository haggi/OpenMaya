#ifdef HAS_OVERRIDE

#include <maya/MGlobal.h>
#include <maya/MFragmentManager.h>
#include "world.h"
#include "coronaLightMaterialOverride.h"

MHWRender::MPxSurfaceShadingNodeOverride* LightMaterialOverride::creator(const MObject& obj)
{
	return new LightMaterialOverride(obj);
}

LightMaterialOverride::LightMaterialOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer)
	{
		MHWRender::MFragmentManager* fragmentMgr = theRenderer->getFragmentManager();
		if (fragmentMgr)
		{
			for (uint i = 0; i < getWorldPtr()->shaderSearchPath.length(); i++)
				fragmentMgr->addFragmentPath(getWorldPtr()->shaderSearchPath[i]);

			MString fragment = "CoronaLight";
			bool fragAdded = fragmentMgr->hasFragment(fragment);
			if (!fragAdded)
			{
				fragAdded = (fragment == fragmentMgr->addShadeFragmentFromFile(fragment + ".xml", false));
				if (fragAdded)
				{
					MGlobal::displayInfo(MString("Successfully loaded fragment ") + fragment + ".xml");
				}
				else{
					MGlobal::displayInfo(MString("Could not load fragment ") + fragment + ".xml");
				}
			}
		}
	}
}

LightMaterialOverride::~LightMaterialOverride()
{
}

MHWRender::DrawAPI LightMaterialOverride::supportedDrawAPIs() const
{
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString LightMaterialOverride::fragmentName() const
{
	return "CoronaLight";
}

MString LightMaterialOverride::primaryColorParameter() const
{
	return "emissionColor";
}

#endif