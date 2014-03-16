#include "Indigo.h"
#include "utilities/tools.h"
#include "../mtin_common/mtin_renderGlobals.h"

void IndigoRenderer::createToneMapper()
{
	Indigo::SceneNodeTonemappingRef tone_mapping(new Indigo::SceneNodeTonemapping());

	switch(this->mtin_renderGlobals->tone_mapper)
	{
	case 0:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Linear);
			tone_mapping->scale = this->mtin_renderGlobals->tone_linearScale;
		}
		break;
	case 1:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Reinhard);
			tone_mapping->pre_scale = this->mtin_renderGlobals->tone_reinhardPreScale;
			tone_mapping->post_scale = this->mtin_renderGlobals->tone_reinhardPostScale;
			tone_mapping->burn = this->mtin_renderGlobals->tone_reinhardBurn;
		}
		break;
	case 2:
		{
			tone_mapping->setType(Indigo::SceneNodeTonemapping::Camera);
			tone_mapping->ev_adjust = this->mtin_renderGlobals->tone_cameraEv_adjust;
			tone_mapping->film_iso = this->mtin_renderGlobals->tone_cameraFilm_iso;
			MString cameraFunc = (getRendererHome() + "ressources/");
			tone_mapping->response_function_path = Indigo::String(this->mtin_renderGlobals->tone_cameraResponse_function_path.asChar());
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