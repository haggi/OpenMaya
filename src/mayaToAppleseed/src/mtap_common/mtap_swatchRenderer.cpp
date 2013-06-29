#include "mtap_swatchRenderer.h"


MSwatchRenderBase* mtap_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mtap_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mtap_SwatchGenerator::mtap_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mtap_SwatchGenerator::~mtap_SwatchGenerator()
{

}

bool mtap_SwatchGenerator::doIteration()
{
	return true;
}
