#ifndef MTM_GLOBALS_H
#define MTM_GLOBALS_H

#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <maya/MPxNode.h>
#include <mayarendernodes/renderGlobalsNode.h>

const MString mayaToMantraGlobalsName = "mantraGlobals";

class mayaToMantraGlobals :  public MayaRenderGlobalsNode
{
public:
						mayaToMantraGlobals();
	virtual				~mayaToMantraGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
	static    MObject detectShapeDeform;
	static    MObject scaleFactor;
	static    MObject output;
	static    MObject singleFileOutput;
	static    MObject motionblur;
	static    MObject imagemotionblur;
	static    MObject xftimesamples;
	static    MObject geotimesamples;
	static    MObject motionfactor;
	static    MObject mbtype;
	static    MObject dof;
	static    MObject samples;
	static    MObject samplelock;
	static    MObject jitter;
	static    MObject noiselevel;
	static    MObject randomseed;
	static    MObject rayvariance;
	static    MObject minraysamples;
	static    MObject maxraysamples;
	static    MObject volumestepsize;
	static    MObject decoupleshadowstep;
	static    MObject shadowstepsize;
	static    MObject filtertype;
	static    MObject filtersizeuv;
	static    MObject bitdepth;
	static    MObject renderengine;
	static    MObject tilesize;
	static    MObject opacitylimit;
	static    MObject verbosity;
	static    MObject translatorVerbosity;
	static    MObject usemaxproc;
	static    MObject threadcount;
	static    MObject accerationtype;
	static    MObject kdmemfac;
	static    MObject geocachesize;
	static    MObject texcachesize;
	static    MObject renderviewcam;
	static    MObject autogenenvmap;
	static    MObject autogensmap;
	static    MObject reflectlimit;
	static    MObject refractlimit;
	static    MObject glossylimit;
	static    MObject diffuselimit;
	static    MObject volumelimit;
	static    MObject raytracebias;
	static    MObject biasalongnormal;
	static    MObject colorspace;
	static    MObject raylimit;
	static    MObject smoothgrid;
	static    MObject colorlimit;
	static    MObject minreflectratio;

	static    MObject basePath;
	static    MObject imagePath;
	static    MObject imageName;

	static	  MObject geoFileType;
};

#endif
