#ifndef mtfu_GLOBALS_H
#define mtfu_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToFujiGlobalsName = "FujiGlobals";

class MayaToFujiGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToFujiGlobals();
	virtual				~MayaToFujiGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 

		if(!getString(MString("optimizedTexturePath"), FujiGlobals, this->optimizedTexturePath))
			throw("problem reading FujiGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), FujiGlobals, this->basePath))
			throw("problem reading FujiGlobals.basePath");

		if(!getString(MString("imagePath"), FujiGlobals, this->imagePath))
			throw("problem reading FujiGlobals.imagePath");

		if(!getString(MString("imageName"), FujiGlobals, this->imageName))
			throw("problem reading FujiGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), FujiGlobals, this->adaptiveSampling))
			throw("problem reading FujiGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), FujiGlobals, this->doMb))
			throw("problem reading FujiGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), FujiGlobals, this->doDof))
			throw("problem reading FujiGlobals.doDof");

		if(!getBool(MString("doDof"), FujiGlobals, this->doDof))
			throw("problem reading FujiGlobals.doDof");

		if(!getFloat(MString("sceneScale"), FujiGlobals, this->sceneScale))
			throw("problem reading FujiGlobals.sceneScale");

};

#endif