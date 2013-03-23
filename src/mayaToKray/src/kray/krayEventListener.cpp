#include "krayEventListener.h"
#include <iostream>

namespace krayRender
{
	EventListener::EventListener()
	{
		this->interrupted = false;
	}

	void EventListener::activate(class Instance *k)
	{
		std::cout << " EventListener::activate\n" << std::flush;
	};	// called when listener is activated (with setEventListner() call)

	void EventListener::deactivate(class Instance *k)
	{
		std::cout << " EventListener::deactivate\n" << std::flush;
	};	// called when listener is deactivated

	void EventListener::textLine(void* handle,const char* txt)
	{
		std::cout << "kray says : " << txt << "\n" << std::flush;
	};

	void EventListener::textNewLine(void* handle)
	{
		std::cout << std::endl;
	};

	const char* EventListener::getTextLine(void* handle,const char* promptString)
	{
		std::cout << " EventListener::getTextLine\n" << std::flush;
		return 0;
	};

	int EventListener::getTextFlags(void* handle)
	{
		if( interrupted )
			std::cout << " EventListener::getTextFlags - return stop\n" << std::flush;
		//else
		//	std::cout << " EventListener::getTextFlags - return nonstop\n" << std::flush;
		return interrupted ? KRAY_TEXT_GET_INTERRUPT:0;		// return interupt flag to Kray when user wants to stop rendering
	}

	void* EventListener::createWindow()
	{
		std::cout << " EventListener::createWindow\n" << std::flush;
		return 0;
	}
	
	void EventListener::destroyWindow(void* windowHandle)
	{
		std::cout << " EventListener::destroyWindow\n" << std::flush;
	}

	//void EventListener::setWindowSize(void* windowHandle,const KrayEventWindow& kew)
	//{
	//	std::cout << "EventListener::setWindowSize: x: " << kew.width << " y: " << kew.height << "\n";
	//	this->height = kew.height;
	//}
	int EventListener::setWindowSize(void* windowHandle,const KrayEventWindow& kew)
	{
		std::cout << "EventListener::setWindowSize: x: " << kew.width << " y: " << kew.height << "\n";
		this->height = kew.height;
		return 1;
	}

	void EventListener::setPixels(void* windowHandle,const KrayEventPixelTable& kept)
	{
		//std::cout << " EventListener::setPixels count: " << kept.count << "\n" << std::flush;
		EventQueue::RandomPixel *pixels = new EventQueue::RandomPixel[kept.count];
		for( size_t pId = 0; pId < kept.count; pId++)
		{
			//RGB((c>>16)&0xff,(c>>8)&0xff,c&0xff)
			RV_PIXEL pix;
			pix.r = ((kept.pxls[pId].c>>16)&0xff);
			pix.g = ((kept.pxls[pId].c>>8)&0xff);
			pix.b = ((kept.pxls[pId].c)&0xff);
			pix.a = 1.0f;
			pixels[pId].pixel = pix;
			pixels[pId].x = kept.pxls[pId].x;
			pixels[pId].y = this->height - kept.pxls[pId].y - 1;
		}
		
		EventQueue::Event e;
		e.data = pixels;
		e.numPixels = kept.count;
		e.type = EventQueue::Event::PIXELSDONE; // send event pixels done
		e.pixelMode = EventQueue::Event::PIXELS; // random pixels should be updated
		theRenderEventQueue()->push(e);

	}

	void EventListener::copyPixels(void* windowHandle,const KrayEventPixelRectangle &kepr)
	{
		std::cout << "EventListener::copyPixels\n";

		//size_t tw =  tile.get_width();
		//size_t th =  tile.get_height();
		//size_t numPixels = tw * th;
		//RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		//for( size_t yy = 0; yy < th; yy++)
		//{
		//	for( size_t xx = 0; xx < tw; xx++)
		//	{
		//		size_t pixelId = yy * tw + xx;
		//		size_t yy1 = th - yy - 1;
		//		asf::uint8 *source = uint8_rgb_tile.pixel(xx, yy1);
		//		pixels[pixelId].r = (float)source[0]; 
		//		pixels[pixelId].g = (float)source[1]; 
		//		pixels[pixelId].b = (float)source[2]; 
		//		pixels[pixelId].a = (float)source[3]; 
		//	}
		//}
	
		//size_t x = tile_x * frame_props.m_tile_width;
		//size_t y = tile_y * frame_props.m_tile_height;
		//size_t x1 = x + tw - 1;
		//size_t y1 = y + th;
		//size_t miny = img.properties().m_canvas_height - y1;
		//size_t maxy = img.properties().m_canvas_height - y - 1;

		//EventQueue::Event e;
		//e.data = pixels;
		//e.type = EventQueue::Event::TILEDONE;
		//e.tile_xmin = x;
		//e.tile_xmax = x1;
		//e.tile_ymin = miny;
		//e.tile_ymax = maxy;
		//theRenderEventQueue()->push(e);

		//pixels += y1*sx+x1;
		//sx-=(x2-x1);
		//for (int y=y1 ; y<y2 ; y++){
		//	for (int x=x1 ; x<x2 ; x++){
		//		setPixel(x,y,*pixels++);
		//	}
		//	pixels+=sx;
		//}
	}
	
	int EventListener::getWindowFlags(void* windowHandle)
	{
		//std::cout << " EventListener::getWindowFlags\n" << std::flush;
		return 0;
	}

	void EventListener::setWindowFlags(void* windowHandle,int flags)
	{
		//std::cout << " EventListener::setWindowFlags\n" << std::flush;
	}

	void EventListener::setWindowTitle(void* windowHandle,const char* title)
	{
		//std::cout << " EventListener::setWindowTitle\n" << std::flush;
	}

	void EventListener::getWindowMouseRawPosition(void* windowHandle, KrayEventMousePosition& mp)
	{	// mouse traced outside window, relative coordinates
		getWindowMousePosition(windowHandle,mp);
	}

	void EventListener::getWindowMousePosition(void* windowHandle,KrayEventMousePosition& mp)
	{// mouse traced inside window, absolute coordinates
	}	

	//void EventListener::getWindowKeyEvent(void* windowHandle,KrayEventKey& key)
	//{
	//	std::cout << " EventListener::getWindowKeyEvent\n" << std::flush;
	//}

	int EventListener::getMouseRawDPI(void* windowHandle)
	{
		return 0;
	}

	void EventListener::renderFinished()
	{
		std::cout << " EventListener::renderFinished\n" << std::flush;
	}

	void EventListener::renderInterupted()
	{
		std::cout << " EventListener::renderInterupted\n" << std::flush;
	}

	void EventListener::doInterrupt()
	{
		std::cout << " EventListener::doInterrupt\n" << std::flush;
		this->interrupted = true;
	}

} // namespace
