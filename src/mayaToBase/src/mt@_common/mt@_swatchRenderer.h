#ifndef mt@_SWATCH_RENDERER
#define mt@_SWATCH_RENDERER

#include <maya/MSwatchRenderBase.h> 

class mt@_SwatchGenerator : public MSwatchRenderBase
{

public:
   mt@_SwatchGenerator(MObject dependNode, MObject renderNode, int imageResolution);
   ~mt@_SwatchGenerator();

   static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);

   virtual bool doIteration();

};


#endif