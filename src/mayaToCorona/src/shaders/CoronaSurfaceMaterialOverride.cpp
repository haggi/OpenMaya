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

#include "CoronaSurfaceMaterialOverride.h"
#include <maya/MStringArray.h>
#include <maya/MFragmentManager.h>
#include <maya/MGlobal.h>
#include <fstream>
#include "world.h"

MHWRender::MPxSurfaceShadingNodeOverride* CoronaSurfaceOverride::creator(const MObject& obj)
{
	return new CoronaSurfaceOverride(obj);
}

CoronaSurfaceOverride::CoronaSurfaceOverride(const MObject& obj)
: MPxSurfaceShadingNodeOverride(obj), mfragmentName("")
{
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer)
	{
		MHWRender::MFragmentManager* fragmentMgr = theRenderer->getFragmentManager();
		if (fragmentMgr)
		{

			//MStringArray frags;
			//fragmentMgr->fragmentList(frags);
			//std::ofstream xmlFile("c:/daten/xmlFiles.xml");
			//for (uint i = 0; i < frags.length(); i++)
			//{
			//	MString buffer;
			//	fragmentMgr->getFragmentXML(frags[i], buffer);
			//	xmlFile << buffer.asChar() << "\n";
			//	//std::cout << xmlFile.asChar() << "\n";
			//}
			//xmlFile.close();

			for (uint i = 0; i < getWorldPtr()->shaderSearchPath.length(); i++)
				fragmentMgr->addFragmentPath(getWorldPtr()->shaderSearchPath[i]);
			MStringArray fragments;
			fragments.append("coronaOutput");
			fragments.append("coronaDiffuse");
			fragments.append("coronaSpecular");
			fragments.append("coronaCombiner");
			fragments.append("coronaGeom");
			fragments.append("coronaPassThrough");
			fragments.append("coronaLightAccum");


			for (uint i = 0; i < fragments.length(); i++)
			{
				MString fragment = fragments[i];
				bool fragAdded = fragmentMgr->hasFragment(fragment);
				if (!fragAdded)
				{
					fragAdded = (fragment == fragmentMgr->addShadeFragmentFromFile(fragment + ".xml", false));
					if (fragAdded)
					{
						//MGlobal::displayInfo(MString("Successfully loaded fragment ") + fragment + ".xml");
					}
					else{
						MGlobal::displayInfo(MString("Could not load fragment ") + fragment + ".xml");
					}
				}
			}
			MString graphFragment("CoronaSurface");
			bool fragAdded = fragmentMgr->hasFragment(graphFragment);
			mfragmentName = graphFragment;
			//if (!fragAdded)
			//{
			//	fragAdded = (graphFragment == fragmentMgr->addShadeFragmentFromFile(graphFragment + ".xml", false));
			//	if (fragAdded)
			//	{
			//		MGlobal::displayInfo(MString("Successfully loaded fragment ") + graphFragment + ".xml");
			//		mfragmentName = graphFragment;
			//		MGlobal::displayInfo(MString("Setting mfragmentName to ") + mfragmentName);
			//	}
			//	else{
			//		MGlobal::displayInfo(MString("Could not load fragment ") + graphFragment + ".xml");
			//		mfragmentName = "";
			//	}
			//}
			if (!fragAdded)
			{
				fragAdded = (graphFragment == fragmentMgr->addFragmentGraphFromFile(graphFragment + ".xml"));
				if (fragAdded)
				{
					//MGlobal::displayInfo(MString("Successfully loaded fragmentGraph ") + graphFragment + ".xml");
				}
				else{
					MGlobal::displayInfo(MString("Could not load fragmentGraph ") + graphFragment + ".xml");
					mfragmentName = "";
				}
			}
		}
	}
}

CoronaSurfaceOverride::~CoronaSurfaceOverride()
{
}

MHWRender::DrawAPI CoronaSurfaceOverride::supportedDrawAPIs() const
{
	return MHWRender::kOpenGL;// | MHWRender::kDirectX11;
}

MString CoronaSurfaceOverride::fragmentName() const
{
	return mfragmentName;
}

void CoronaSurfaceOverride::getCustomMappings(
	MHWRender::MAttributeParameterMappingList& mappings)
{
	//MHWRender::MAttributeParameterMapping diffuseMapping(
	//	"color", "diffuse", true, true);
	//mappings.append(diffuseMapping);

	//MHWRender::MAttributeParameterMapping translucenceMapping(
	//	"translucence", "translucency", true, true);
	//mappings.append(translucenceMapping);

	//MHWRender::MAttributeParameterMapping emissionMapping(
	//	"incandescence", "emissionColor", true, true);
	//mappings.append(emissionMapping);

	//MHWRender::MAttributeParameterMapping reflMapping(
	//	"specularColor", "refl", true, true);
	//mappings.append(reflMapping);

	////MHWRender::MAttributeParameterMapping tMapping(
	////	"transparency", "opacity", true, true);
	////mappings.append(tMapping);

	//MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	//if (theRenderer)
	//{
	//	MHWRender::MFragmentManager* fragmentMgr = theRenderer->getFragmentManager();
	//	if (fragmentMgr)
	//	{
	//		MStringArray list;
	//		fragmentMgr->fragmentList(list);
	//		MGlobal::displayInfo("Fragement List:");
	//		//for (uint i = 0; i < list.length(); i++)
	//		//	MGlobal::displayInfo(list[i]);

	//		MString buffer;
	//		fragmentMgr->getFragmentXML("mayaPhongSurface", buffer);
	//		MGlobal::displayInfo("Phong Fragment---------------------");
	//		MGlobal::displayInfo(buffer);
	//	}
	//}

}

MString CoronaSurfaceOverride::primaryColorParameter() const
{
	return "diffuse";
}

MString CoronaSurfaceOverride::transparencyParameter() const
{
	return "opacity";
}

MString CoronaSurfaceOverride::bumpAttribute() const
{
	return "normalCamera";
}

#endif