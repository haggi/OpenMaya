//#include <maya/MRenderView.h>
//#include "mayaScene.h"
//#include "threads/queue.h"
//#include "utilities/logging.h"
//#include "utilities/tools.h"
//#include "utilities/attrTools.h"
//
//#include <boost/thread/thread.hpp>
//#include <boost/bind.hpp>
//
//static Logging logger;
//
//void MayaScene::prepareRenderView()
//{
//	if(!MRenderView::doesRenderEditorExist())
//		return;
//
//	uint width = renderGlobals->imgWidth;
//	uint height = renderGlobals->imgHeight;
//	bool immediateFeedback = true;
//	bool doNotClearBG = false;
//	MStatus status = MRenderView::startRender(width, height, doNotClearBG, immediateFeedback);
//	renderComputation.beginComputation();
//}
//
//void MayaScene::finishRenderView()
//{
//	renderComputation.endComputation();
//	return;
//}
//
////
////	event worker gets the events placed in the event queue
////
//
//
//bool MayaScene::eventWorkerThread(MayaScene *scene)
//{
//	EventQueue::Event qEvent;
//	int tileCount = 0;
//
//	while(true)
//	{
//		EventQueue::theQueue()->wait_and_pop(qEvent);
//
//		//	this->finishRenderView();
//		switch(qEvent.type)
//		{
//			case EventQueue::Event::RENDERDONE:
//			{
//				logger.debug(MString("Rendering done called."));
//				scene->finishRenderView();
//				break;
//			}
//
//			case EventQueue::Event::INTERRUPT:
//			{
//				logger.debug(MString("Interrupt called."));
//				break;
//			}
//
//			case EventQueue::Event::PRETILE:
//			{
//				logger.debug(MString("PreTile called."));
//				if( qEvent.data != NULL)
//				{
//					//reverse tile position in image
//					uint width = scene->renderGlobals->imgWidth;
//					uint height = scene->renderGlobals->imgHeight;
//
//					size_t miny = height - qEvent.tile_ymax;
//					size_t maxy = height - qEvent.tile_ymin - 1;
//
//					RV_PIXEL* pixels = (RV_PIXEL*)qEvent.data;
//					if( MRenderView::doesRenderEditorExist() )
//					{
//						MRenderView::updatePixels(qEvent.tile_xmin, qEvent.tile_xmax, miny, maxy, pixels);
//						MRenderView::refresh(qEvent.tile_xmin, qEvent.tile_xmax, miny, maxy);
//					}
//					delete[] pixels;
//				}
//			}
//
//			case EventQueue::Event::STARTRENDER:
//			{
//				tileCount = 0;
//				scene->prepareRenderView();
//				break;
//			}
//
//			case EventQueue::Event::TILEDONE:
//			{
//				logger.debug(MString("Tile done called."));
//				if( qEvent.data != NULL)
//				{
//					//if( ((tileCount++ % 20) == 0) || (tileCount == numTiles))
//					//{
//					//	//logger.progress(MString("Tile ") + tileCount + " of " + numTiles + " done");
//					//	int p =  (int)((float)tileCount/(float)numTiles * 100.0);
//					//	MString perc =  MString("") + p;
//					//	if( p < 10)
//					//		perc = MString(" ") + perc;
//					//	logger.progress(MString("") + perc + "% done");
//					//}
//					RV_PIXEL* pixels = (RV_PIXEL*)qEvent.data;
//					if(  MRenderView::doesRenderEditorExist()  )
//					{
//						MRenderView::updatePixels(qEvent.tile_xmin, qEvent.tile_xmax, qEvent.tile_ymin, qEvent.tile_ymax, pixels);
//						MRenderView::refresh(qEvent.tile_xmin, qEvent.tile_xmax, qEvent.tile_ymin, qEvent.tile_ymax);
//					}
//					delete[] pixels;
//				}
//			}
//		}
//	}
//	return true;
//}
//
//
////
////	this thread calls the real render process and waits until it is finished
////
//
//bool MayaScene::renderThread(MayaScene *scene)
//{
//	scene->renderThreadWorker();
//
//	EventQueue::Event qEvent;
//	qEvent.data = NULL;
//	qEvent.type = EventQueue::Event::RENDERDONE;
//	EventQueue::theQueue()->push(qEvent);
//	return true;
//}
//
//
////
////
////	render image process:
////	here we start the render process in two flavours, as IPR interactive rendering or as normal, non-interactive rendering.
////  Both versions start a seperate render thread whereas the interactive one will be non blocking, the second one will be blocking
////	and waiting until rendering is finished.
////	
////
//
//bool MayaScene::renderImageProcess()
//{
//
//	// first start the event worker, it will prepare the event queue and wait for entries
//	boost::thread eventWorkerThread_tread(MayaScene::eventWorkerThread, this);	
//
//	// then start the render thread itself. It will put events into the queue at least the RENDERDONE event
//	boost::thread renderThread_tread(MayaScene::renderThread, this);	
//
//	// if not ipr: eventWorkerThread_tread.join() ...renderThread_tread.join()
//	// if ipr detach both.
//
//	return true;
//}
//
