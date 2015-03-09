#ifdef HAS_OVERRIDE

#include <maya/MGlobal.h>
#include <maya/MFragmentManager.h>
#include "coronaVolumeMaterialOverride.h"
#include "world.h"


MHWRender::MPxSurfaceShadingNodeOverride* VolumeMaterialOverride::creator(const MObject& obj)
{
	return new VolumeMaterialOverride(obj);
}

VolumeMaterialOverride::VolumeMaterialOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj)
{
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer)
	{
		MHWRender::MFragmentManager* fragmentMgr = theRenderer->getFragmentManager();
		if (fragmentMgr)
		{
			for (uint i = 0; i < MayaTo::getWorldPtr()->shaderSearchPath.length(); i++)
				fragmentMgr->addFragmentPath(MayaTo::getWorldPtr()->shaderSearchPath[i]);

			MString fragment = "CoronaVolume";
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

VolumeMaterialOverride::~VolumeMaterialOverride()
{
}

MHWRender::DrawAPI VolumeMaterialOverride::supportedDrawAPIs() const
{
	// works in both gl and dx
	return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString VolumeMaterialOverride::fragmentName() const
{
	return "CoronaVolume";
}

MString VolumeMaterialOverride::primaryColorParameter() const
{
	return "attenuationColor";
}

MString VolumeMaterialOverride::transparencyParameter() const
{
	return "opacity";
}

#endif