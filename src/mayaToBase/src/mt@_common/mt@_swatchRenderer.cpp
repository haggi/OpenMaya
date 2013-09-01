#include "mt@_swatchRenderer.h"


MSwatchRenderBase* mt@_SwatchGenerator::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
   return new mt@_SwatchGenerator(dependNode, renderNode, imageResolution);
}

mt@_SwatchGenerator::mt@_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution):MSwatchRenderBase(dependNode, renderNode, imageResolution)
{

}

mt@_SwatchGenerator::~mt@_SwatchGenerator()
{

}

bool mt@_SwatchGenerator::doIteration()
{
	return true;
}
