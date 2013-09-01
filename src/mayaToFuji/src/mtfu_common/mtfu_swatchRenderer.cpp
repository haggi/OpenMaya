#include "mtfu_swatchRenderer.h"


MSwatchRenderBase* mtfu_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mtfu_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mtfu_SwatchGenerator::mtfu_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mtfu_SwatchGenerator::~mtfu_SwatchGenerator()
{

}

bool mtfu_SwatchGenerator::doIteration()
{
	return true;
}
