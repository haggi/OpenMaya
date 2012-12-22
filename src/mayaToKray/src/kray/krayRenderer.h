#ifndef KRAY_RENDERER_H
#define KRAY_RENDERER_H

#include "kraysdk/proto/direct.h"
#include <vector>

class mtkr_MayaObject;
class mtkr_MayaScene;
class mtkr_RenderGlobals;
class MObject;

namespace krayRender
{
	class KrayRenderer
	{
	public:
		KrayRenderer();
		~KrayRenderer();

		mtkr_MayaScene *mtkr_scene;
		mtkr_RenderGlobals *mtkr_renderGlobals;
		Kray::DirectPrototyper *prot;

		std::vector<mtkr_MayaObject *> interactiveUpdateList;
		std::vector<MObject> interactiveUpdateMOList;

		void updateTransform(mtkr_MayaObject *obj);	// updates the transform node in renderer
		void updateDeform(mtkr_MayaObject *obj);	// updates the mesh deform node in renderer

		void defineGeometry(mtkr_MayaObject *obj);
		void defineMesh(mtkr_MayaObject *obj);

		void render();

	private:
	};
} // namespace

#endif
