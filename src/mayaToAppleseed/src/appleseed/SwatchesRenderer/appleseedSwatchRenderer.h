#ifndef MTAP_NEWSWATCHRENDER_H
#define MTAP_NEWSWATCHRENDER_H

#include "renderer/api/scene.h"
#include "renderer/api/project.h"
#include "renderer/global/globallogger.h"
#include "renderer/api/rendering.h"

#include <maya/MObject.h>

namespace asf = foundation;
namespace asr = renderer;

class NewSwatchRenderer;

class AppleseedSwatchRenderer
{
public:
	AppleseedSwatchRenderer();
	~AppleseedSwatchRenderer();
	void mainLoop();
	void setSize(int size);
	void setShader(MObject shader);
	void renderSwatch();
	void renderSwatch(NewSwatchRenderer *sr);
	void fillSwatch(float *pixels);
	bool terminateLoop = false;
	bool enableSwatchRenderer = true;
	bool loopDone = false;
	
	asf::auto_release_ptr<asr::Scene> scene;
	asf::auto_release_ptr<asr::Project> project;
	std::auto_ptr<asf::ILogTarget> log_target;
	std::auto_ptr<asr::MasterRenderer> mrenderer;
	asr::DefaultRendererController renderer_controller;

	static void startAppleseedSwatchRender(AppleseedSwatchRenderer *swRend);
	static void terminateAppleseedSwatchRender(AppleseedSwatchRenderer *swRend);

	void defineMaterial(MObject shadingNode);

};


#endif