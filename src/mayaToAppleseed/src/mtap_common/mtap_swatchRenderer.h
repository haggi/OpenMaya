#ifndef MTAP_SWATCH_RENDERER
#define MTAP_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mtap_SwatchGenerator : public MSwatchRenderBase
{

public:
   mtap_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mtap_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif