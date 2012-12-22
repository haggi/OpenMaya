
#include <iostream>

#include "threads/renderQueueWorker.h"
#include "../mtkr_common/mtkr_mayaObject.h"
#include "../mtkr_common/mtkr_mayaScene.h"
#include "../mtkr_common/mtkr_renderGlobals.h"

#include "kraysdk/proto/direct.h"
#include "kraysdk/SharedSources/libraryDir.h"
#include "kraysdk/kray.h"

#include "krayRenderer.h"
#include "krayEventListener.h"

#include "krayTestScene.h"
#include "utilities/logging.h"

static Logging logger;

namespace krayRender
{
	KrayRenderer::KrayRenderer()
	{
		EventListener listener;
		const char *libpath = "C:/Users/haggi/coding/OpenMaya/src/mayaToKray/mtkr_devmodule/bin/";
		Kray::Library klib(libpath);				// here we start with KrayLib object
		//Kray::Library klib(KRAY_DIRECTORY);				// here we start with KrayLib object
		//Kray::Instance* kinst=klib.createInstance();	// then we create Kray instance, each instance have its own scene and settings
		//klib.createInstanceWithHandler()
		Kray::Instance* kinst = klib.createInstanceWithListner(&listener);
		this->prot = NULL;
		if (kinst) // check if instance was created, if not, Kray library wasn't found
		{				
			this->prot = new Kray::DirectPrototyper(kinst);		// prototyper gives access to Kray Script command prototypes
		}else{
			logger.error("Unable to create kray renderer instance");
		}
	}

	KrayRenderer::~KrayRenderer()
	{
	}

	void KrayRenderer::updateTransform(mtkr_MayaObject *obj)
	{
		logger.debug(MString("KrayRenderer::updateTransform ") + obj->shortName);
	}

	void KrayRenderer::updateDeform(mtkr_MayaObject *obj)
	{
		logger.debug(MString("KrayRenderer::updateDeform ") + obj->shortName);



	}

	void KrayRenderer::render()
	{	
		//EventListener listener;
		//const char *libpath = "C:/Users/haggi/coding/OpenMaya/src/mayaToKray/mtkr_devmodule/bin/";
		//Kray::Library klib(libpath);				// here we start with KrayLib object
		////Kray::Library klib(KRAY_DIRECTORY);				// here we start with KrayLib object
		////Kray::Instance* kinst=klib.createInstance();	// then we create Kray instance, each instance have its own scene and settings
		////klib.createInstanceWithHandler()
		//Kray::Instance* kinst = klib.createInstanceWithListner(&listener);

		//if (kinst) // check if instance was created, if not, Kray library wasn't found
		//{	
		if( this->prot )
		{
			//Kray::DirectPrototyper prot(kinst);		// prototyper gives access to Kray Script command prototypes


			setupSimpleMeshScene(*prot);
			
			// define image size
			int width = this->mtkr_renderGlobals->imgWidth;
			int height = this->mtkr_renderGlobals->imgHeight;
			prot->frameSize(width, height);
			// define camera
			prot->echo("Rendering....");
			prot->render();
			prot->echo("Pause to see render result....");
			prot->pause(2);	// wait two secs before closing render window
			prot->echo("Saving image....");
			prot->outputSave_tif("C:/daten/3dprojects/kray/images/kray.tif");
		}else{
			std::cout << "No prot" << std::endl;
		}
	
		EventQueue::Event e;
		e.data = NULL;
		e.type = EventQueue::Event::FRAMEDONE;
		theRenderEventQueue()->push(e);

	}

} // namespace kray