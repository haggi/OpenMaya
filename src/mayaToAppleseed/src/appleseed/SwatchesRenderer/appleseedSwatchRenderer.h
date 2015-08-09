#ifndef MTAP_NEWSWATCHRENDER_H
#define MTAP_NEWSWATCHRENDER_H

class AppleseedSwatchRenderer
{
public:
	AppleseedSwatchRenderer();
	~AppleseedSwatchRenderer();
	void mainLoop();
	bool terminateLoop = false;
	bool enableSwatchRenderer = true;
	bool loopDone = false;

	static void startAppleseedSwatchRender(AppleseedSwatchRenderer *swRend);
	static void terminateAppleseedSwatchRender(AppleseedSwatchRenderer *swRend);

};


#endif