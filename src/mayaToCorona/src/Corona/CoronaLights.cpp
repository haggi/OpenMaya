#include "Corona.h"

void CoronaRenderer::defineLights()
{

	Corona::Sun sun = Corona::Sun();
	sun.active = true;
	sun.dirTo = Corona::Dir(0, 8, 5).getNormalized();
	sun.color = Corona::Rgb(1,1,1);
	this->context.scene->getSun() = sun;

}
