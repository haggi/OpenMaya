#include <iostream>
#include <time.h>
#include <maya/MString.h>
#include <SDK/Integration/sdk.h>
#include <SDK/Integration/sdk.plus.h>
#include <SDK/XML/sdk.xml.importer.h>

#include "renderGlobals.h"
#include "utilities/logging.h"
#include "world.h"

static Logging logger;

#include "Thea.h"

#define __SAVE_IMAGE_PERIOD__ 60 // save temporary image every 60 seconds


void renderend(void *info)
{
	volatile bool *isrendering=(volatile bool *)info;
	*isrendering=false;
}


void StopRenderTest()
{
	char phase[100];
	TheaSDK::Precision progress=(TheaSDK::Precision)TheaSDK::GetProgress(phase,100);
	if (progress>0.05f && strncmp(phase,"Ray Tracing",strlen("Ray Tracing"))==0)
		TheaSDK::StopRendering(true);
}


void PauseRenderTest()
{
	static int pauseState=0;
	if (pauseState>=0)
	{
		char phase[100];
		TheaSDK::Precision progress=(TheaSDK::Precision)TheaSDK::GetProgress(phase,100);
		if (pauseState==0)
		{
		// pause render.
			if (progress>0.05f && strncmp(phase,"Ray Tracing",strlen("Ray Tracing"))==0)
			{
				TheaSDK::PauseRendering();
				pauseState=1;
				std::cout << "Render paused! Please wait";
				std::cout.flush();
			}
		}
		else
		{
		// continue after 10 tries.
			std::cout << ".";
			if (pauseState++>=10)
			{
				TheaSDK::ContinueRendering();
				pauseState=-1;
				std::cout << " Continuing!\n";
			}
			std::cout.flush();
		}
	}
}


void SaveTemporaryImage(time_t &startTimer)
{
#ifdef __SAVE_IMAGE_PERIOD__
	time_t endTimer;
	time(&endTimer);
	if (endTimer-startTimer>__SAVE_IMAGE_PERIOD__)
	{
		TheaSDK::SaveImage("C:/daten/3dprojects/mayaToThea/images/outputtemp.png");
		startTimer=endTimer;
	}
#endif
}


void TheaRenderer::doTestRender()
{
	MFnDependencyNode gFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	std::shared_ptr<MayaScene> scene = MayaTo::getWorldPtr()->worldScenePtr;

	const char *filename = "H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToThea/mtth_devmodule/ressources/CornellBox.xml";

	if (TheaSDK::Init()==false)
	{
		std::cout << "Init failed\n";
		return;
	}
	std::cout << "Successfully linked with Thea (version: " << TheaSDK::GetVersion() << ")\n";

	if (TheaSDK::OpenScene(filename)==false)
	{
		std::cout << "Error opening input filename \"" << filename << "\"\n";
		TheaSDK::Shutdown();
		return;
	}
	int width, height;
	renderGlobals->getWidthHeight(width, height);
	MString imgSize = MString() + width + "x" + height;
	Logging::info(MString("img size: ") + imgSize);
	if (TheaSDK::SetActiveNthCamera(0,imgSize.asChar())==false)
	{
		std::cout << "Can not set first active camera, perhaps there are no cameras defined!\n";
		TheaSDK::Shutdown();
		return;
	}
	std::cout.flush();


	time_t startTimer;
	time(&startTimer);

// another loop that ends with a callback!
	volatile bool isrendering=true;

	if (TheaSDK::StartRendering( renderend,(void *)&isrendering,true,false)==false)
	{
		std::cout << "Error starting rendering thread... Aborting!\n";
		TheaSDK::Shutdown();
		return;
	}

	do
	{
		//StopRenderTest();
		//PauseRenderTest();
		TheaSDK::Sleep(1000);

		char phase[200]="";
		double p=TheaSDK::GetProgress(phase,200);
		if (phase[0]!='\0') // print only if phase has been set.
		{
			std::cout << phase << " " << int(p*100) << "." << int(p*10000)%100 << "%\n";
		}
		
		if( TheaSDK::CheckImageUpdate())
		{
			this->frameBufferCallback();
		}
		SaveTemporaryImage(startTimer);
	}
	while (isrendering);


	if (TheaSDK::SaveImage("C:/daten/3dprojects/mayaToThea/images/output.jpg")==false)
	{
		std::cout << "Warning: can not save output image.\n";
		TheaSDK::Shutdown();
		return;
	}

	TheaSDK::Shutdown();
	return;
}
