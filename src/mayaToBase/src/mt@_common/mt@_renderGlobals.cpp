#include "mt@_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mt@_RenderGlobals::mt@_RenderGlobals()
{
	this->getMtapGlobals();
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
