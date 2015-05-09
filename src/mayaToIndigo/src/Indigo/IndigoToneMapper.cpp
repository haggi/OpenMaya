#include "Indigo.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "renderGlobals.h"
#include "mayaScene.h"
#include "world.h"

void IndigoRenderer::createToneMapper()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	Indigo::SceneNodeTonemappingRef tone_mapping(new Indigo::SceneNodeTonemapping());

	switch(getEnumInt("tone_mapper", gFn))
	{
	case 0:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Linear);
			tone_mapping->scale = getFloatAttr("tone_linearScale", gFn, 1.0);
		}
		break;
	case 1:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
			tone_mapping->pre_scale = getFloatAttr("tone_reinhardPreScale", gFn, 1.0);
			tone_mapping->post_scale = getFloatAttr("tone_reinhardPostScale", gFn, 1.0);
			tone_mapping->burn = getFloatAttr("tone_reinhardBurn", gFn, 10.0);
		}
		break;
	case 2:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Camera);
			tone_mapping->ev_adjust = getFloatAttr("tone_cameraEv_adjust", gFn, 0.0);
			tone_mapping->film_iso = getFloatAttr("tone_cameraFilm_iso", gFn, 100.0);
			MString cameraFunc = (getRendererHome() + "ressources/");
			tone_mapping->response_function_path = Indigo::String(getStringAttr("tone_cameraResponse_function_path", gFn, "").asChar());
		}
		break;
	default:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
			tone_mapping->pre_scale = 1;
			tone_mapping->post_scale = 1;
			tone_mapping->burn = 6;
		}
	};


	sceneRootRef->addChildNode(tone_mapping);

}