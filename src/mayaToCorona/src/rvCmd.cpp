#include "rvCmd.h"
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include <maya/MTimerMessage.h>
#include <thread>
#include <memory>
#include <maya/MColor.h>

static MColor color = 0.0;
static bool renderDone = false;
static bool renderDoneRegistered = false;

void updateRenderView(float time, float lastTime, void *userPtr)
{
	std::shared_ptr<RV_PIXEL> data(new RV_PIXEL[640*480]);
	for (int y = 0; y < 480; y++)
	{
		for (int x = 0; x < 640; x++)
		{
			int index = y * 640 + x;
			data.get()[index].r = color.r;
			data.get()[index].g = color.g;
			data.get()[index].b = color.b;
			data.get()[index].a = 255.0;
		}
	}
	MRenderView::updatePixels(0, 640-1, 0, 480-1, data.get());
	MRenderView::refresh(0, 640 - 1, 0, 480 - 1);

	if (renderDone && !(renderDoneRegistered))
	{
		renderDoneRegistered = true;
		MGlobal::displayInfo("rendering done.");
		MRenderView::endRender();
	}

}


void renderProcess()
{

	for (int i = 0; i < 256; i++)
	{
		color.b = i;
		color.r = 0;
		color.g = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	MGlobal::displayInfo("renderProcess done.");
	renderDone = true;
}

void postRenderProcess()
{
	while (!renderDone)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	MGlobal::displayInfo("rendering done - starting post render process.");
	for (int i = 0; i < 256; i++)
	{
		color.r = i;
		color.g = i;
		color.b = i;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	MGlobal::displayInfo("postRenderProcess done.");
}

void* RvCmd::creator()
{
	return new RvCmd();
}

RvCmd::RvCmd() {}
RvCmd::~RvCmd() {}


MStatus RvCmd::doIt(const MArgList& args)
{
	MGlobal::displayInfo("Starting RvCmd");
	renderCallbackId = MTimerMessage::addTimerCallback(0.1, updateRenderView, nullptr);
	MRenderView::startRender(640,480, false, true);
	std::thread rp = std::thread(renderProcess);
	std::thread prp = std::thread(postRenderProcess);
	rp.detach();
	prp.detach();

	return MStatus::kSuccess;
}
