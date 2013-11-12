#ifndef mtin_SWATCH_RENDERER
#define mtin_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mtin_SwatchGenerator : public MSwatchRenderBase
{

public:
   mtin_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mtin_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif