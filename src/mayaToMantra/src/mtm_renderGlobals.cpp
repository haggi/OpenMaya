#include <maya/MSelectionList.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MRenderUtil.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MAnimControl.h>
#include <maya/MGlobal.h>
#include <maya/MFnRenderLayer.h>

#include "mtm_renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

bool mtm_RenderGlobals::getMtmGlobals()
{
	MSelectionList mantraGlobalsList;
	MObject node;

	MCommonRenderSettingsData data;
	MRenderUtil::getCommonRenderSettings(data);

	mantraGlobalsList.add("mayaToMantraGlobals");
	if( mantraGlobalsList.length() == 0 )
		return false;

	mantraGlobalsList.getDependNode(0, node);
	MFnDependencyNode fnMantraGlobals(node);

	logger.info("mtm_RenderGlobals::getMtmGlobals");
	
	try{

		if(!getInt(MString("translatorVerbosity"), fnMantraGlobals, this->translatorVerbosity))
			throw("problem reading mayaToMantraGlobals.translatorVerbosity");

		if(this->translatorVerbosity <= 4)
		{
			if( this->translatorVerbosity == 0)
				Logging::setLogLevel(Logging::Error);
			if( this->translatorVerbosity == 1)
				Logging::setLogLevel(Logging::Warning);
			if( this->translatorVerbosity == 2)
				Logging::setLogLevel(Logging::Info);
			if( this->translatorVerbosity == 3)
				Logging::setLogLevel(Logging::Progress);
			if( this->translatorVerbosity == 4)
				Logging::setLogLevel(Logging::Debug);
		}else{
			Logging::setLogLevel(Logging::Debug);
		}

		if(!getString(MString("basePath"), fnMantraGlobals, this->basePath))
			throw("problem reading mayaToMantraGlobals.basePath");

		if(!getString(MString("imagePath"), fnMantraGlobals, this->imagePath))
			throw("problem reading mayaToMantraGlobals.imagePath");

		if(!getString(MString("imageName"), fnMantraGlobals, this->imageName))
			throw("problem reading mayaToMantraGlobals.imageName");

		MObject renderLayer = MFnRenderLayer::currentLayer();
		this->imageOutputFile = data.getImageName(data.kFullPathImage, 0, this->imageName, MString(""), MString("tif"), renderLayer); 

		if(!getEnum(MString("output"), fnMantraGlobals, this->outputType))
			throw("problem reading mayaToMantraGlobals.output");

		if(!getInt2(MString("samples"), fnMantraGlobals, this->samples))
			throw("problem reading mayaToMantraGlobals.samples");

		if(!getString(MString("verbosity"), fnMantraGlobals, this->verbosity))
			throw("problem reading mayaToMantraGlobals.verbosity");
		
		if(!getBool(MString("motionblur"), fnMantraGlobals, this->doMb))
			logger.error(MString("problem reading mayaToMantraGlobals.motionblur"));

		if(!getInt(MString("xftimesamples"), fnMantraGlobals, this->xftimesamples))
			throw("problem reading mayaToMantraGlobals.xftimesamples");

		if(!getInt(MString("geotimesamples"), fnMantraGlobals, this->geotimesamples))
			throw("problem reading mayaToMantraGlobals.geotimesamples");

		if(!getEnum(MString("mbtype"), fnMantraGlobals, this->mbtype))
			throw("problem reading mayaToMantraGlobals.mbtype");

		if(!getEnum(MString("renderengine"), fnMantraGlobals, this->renderengine))
			throw("problem reading mayaToMantraGlobals.renderengine");

		if(!getBool(MString("detectShapeDeform"), fnMantraGlobals, this->detectShapeDeform))
			throw("problem reading mayaToMantraGlobals.detectShapeDeform");

		if(!getFloat(MString("scaleFactor"), fnMantraGlobals, this->scaleFactor))
			throw("problem reading mayaToMantraGlobals.scaleFactor");

		if(!getBool(MString("dof"), fnMantraGlobals, this->doDof))
			throw("problem reading mayaToMantraGlobals.dof");

		if(!getInt(MString("bitdepth"), fnMantraGlobals, this->bitdepth))
			throw("problem reading mayaToMantraGlobals.bitdepth");

		if(!getFloat(MString("jitter"), fnMantraGlobals, this->jitter))
			throw("problem reading mayaToMantraGlobals.jitter");

		if(!getInt(MString("refractlimit"), fnMantraGlobals, this->refractlimit))
			throw("problem reading mayaToMantraGlobals.refractlimit");

		if(!getInt(MString("reflectlimit"), fnMantraGlobals, this->reflectlimit))
			throw("problem reading mayaToMantraGlobals.reflectlimit");

		if(!getInt(MString("volumelimit"), fnMantraGlobals, this->volumelimit))
			throw("problem reading mayaToMantraGlobals.volumelimit");

		if(!getFloat(MString("opacitylimit"), fnMantraGlobals, this->opacitylimit))
			throw("problem reading mayaToMantraGlobals.opacitylimit");

		if(!getInt(MString("minraysamples"), fnMantraGlobals, this->minRaySamples))
			throw("problem reading mayaToMantraGlobals.minraysamples");

		if(!getInt(MString("maxraysamples"), fnMantraGlobals, this->maxRaySamples))
			throw("problem reading mayaToMantraGlobals.maxraysamples");

		if(!getFloat(MString("noiselevel"), fnMantraGlobals, this->noiselevel))
			throw("problem reading mayaToMantraGlobals.noiselevel");

		if(!getInt(MString("randomseed"), fnMantraGlobals, this->randomseed))
			throw("problem reading mayaToMantraGlobals.randomseed");

		if(!getBool(MString("rayvariance"), fnMantraGlobals, this->rayvariance))
			throw("problem reading mayaToMantraGlobals.rayvariance");

		// 0 = binary, 1 = ascii so switch
		if(!getBool(MString("geoFileType"), fnMantraGlobals, this->binaryGeoExport))
			throw("problem reading mayaToMantraGlobals.geoFileType");
		else{
			this->binaryGeoExport = !this->binaryGeoExport;
		}

		if(!getFloat(MString("raytracebias"), fnMantraGlobals, this->raytracebias))
			throw("problem reading mayaToMantraGlobals.raytracebias");

		if(!getBool(MString("biasalongnormal"), fnMantraGlobals, this->biasalongnormal))
			throw("problem reading mayaToMantraGlobals.biasalongnormal");

		if(!getInt(MString("accerationtype"), fnMantraGlobals, this->accelerationType))
			throw("problem reading mayaToMantraGlobals.accelerationType");

		if(!getFloat(MString("colorspace"), fnMantraGlobals, this->colorspace))
			throw("problem reading mayaToMantraGlobals.colorspace");

		if(!getInt(MString("raylimit"), fnMantraGlobals, this->raylimit))
			throw("problem reading mayaToMantraGlobals.raylimit");

		if(!getFloat(MString("colorlimit"), fnMantraGlobals, this->colorlimit))
			throw("problem reading mayaToMantraGlobals.colorlimit");

		if(!getFloat(MString("minreflectratio"), fnMantraGlobals, this->minreflectratio))
			throw("problem reading mayaToMantraGlobals.minreflectratio");

		if(!getInt(MString("threadcount"), fnMantraGlobals, this->numThreads))
			throw("problem reading mayaToMantraGlobals.numThreads");

		if(!getInt(MString("tilesize"), fnMantraGlobals, this->tilesize))
			throw("problem reading mayaToMantraGlobals.tilesize");

		if(!getFloat(MString("kdmemfac"), fnMantraGlobals, this->kdmemfac))
			throw("problem reading mayaToMantraGlobals.kdmemfac");

	}catch(char *errorMsg){

		logger.error(errorMsg);
		this->good = false;
		return false;	
	}
	this->good = true;
	return true;
}

mtm_RenderGlobals::mtm_RenderGlobals()
{
	this->getMtmGlobals();
	// if there is no renderengine given, the micropolygon renderer is used automatically
	this->renderengine = 0;
	renderEngines.append("");
	renderEngines.append("raytrace");
	renderEngines.append("pbrmicropoly");
	renderEngines.append("pbrraytrace");
	renderEngines.append("photon");
}