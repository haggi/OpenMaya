#ifndef CORONA_VFB_H
#define CORONA_VFB_H
#include "CoronaCore/api/Api.h"

class CornoaVfbCallbacks : public Corona::Abstract::VfbCallbacks {
public:
	Corona::ICore* core;
	bool isWindowClosed;

	CornoaVfbCallbacks() {
		isWindowClosed = false;
	}
	virtual void windowClosed() {
		this->isWindowClosed = true;
		core->cancelRender();
		core->getWxVfb().renderFinished();
	}
	virtual void renderStop(const bool) {
		core->cancelRender();
		core->getWxVfb().renderFinished();
	}
	virtual void renderStart(const bool interactive) {}
	virtual void renderPause() {}
	virtual void duplicate(const int selectedChannelId) {}
	virtual void save(const int selectedChannelId) {}

	virtual wxMenuBar* buildMenu() {
		return NULL;
	}
	virtual void menuCommand(const int menuId, const bool checked) {}
	virtual void sidebarActivated(const bool activated) {}
	virtual void sidebarTabChanged(const Corona::WxVfbTab tab) {}
	virtual void textEditFocus() {}
	virtual void textEditDefocus() {}
	virtual void colorMappingChanged(const Corona::ColorMappingData& newValues) {}
	virtual void saveAll() {}
	virtual void canvasSizeChanged(const Corona::Pixel newSize) {}
	virtual bool zoomingEnabled() {
		return true;
	}
	virtual void mouseOverCanvas(const Corona::Pixel){};
};

#endif