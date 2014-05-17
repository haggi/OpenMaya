#ifndef mtth_SWATCH_RENDERER
#define mtth_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mtth_SwatchGenerator : public MSwatchRenderBase
{

public:
   mtth_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mtth_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif