#ifndef mtfu_SWATCH_RENDERER
#define mtfu_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mtfu_SwatchGenerator : public MSwatchRenderBase
{

public:
   mtfu_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mtfu_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif