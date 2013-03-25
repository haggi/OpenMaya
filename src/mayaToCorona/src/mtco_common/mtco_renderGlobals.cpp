#include "mtco_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtco_RenderGlobals::mtco_RenderGlobals()
{
	this->getMtapGlobals();
}


bool mtco_RenderGlobals::getMtCoronaGlobals()
{
	logger.debug("mtco_RenderGlobals::getMtCoronaGlobals");

	MSelectionList CoronaGlobalsList;
	CoronaGlobalsList.add("CoronaGlobals");

	if( CoronaGlobalsList.length() == 0 )
	{
		logger.debug("CoronaGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	CoronaGlobalsList.getDependNode(0, node);
	MFnDependencyNode CoronaGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), CoronaGlobals, this->translatorVerbosity))
			throw("problem reading CoronaGlobals.translatorVerbosity");
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
		if(!getInt(MString("samplingSubdivAA"), coronaGlobals, this->samplingSubdivAA));
		throw("problem reading coronaGlobals.samplingSubdivAA");

		if(!getInt(MString("samplingAdaptiveSteps"), coronaGlobals, this->samplingAdaptiveSteps));
		throw("problem reading coronaGlobals.samplingAdaptiveSteps");

		if(!getFloat(MString("samplingAdaptiveThresh"), coronaGlobals, this->samplingAdaptiveThresh));
		throw("problem reading coronaGlobals.samplingAdaptiveThresh");

		if(!getEnum(MString("environmentType"), coronaGlobals, this->environmentType));
		throw("problem reading coronaGlobals.environmentType");

		if(!getColor(MString("environmentColor"), coronaGlobals, this->environmentColor));
		throw("problem reading coronaGlobals.environmentColor");

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
