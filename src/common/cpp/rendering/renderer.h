#ifndef RENDERER_H
#define RENDERER_H

class MayaObject;

namespace MayaTo{

	class Renderer{

	public:
		virtual void defineCamera() = 0;
		virtual void defineEnvironment() = 0;
		virtual void defineGeometry() = 0;
		virtual void defineLights() = 0;

		virtual void render() = 0;
		virtual void initializeRenderer() = 0;
		virtual void updateShape(MayaObject *obj) = 0;
		virtual void updateTransform(MayaObject *obj) = 0;
		virtual void IPRUpdateEntities() = 0;
		virtual void reinitializeIPRRendering() = 0;
		virtual void abortRendering() = 0;
	private:

	};
}

#endif