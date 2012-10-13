#ifndef DUMMY_RENDERER_H
#define DUMMY_RENDERER_H

#include <maya/MImage.h>
#include "../threads/queue.h"

struct Pixel{
	unsigned char r, g, b, a;
};

class CallbackClass
{
public:
	virtual void tileDoneCallback(int x, int y, int width, int height, Pixel *pixels) = 0;
	virtual void tilePreCallback(int x, int y, int width, int height, Pixel *pixels) = 0;
	virtual void frameDoneCallback(int width, int height, Pixel *pixels) = 0;
	virtual void scanlineDoneCallback(int width, int height, Pixel *pixels) = 0;
	virtual void progressiveImageDoneCallback(int width, int height, Pixel *pixels) = 0;
	virtual void iprImageDoneCallback(int width, int height, Pixel *pixels) = 0;
	virtual void processCallback() = 0;
};

struct RenderMethods{
	enum RenderMethod{
		SCANLINE = 0,
		TILEBASED = 1,
		FRAMEBASED = 2,
		PROGRESSIVE = 3,
		IPR = 4,
		NONE = 5
	};
};

struct Interrupts{
	enum Interrupt{
		RESTART = 0,
		REFRESH = 1,
		FINISH = 2,
		NONE = 3
	};
};

static Interrupts::Interrupt interrupt = Interrupts::NONE;

class DummyRenderer
{
public:
	DummyRenderer();
	~DummyRenderer();

	void render(RenderMethods::RenderMethod method);

	CallbackClass *callbackClass;
	int width, height;
	Pixel *pixels;
	Pixel updateColor;
	RenderMethods::RenderMethod renderMethod;
	int tileSize, tilesX, tilesY;
	Pixel px[640][480];


private:
	void doFrameBasedRendering();
	void doTileBasedRendering();
	void doProgressiveRendering();
	MImage mImage;
};

void setInterrupt(Interrupts::Interrupt type);

#endif
