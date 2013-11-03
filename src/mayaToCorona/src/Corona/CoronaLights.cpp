#include "Corona.h"

void CoronaRenderer::defineLights()
{
	Corona::Sun sun;
	sun.active = true;
	sun.dirTo = Corona::Dir(0, 8, 5).getNormalized();
	sun.color = Corona::Rgb(100,100,100);
	sun.visibleDirect = true;
	sun.visibleReflect = true;
	sun.visibleRefract = true;
	sun.sizeMultiplier = 2.0f;
	this->context.scene->getSun() = sun;
}
