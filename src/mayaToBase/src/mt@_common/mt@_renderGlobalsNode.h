#ifndef mt@_GLOBALS_H
#define mt@_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaTo@GlobalsName = "@Globals";

class MayaTo@Globals : public MayaRenderGlobalsNode
{
public:
						MayaTo@Globals();
	virtual				~MayaTo@Globals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 

		if(!getString(MString("optimizedTexturePath"), @Globals, this->optimizedTexturePath))
			throw("problem reading @Globals.optimizedTexturePath");

		if(!getString(MString("basePath"), @Globals, this->basePath))
			throw("problem reading @Globals.basePath");

		if(!getString(MString("imagePath"), @Globals, this->imagePath))
			throw("problem reading @Globals.imagePath");

		if(!getString(MString("imageName"), @Globals, this->imageName))
			throw("problem reading @Globals.imageName");

		if(!getBool(MString("adaptiveSampling"), @Globals, this->adaptiveSampling))
			throw("problem reading @Globals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), @Globals, this->doMb))
			throw("problem reading @Globals.doMotionBlur");

		if(!getBool(MString("doDof"), @Globals, this->doDof))
			throw("problem reading @Globals.doDof");

		if(!getBool(MString("doDof"), @Globals, this->doDof))
			throw("problem reading @Globals.doDof");

		if(!getFloat(MString("sceneScale"), @Globals, this->sceneScale))
			throw("problem reading @Globals.sceneScale");

};

#endif