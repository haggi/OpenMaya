#ifndef mtco_SWATCH_RENDERER
#define mtco_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mtco_SwatchGenerator : public MSwatchRenderBase
{

public:
   mtco_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mtco_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif