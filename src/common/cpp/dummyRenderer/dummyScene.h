#ifndef DUMMY_SCENE_H
#define DUMMY_SCENE_H
#include <maya/MRenderView.h>

#include "dummyRenderer.h"

struct RGlobal
{
	int width;
	int height;
	RGlobal(){
		width = 640;
		height = 480;
	};
};


class MScene{
public:
	RGlobal *renderGlobals;
	enum RType{
		NORMAL,
		IPR
	};
	RType type;
	MScene();
	DummyRenderer *renderer;
	bool isRendering;
	static boost::thread renderThread;
	static void doRender(MScene *);
	void render();
	void interruptRender();
	~MScene(){};
};


class myCallbackClass : public CallbackClass
{
	void tileDoneCallback(int x, int y, int width, int height, Pixel *pixels);
	void tilePreCallback(int x, int y, int width, int height, Pixel *pixels);
	void frameDoneCallback(int width, int height, Pixel *pixels);
	void scanlineDoneCallback(int width, int height, Pixel *pixels);
	void progressiveImageDoneCallback(int width, int height, Pixel *pixels);
	void iprImageDoneCallback(int width, int height, Pixel *pixels);
	void processCallback();	
};



#endif