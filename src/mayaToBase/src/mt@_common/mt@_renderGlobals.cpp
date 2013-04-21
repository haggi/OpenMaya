#include "mt@_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mt@_RenderGlobals::mt@_RenderGlobals()
{
	this->getMt@Globals();
}

MString mt@_RenderGlobals::getImageExt()
{
	return MString("ext");
}

bool mt@_RenderGlobals::getMt@Globals()
{
	logger.debug("mt@_RenderGlobals::getMt@Globals");

	MSelectionList @GlobalsList;
	@GlobalsList.add("@Globals");

	if( @GlobalsList.length() == 0 )
	{
		logger.debug("@Globals not found. Stopping.");
		return false;
	}
	MObject node;
	@GlobalsList.getDependNode(0, node);
	MFnDependencyNode @Globals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), @Globals, this->translatorVerbosity))
			throw("problem reading @Globals.translatorVerbosity");
		switch(this->translatorVerbosity)
		{
		case 0:
			logger.setLogLevel(Logging::Info);
			break;
		case 1:
			logger.setLogLevel(Logging::Error);
			break;
		case 2:
			logger.setLogLevel(Logging::Warning);
			break;
		case 3:
			logger.setLogLevel(Logging::Progress);
			break;
		case 4:
			logger.setLogLevel(Logging::Debug);
			break;
		}

//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 
		
		if(!getString(MString("optimizedTexturePath"), LuxGlobals, this->optimizedTexturePath))
			throw("problem reading luxGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), LuxGlobals, this->basePath))
			throw("problem reading luxGlobals.basePath");

		if(!getString(MString("imagePath"), LuxGlobals, this->imagePath))
			throw("problem reading luxGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), LuxGlobals, this->exportSceneFile))
			throw("problem reading luxGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), LuxGlobals, this->exportSceneFileName))
			throw("problem reading luxGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), LuxGlobals, this->imageName))
			throw("problem reading luxGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), LuxGlobals, this->adaptiveSampling))
			throw("problem reading luxGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), LuxGlobals, this->doMb))
			throw("problem reading LuxGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), LuxGlobals, this->doDof))
			throw("problem reading LuxGlobals.doDof");

		if(!getBool(MString("doDof"), LuxGlobals, this->doDof))
			throw("problem reading LuxGlobals.doDof");

		if(!getFloat(MString("sceneScale"), LuxGlobals, this->sceneScale))
			throw("problem reading LuxGlobals.sceneScale");

		this->sceneScaleMatrix.setToIdentity();
		this->sceneScaleMatrix.matrix[0][0] = this->sceneScale;
		this->sceneScaleMatrix.matrix[1][1] = this->sceneScale;
		this->sceneScaleMatrix.matrix[2][2] = this->sceneScale;

	}catch(char *errorMsg){

		logger.error(errorMsg);
		this->good = false;
		return false;	
	}
	return true;	

}
