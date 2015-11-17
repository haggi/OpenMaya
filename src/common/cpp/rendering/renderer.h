#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <maya/MString.h>

class MayaObject;
class MObject;
struct InteractiveElement;

namespace MayaTo{

	class Renderer{

	public:
		std::vector<InteractiveElement *>  interactiveUpdateList;
		virtual ~Renderer() {};
		virtual void defineCamera() = 0;
		virtual void defineEnvironment() = 0;
		virtual void defineGeometry() = 0;
		virtual void defineLights() = 0;
		// interactiveFbCallback can be called after a rendering is finished and
		// the user wants to modify the color mapping of the final rendering.
		// all interactive callbacks should be pushed into the worker queue with INTERACTIVEFBCALLBACK so that we don't get
		// any race conditions if the callback takes longer than the next call.
		virtual void interactiveFbCallback() = 0;
		//virtual void framebufferCallback() = 0;

		virtual void render() = 0;
		// initializeRenderer is called before rendering starts
		// it should prepare all data which can/should be reused during
		// ipr/frame/sequence rendering
		virtual void initializeRenderer() = 0;
		// unInitializeRenderer is called at the end of rendering.
		virtual void unInitializeRenderer() = 0;
		// updateShape is called during scene updating. If a renderer can update motionblur steps on the fly,
		// the geometry is defined at the very first step and later this definition will be updated for every motion step.
		// Other renderers will need a complete definition of all motionblur steps at once, so the motion steps will be
		// in the geometry e.g. with obj->addMeshData(); and at the very last step, everything is defined.
		virtual void updateShape(std::shared_ptr<MayaObject> obj) = 0;
		// This method is necessary only if the renderer is able to update the transform definition interactively.
		// In other cases, the world space transform will be defined directly during the creation of the geometry.
		virtual void updateTransform(std::shared_ptr<MayaObject> obj) = 0;
		virtual void abortRendering() = 0;
		// make an interactive update of the scene. Before this call the interactiveUpdateList should be filled appropriatly
		virtual void doInteractiveUpdate() = 0;
		virtual void handleUserEvent(int event, MString strData, float floatData, int intData) = 0;
	private:

	};
}

#endif