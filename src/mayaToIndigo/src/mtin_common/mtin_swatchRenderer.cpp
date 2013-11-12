#include "mtin_swatchRenderer.h"


MSwatchRenderBase* mtin_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mtin_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mtin_SwatchGenerator::mtin_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mtin_SwatchGenerator::~mtin_SwatchGenerator()
{

}

bool mtin_SwatchGenerator::doIteration()
{
	return true;
}
