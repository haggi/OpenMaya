#include "mtco_swatchRenderer.h"


MSwatchRenderBase* mtco_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mtco_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mtco_SwatchGenerator::mtco_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mtco_SwatchGenerator::~mtco_SwatchGenerator()
{

}

bool mtco_SwatchGenerator::doIteration()
{
	return true;
}
