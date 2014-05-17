#include "mtth_swatchRenderer.h"


MSwatchRenderBase* mtth_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mtth_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mtth_SwatchGenerator::mtth_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mtth_SwatchGenerator::~mtth_SwatchGenerator()
{

}

bool mtth_SwatchGenerator::doIteration()
{
	return true;
}
