#ifndef KRAY_EVENT_LISTENER_H
#define KRAY_EVENT_LISTENER_H

#include "kraysdk/base/commons.h"
#include "kraysdk/base/listener.h"
#include "threads/renderQueueWorker.h"

namespace krayRender
{
	class EventListener: public Kray::EventListener
	{
	public:
		EventListener();
		int height;
		bool interrupted;
		void doInterrupt();
		void activate(class Instance *k);	// called when listener is activated (with setEventListner() call)
		void deactivate(class Instance *k);	// called when listener is deactivated
		void textLine(void* handle,const char* txt);	// display text line
		void textNewLine(void* handle);	// moves cursor to next line
		const char* getTextLine(void* handle,const char* promptString);	// gets line from command line, 0 return means 'unsupported'
		int getTextFlags(void* handle);
		void* createWindow();
		void destroyWindow(void* windowHandle);
		void setWindowSize(void* windowHandle,const KrayEventWindow& kew);
		void setPixels(void* windowHandle,const KrayEventPixelTable& kept);
		void copyPixels(void* windowHandle,const KrayEventPixelRectangle &kepr);
		int getWindowFlags(void* windowHandle);
		void setWindowFlags(void* windowHandle,int flags);
		void setWindowTitle(void* windowHandle,const char* title);
		void getWindowMouseRawPosition(void* windowHandle, KrayEventMousePosition& mp);
		void getWindowMousePosition(void* windowHandle,KrayEventMousePosition& mp);			// mouse traced inside window, absolute coordinates
		//void getWindowKeyEvent(void* windowHandle,KrayEventKey& key);
		int getMouseRawDPI(void* windowHandle);
		void renderFinished();
		void renderInterupted();
	};

} // namespace krayRender

#endif
