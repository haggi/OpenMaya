#include <maya/MGlobal.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MLightLinks.h>
#include <maya/MDagPathArray.h>

#include "mtm_mayaScene.h"
#include "mtm_mayaObject.h"
#include "mtm_renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "shadingTools/mtm_material.h"

static Logging logger;

void mtm_MayaScene::transformUpdateCallback(MayaObject& obj)
{
}
void mtm_MayaScene::deformUpdateCallback(MayaObject& obj)
{
}



bool mtm_MayaScene::renderImage()
{
	return true;
}

MayaObject* mtm_MayaScene::mayaObjectCreator(MObject& mobject)
{
	return new mtm_MayaObject(mobject);
}

void mtm_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtm_MayaObject *mtm_obj = (mtm_MayaObject *)obj;
	delete mtm_obj;
	obj = NULL;
}

void mtm_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtm_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;

}

mtm_MayaScene::mtm_MayaScene()
{
	getRenderGlobals();
	this->good = true;
	this->outFile = NULL;
	this->defaultCamera = NULL;
	this->needsLightCategories = false;

	if( !this->renderGlobals->good)
	{
		this->good = false;
		return;
	}
	
	// Adding env light to be able to identify a new light type
	this->lightIdentifier.push_back(0x106EF1); // mtm_envLight
	this->objectIdentifier.push_back(0x106EF2); // standin object
}

mtm_MayaScene::~mtm_MayaScene()
{}

bool mtm_MayaScene::parseScene()
{
	MayaScene::parseScene();
	// translate shapes not necessary I think
	//this->translateShapes(0);

	// check lights for geometry lights

	return true;
}

// if doAnimation is on and doMb is on and currentFrame is not first frame, I suppose we are in a motion step
// if doAnimation is off and doMb is on, we are in a motion step
bool mtm_MayaScene::isMotionStep()
{
	logger.debug("mtm_MayaScene::isMotionStep");
	// no mb no mbstep
	if(!this->renderGlobals->doMb)
		return false;

	// if we are not at very first mb step, we are on a mb step
	if( this->renderGlobals->currentMbStep != 0)
		return true;

	bool isStartFrame = this->renderGlobals->currentFrameNumber == this->renderGlobals->startFrame;

	// we are at mb step 0, but this is not the first sequence frame, so it is a mb step like 9.8 instead of 10.0
	if( !isStartFrame && (this->renderGlobals->currentMbStep == 0))
		return true;

	return false;
}

bool mtm_MayaScene::sceneNeedsUpdate()
{
	if( this->isMotionStep())
		return true;

	logger.debug("mtm_MayaScene::sceneNeedsUpdate");
	// if we render from the UI, then we only need a scene update if we have motionblur turned on
	if( !this->renderGlobals->inBatch )
		return false;

	bool isStartFrame = this->renderGlobals->currentFrameNumber == this->renderGlobals->startFrame;
	// very first frame is already defined
	if(!isStartFrame)
		return true;

	return false;
}
// here all objects will be updated for the current frame step, 
// this includes export shape on demand for a deforming object,
// transformation matrix etc.
//
// This procedure will be called at least once if in an animation or motionblur or not.

bool mtm_MayaScene::updateScene()
{
	logger.debug(MString("mtm_MayaScene::updateScene"));

	//if( !this->sceneNeedsUpdate() )
	//{
	//	logger.debug("mtm_MayaScene::updateScene() No Scene update needed.");
	//	return true;
	//}
	
	int numElements = (int)this->objectList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->objectList[objId];

		// not all object types are supported...
		if( !obj->geometryShapeSupported() )
			continue;

		// if this is not the very first time step of a rendering
		// we must be in a motionblur step
		// isMbStartStep will be always true in an animation without motionblur
		if( !this->renderGlobals->isMbStartStep )
		{
			if( !obj->motionBlurred )
			{
				continue;
			}
		}
				
		// clear transformation matrices if we are at very first mb step
		// we can have xform and transform steps for the same timeframe
		// currentMbStep is only 0 for the very first step
		if( this->renderGlobals->currentMbStep == 0 )
		{
			obj->transformMatrices.clear();			
		}

		// even if we dont have motionblur a default dummy mb element is added with type Both
		if( this->renderGlobals->isTransformStep() )
		{
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
		}

		// if we are here at the very first motionBlur step, then remove export filesNames, export filenames will be automatically filled by the exporter
		// at start frame, always clear
		// not at start frame only clear if shapedeformation is detected
		if( this->renderGlobals->currentMbStep == 0 )
		{
			if( this->renderGlobals->currentFrameNumber != this->renderGlobals->startFrame )
			{
				if( obj->shapeConnected && this->renderGlobals->detectShapeDeform)
				{
					obj->exportFileNames.clear();
				}
			}else{
				obj->exportFileNames.clear();
			}
		}
		
		// export shape always the first time no matter which type of mbstep we have
		if( this->renderGlobals->isMbStartStep )
		{
			if( (!obj->isInstancerObject) && 
				( obj->instanceNumber == 0))
			{
				if( obj->exportFileNames.size() == 0)
				{
					obj->exportShape(this->renderGlobals->currentMbStep, this->renderGlobals->binaryGeoExport, this->renderGlobals->basePath, this->renderGlobals->useShortNames);
				}
			}
		}else{
			// only export if shape has deformation, of course only for original shapes
			if(obj->shapeConnected && this->renderGlobals->detectShapeDeform && (!obj->isInstancerObject) && (obj->instanceNumber == 0))
			{
				// during motion step
				if( this->renderGlobals->isDeformStep() )
				{
					// in case of geometryMotionblur, we need only the first export filename, no additional steps
					if(obj->geometryMotionblur )
						continue;
					logger.debug(MString("Deforming object not at start frame -> exporting ") + obj->shortName);
					obj->exportShape(this->renderGlobals->currentMbStep, this->renderGlobals->binaryGeoExport, this->renderGlobals->basePath, this->renderGlobals->useShortNames);
				}
			}
		}
	}

	numElements = (int)this->camList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->camList[objId];
		if( this->renderGlobals->currentMbStep == 0)
			obj->transformMatrices.clear();

		if( this->renderGlobals->isTransformStep() )
		{
			logger.debug(MString("Adding cam transform at time ") + this->renderGlobals->currentFrameNumber);
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
		}
		if( obj->transformMatrices.size() > 1)
		{
			MMatrix m1 = obj->transformMatrices[0];
			MMatrix m2 = obj->transformMatrices[1];
			logger.debug(MString("CamMatrix1: t: ") + m1[3][0] + " " + m1[3][1] + " " + m1[3][2]);
			logger.debug(MString("CamMatrix2: t: ") + m2[3][0] + " " + m2[3][1] + " " + m2[3][2]);
		}
	}
	numElements = (int)this->lightList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->lightList[objId];
		if( this->renderGlobals->currentMbStep == 0 )
			obj->transformMatrices.clear();

		if( this->renderGlobals->isTransformStep() )
		{
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
		}
	}

	// Recreate the instancer array for every frame. If we are in a motion blur step, update it.
	// With no motionblur, clear the array and re-parse the instancers. This is necessary because the contents of the instancer
	// can vary from frame to frame e.g. if instances disappear with the death of a particle
	if( !this->renderGlobals->doMb )
	{
		this->clearInstancerNodeList();
		this->parseInstancer();
	}else{
		this->updateInstancer();
	}

	return true;
}

void mtm_MayaScene::runupDynamics()
{
	int numobjects = (int)this->objectList.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->objectList[objId];
		if( (obj->mobject.apiType() == MFn::kParticle) || (obj->mobject.apiType() == MFn::kNParticle))
		{
			MStatus stat;
			MFnParticleSystem psys(obj->mobject, &stat);
			if( !psys.isValid() )
			{
				logger.debug("runupDynamics: Invalid particle system.");
				continue;
			}

			//logger.debug(MString("PartySys:") + psys.name() + " type " + obj->mobject.apiTypeStr());
			MTime currentTime(this->renderGlobals->currentFrameNumber);
			psys.evaluateDynamics(currentTime, false);
		}
	}
}

bool mtm_MayaScene::translateShaders(int timeStep)
{
	return true;
}

bool mtm_MayaScene::translateShapes( int timeStep)
{
	int numobjects = (int)this->objectList.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->objectList[objId]; // does this work?

		if( (obj->instanceNumber == 0) && (!obj->isInstancerObject))
		{
			logger.debug(MString("exporting shape for ") + obj->shortName);
			obj->exportShape(timeStep, this->renderGlobals->binaryGeoExport, this->renderGlobals->basePath, true);
		}
	}
	return true;
}


// loop though elements and fill the passes array in render globals
bool mtm_MayaScene::getPasses()
{
	// delete any existing render passes
	for( uint rpId = 0; rpId < this->renderGlobals->renderPasses.size(); rpId++)
		delete this->renderGlobals->renderPasses[rpId];
	this->renderGlobals->renderPasses.clear();

	// first loop though lights and see if we need shadow map render passes.
	// I create one, if I don't need it, I'll delete it later.
	RenderPass *rp = new RenderPass();
	rp->evalFrequency = RenderPass::OncePerFrame; // adapt this later
	rp->passType = RenderPass::ShadowMap;

	int numlights = (int)this->lightList.size();
	for( int objId = 0; objId < numlights; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->lightList[objId];
		if( obj->shadowMapCastingLight())
			rp->objectList.push_back(obj);
	}	
	if( rp->objectList.size() == 0)
		delete rp;
	else
		this->renderGlobals->renderPasses.push_back(rp);

	// do photon things
	rp = new RenderPass();
	rp->evalFrequency = RenderPass::OncePerFrame; // adapt this later
	rp->passType = RenderPass::PhotonGI;
	{}
	if( rp->objectList.size() == 0)
		delete rp;
	else
		this->renderGlobals->renderPasses.push_back(rp);

	// finally do the normal beauty passes
	rp = new RenderPass();
	rp->evalFrequency = RenderPass::OncePerFrame; // cameras are always once per frame
	rp->passType = RenderPass::Beauty;
	int numcams = (int)this->camList.size();
	for( int objId = 0; objId < numcams; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->camList[objId];
		rp->objectList.push_back(obj);
	}	
	this->renderGlobals->renderPasses.push_back(rp);
	// no need for a beauty render pass check because if there are no cameras, we will not reach this point.
	// so we have at least one camera. Save the first camera for passes motionblur calculations where no other
	// camera is avaliable e.g. shadow maps.
	this->defaultCamera = (MayaObject *)rp->objectList[0];
	
	return true;
}

bool mtm_MayaScene::doPreRenderJobs()
{
	logger.debug("mtm_MayaScene::doPreRenderJobs()");
	this->getShadingGroups(); // get them for all geometry
	this->exportShadingGroups(); // export shading groups
	this->getPasses();
	//exportShadingGroups();
	
	// turn on velocity motionblur for all particle systems
	int numobjects = (int)this->objectList.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		MayaObject *obj = this->objectList[objId];
		if( (obj->mobject.apiType() == MFn::kParticle) || (obj->mobject.apiType() == MFn::kNParticle))
		{
			obj->geometryMotionblur = true;
		}
	}

	return true;
}

// Here we export the geometry definitions.
// These informations are the same for all render passes.
bool mtm_MayaScene::initIfdFile()
{
	logger.debug(MString("mtm_MayaScene::initIfdFile"));

	//exportShadingGroups();
	
	MString exportFileName = "";
	MString exportdir = this->renderGlobals->basePath + "/ifd";
	if(checkDirectory(exportdir))
	{
		exportFileName = exportdir + "/" + this->renderGlobals->imageName + "." + (int)this->currentFrame +  ".ifd";
	}else{
		logger.error(MString("error creating dir:") + exportdir);
		return false;
	}
	
	this->ifdFileName = exportFileName;
	// open ifd file
	this->outFile = new std::ofstream();
	this->outFile->open(exportFileName.asChar());
	if( !outFile->good() )
	{
		outFile->close();
		logger.error(MString("error opening output file:") + exportFileName);
		return false;
	}
	logger.debug(MString("Exporting scene to:") + exportFileName);

	// writing default infos
	// TODO set version in render globals
	*outFile << "ray_version VEX11.0\n";
	*outFile << "setenv HIP = \"" << this->renderGlobals->basePath << "\"\n\n";
	
	MString otlPath = MString("ray_loadotl ") + this->renderGlobals->basePath + "/shaders/shaders.otl";
	*outFile << otlPath << "\n\n";

	return true;
}

bool mtm_MayaScene::writeImagePlanes()
{
	// Bitdepths 8, 16, 16f, 32f
	//static MStringArray({"8", "16", "", "float"}, 4);
	
	//ray_start plane
	if(this->renderGlobals->currentRenderPass->passType == RenderPass::ShadowMap)
	{	
		bool deepShadow = false;
		MayaObject *mo = (MayaObject *)this->renderGlobals->currentRenderPass->objectList[this->renderGlobals->currentRenderPassElementId];
		MFnDependencyNode dn(mo->mobject);
		getBool(MString("mtm_deepShadowMap"), dn, deepShadow);
		MString smapFileName = mo->shadowMapFiles[mo->shadowMapFiles.size()-1];
		*outFile << "\tray_start plane\n";
		if( deepShadow )
		{
			*outFile << "\t\tray_property plane planefile \"null:\"\n";
			*outFile << "\t\tray_property plane variable \"Of\"\n";
			*outFile << "\t\tray_property plane vextype \"vector\"\n";
			*outFile << "\t\tray_property plane pfilter \"ubox\"\n";
		}else{
			*outFile << "\t\tray_property plane planefile \"" + smapFileName + "\"\n";
			*outFile << "\t\tray_property plane variable \"Z-Far\"\n";
			*outFile << "\t\tray_property plane vextype \"float\"\n";
			*outFile << "\t\tray_property plane quantize \"float\"\n";
			*outFile << "\t\tray_property plane pfilter \"ubox\"\n";
		}
		*outFile << "\tray_end\n\n";
		if( deepShadow )
			*outFile << "\tray_property image deepresolver shadow filename \"" + smapFileName + "\"\n";
	}

	if(this->renderGlobals->currentRenderPass->passType == RenderPass::Beauty)
	{	
		*outFile << "\tray_start plane\n";
		*outFile << "\t\tray_property plane variable \"Cf+Af\"\n";
		*outFile << "\t\tray_property plane vextype \"vector4\"\n";
		*outFile << "\t\tray_property plane channel \"C\"\n";
		
		// default is 16bit half
		if(this->renderGlobals->bitdepth != 2)
		{
			if(this->renderGlobals->bitdepth == 0)
				*outFile << "\t\tray_property plane quantize \"8\"\n";
			if(this->renderGlobals->bitdepth == 1)
				*outFile << "\t\tray_property plane quantize \"16\"\n";
			if(this->renderGlobals->bitdepth == 3)
				*outFile << "\t\tray_property plane quantize \"float\"\n";
		}
		*outFile << "\tray_end\n\n";
	}
	return true;
}

bool mtm_MayaScene::writeOutputData()
{
	if(this->renderGlobals->currentRenderPass->passType == RenderPass::ShadowMap)
	{	
		//smap data
		MayaObject *mo = (MayaObject *)this->renderGlobals->currentRenderPass->objectList[this->renderGlobals->currentRenderPassElementId];
		MFnDependencyNode dn(mo->mobject);
		int width = 512;
		int height = 512;
		getInt(MString("dmapResolution"), dn, width); // using default maya smap resolution
		//ray_property image resolution 640 480
		*outFile  << "\tray_property image resolution " << width << " " << height << "\n";
		//ray_property image pixelaspect 1
		*outFile  << "\tray_property image pixelaspect " << this->renderGlobals->pixelAspect << "\n";
		//ray_property image samples 3 3
// replace with smap samples
		*outFile  << "\tray_property image samples " << this->renderGlobals->samples[0] << " " << this->renderGlobals->samples[1] << "\n";
		//ray_property image window 0 1 0 1
		*outFile  << "\tray_property image window 0 1 0 1\n";
		//ray_property image crop 0 1 0 1
		*outFile  << "\tray_property image crop 0 1 0 1\n";
	}

	if(this->renderGlobals->currentRenderPass->passType == RenderPass::Beauty)
	{	
		//image data
		//ray_property image resolution 640 480
		*outFile << "\tray_property image resolution " << this->renderGlobals->imgWidth << " " << this->renderGlobals->imgHeight << "\n";
		//ray_property image pixelaspect 1
		*outFile << "\tray_property image pixelaspect " << this->renderGlobals->pixelAspect << "\n";
		//ray_property image samples 3 3
		*outFile << "\tray_property image samples " << this->renderGlobals->samples[0] << " " << this->renderGlobals->samples[1] << "\n";
		//ray_property image window 0 1 0 1
		*outFile << "\tray_property image window 0 1 0 1\n";
		//ray_property image crop 0 1 0 1
		*outFile << "\tray_property image crop 0 1 0 1\n";
	}
	if(this->renderGlobals->currentRenderPass->passType == RenderPass::PhotonGI)
	{	
		//image data
		//ray_property image resolution 640 480
		*outFile << "\tray_property image resolution " << this->renderGlobals->imgWidth << " " << this->renderGlobals->imgHeight << "\n";
		//ray_property image pixelaspect 1
		*outFile << "\tray_property image pixelaspect " << this->renderGlobals->pixelAspect << "\n";
		//ray_property image samples 3 3
		*outFile << "\tray_property image samples " << this->renderGlobals->samples[0] << " " << this->renderGlobals->samples[1] << "\n";
		//ray_property image window 0 1 0 1
		*outFile << "\tray_property image window 0 1 0 1\n";
		//ray_property image crop 0 1 0 1
		*outFile << "\tray_property image crop 0 1 0 1\n";
	}

	return true;
}
bool mtm_MayaScene::writeRendererData()
{
    //ray_property renderer renderengine "pbrmicropoly"
	
	logger.debug(MString("MayaScene::writeRendererData renderEngineId: ") + this->renderGlobals->renderengine + " RE String: " + this->renderGlobals->renderEngines[this->renderGlobals->renderengine]);
	if( this->renderGlobals->renderengine > 0 )
		*outFile << "\tray_property renderer renderengine \"" << this->renderGlobals->renderEngines[this->renderGlobals->renderengine] << "\"\n";
	//outFile << "\tray_property renderer renderengine \"raytrace\"\n";
	
	// what exactly does this mean?
    //ray_property renderer pbrshader pathtracer use_renderstate 0
	*outFile << "\tray_property renderer pbrshader pathtracer use_renderstate 0\n";

	*outFile << "\tray_property object reflectlimit " << this->renderGlobals->reflectlimit <<"\n";
	*outFile << "\tray_property object refractlimit " << this->renderGlobals->refractlimit <<"\n";

	return true;
}

bool mtm_MayaScene::exportShadingGroups()
{
	logger.debug("mtm_MayaScene::exportShadingGroups()");
	uint numobjects = this->objectList.size();
	for( uint objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *mo = (mtm_MayaObject *)this->objectList[objId];
		mtm_Material *mat = (mtm_Material *)&mo->material;
		if( mat->shadingEngineNode != MObject::kNullObj )
		{
			logger.debug(MString("Export shader network for ") + mo->shortName);
			//mat->printNodes(mat->surfaceShaderNet);
			mat->exportVfl(this->renderGlobals->basePath);
			mat->compileShader(this->renderGlobals->basePath);
		}
	}
	// export light shaders
	numobjects = this->lightList.size();
	for( uint objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *mo = (mtm_MayaObject *)this->lightList[objId];
		mtm_Material *mat = (mtm_Material *)&mo->material;
		if( mat->shadingEngineNode != MObject::kNullObj )
		{
			logger.debug(MString("Export shader network for ") + mo->shortName);
			mat->printNodes(mat->lightShaderNet);
			mat->exportVfl(this->renderGlobals->basePath);
			mat->compileShader(this->renderGlobals->basePath);
		}
	}
	return true;
}

bool mtm_MayaScene::exportSceneForFrame()
{
	// first write all object geometry definitions including several motionblur steps
	for( uint i = 0; i < this->objectList.size(); i++)
	{
		mtm_MayaObject *mo = (mtm_MayaObject *)this->objectList[i];
		mo->exportGeoDefinition(outFile);
		//logger.debug(MString(" --- -- > export objdef for obj: ") + mo->shortName + " hierarchy names: " + mo->exportFileNames.size());
	}

	*outFile << "# current frame " << this->renderGlobals->currentFrameNumber << "\n";
	// start frame definition
	float time = 0.0;
	*outFile << "ray_time " << time << "\n\n";
	
	// we always have at least one render pass element
	mtm_MayaObject *mo = (mtm_MayaObject *)this->renderGlobals->currentRenderPass->objectList[this->renderGlobals->currentRenderPassElementId];

	// 0 == use mplay 
	// 1 == output to file
	// writing to mplay is not really useful if we render a shadow map or photon map...
	if( (this->renderGlobals->outputType == 0) && (this->renderGlobals->currentRenderPass->passType == RenderPass::Beauty))
	{
		*outFile << "\tray_image \"ip\"\n";
	}else{
		// if we are in a shadow map pass, write to correct smap file
		switch(this->renderGlobals->currentRenderPass->passType)
		{
		case RenderPass::Beauty:
			{
				*outFile << "# beauty rendering \n";
				this->renderGlobals->imageOutputFile = this->renderGlobals->imagePath + "/" + this->renderGlobals->imageName + "." + (int)this->renderGlobals->currentFrameNumber + ".tif";
				*outFile << "\tray_image " << this->renderGlobals->imageOutputFile << "\n\n";
				break;
			}
		case RenderPass::ShadowMap:
			{
				// ray_image "temp:spotlight2.rat"
				*outFile << "# shadow map rendering \n";
				MString datadir = this->renderGlobals->basePath + "/data";
				if(checkDirectory(datadir))
				{
					bool deepShadow = false;
					MFnDependencyNode dn(mo->mobject);
					getBool(MString("mtm_deepShadowMap"), dn, deepShadow);

					MString lightSmapName = mo->shortName + ".rat";
					MString smapFileName = datadir + "/" + lightSmapName;
					mo->shadowMapFiles.push_back(smapFileName);
					if( deepShadow )
						*outFile << "\t\tray_image \"null:\"\n";
					else
						*outFile << "\t\tray_image \"" << smapFileName << "\"\n";
				}else{
					logger.error(MString("error creating data dir:") + datadir + " for smap creation.");
					return false;
				}
				break;
			}
		case RenderPass::PhotonGI:
			{
				*outFile << "# photon map rendering \n";
				// ray_image "temp:spotlight2.rat"
				break;
			}
		}
	}
	
	if(!this->writeImagePlanes())
	{
		logger.error("Problem writing imagePlane info");
		return false;
	}

	if(!this->writeOutputData())
	{
		logger.error("Problem writing writeOutputData info");
		return false;
	}

	if(!this->writeRendererData())
	{
		logger.error("Problem writing writeRendererData info");
		return false;
	}


	// export camera
	// if we have a shadow map render pass, use the current light as the camera projection
	if(this->renderGlobals->currentRenderPass->passType != RenderPass::ShadowMap)
	{
		mo->writeCamInstance(outFile, this->renderGlobals->scaleFactor, this->renderGlobals->doDof);
	}else{
		// write data from a lights perspective
		mo->writeSmapLightInstance(outFile, this->renderGlobals->scaleFactor);
	}

	// write light instances
	// Obviously, if we are in a shadow map pass, there is no need to write light instances
	int numobjects = (int)this->lightList.size();
	if(this->renderGlobals->currentRenderPass->passType != RenderPass::ShadowMap)
	{
		for( int objId = 0; objId < numobjects; objId++)
		{
			mtm_MayaObject *obj = (mtm_MayaObject *)this->lightList[objId];
			obj->writeLightInstance(outFile);
		}
	}

	// write object instance definitions
	numobjects = (int)this->objectList.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->objectList[objId];
		if(obj->supported)
			obj->writeGeoInstance(outFile);
	}

	// write instancer object definitions
	numobjects = (int)this->instancerNodeElements.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		mtm_MayaObject *obj = (mtm_MayaObject *)this->instancerNodeElements[objId];
		if(obj->supported)
			obj->writeGeoInstance(outFile);
	}

	*outFile << "\tray_raytrace\n";
	*outFile << "\tray_reset  -l -o -f\n";	
	*outFile << "ray_quit\n";	

	return true;
}

bool mtm_MayaScene::finalizeExportScene()
{
	if( (this->outFile != NULL) && this->outFile->good())
		this->outFile->close();
	return true;
}

bool mtm_MayaScene::executeGeoConverter()
{
	MStatus stat = MS::kSuccess;
	bool result = true;
	MString cmd = MString("import mtm_initialize as minit; reload(minit);minit.startGeoConverter()");
	logger.info(cmd);
	stat = MGlobal::executePythonCommand(cmd);
	if(!stat)
	{
		logger.error(MString("Problem execting cmd"));
		return false;
	}
	return true;
}

bool mtm_MayaScene::executeRenderCmd()
{
	MStatus stat = MS::kSuccess;
	bool result = true;
	MString cmd = MString("import mtm_initialize as minit; reload(minit);minit.startRenderProc( \"" + this->ifdFileName + "\")");
	logger.info(cmd);

	stat = MGlobal::executePythonCommand(cmd);
	if(!stat)
	{
		logger.error(MString("Problem execting cmd"));
		return false;
	}

	return true;
}

// jobs done for every frame like perFrame shadow maps, perFrame photons or per frame beauty
// every frame may contain one beauty and other passes. Every pass can contain several elements.
// So a beauty pass can contain several cameras, a shadowMap pass can contain several lights.
// The complete data for one frame is written to a ifd file and will be rendered.
bool mtm_MayaScene::doFrameJobs()
{
	logger.debug("mtm_MayaScene::doFrameJobs()");
	logger.progress(MString("\n========== Start rendering of frame ") + this->currentFrame + " ==============\n");

	this->initIfdFile();
	int numPasses = (int)this->renderGlobals->renderPasses.size();
	for( int pId = 0; pId < numPasses; pId++)
	{
		// if it is not a per frame pass, it is already done
		if( this->renderGlobals->renderPasses[pId]->evalFrequency != RenderPass::OncePerFrame )
			continue;

		// Get the objects from the currentPass object list. 
		// This can be a light or a camera. In case of a shadow map generation it is a light,
		// in case of a beauty its a camera.
		this->renderGlobals->currentRenderPass = this->renderGlobals->renderPasses[pId];
		int numPassElements = (int)this->renderGlobals->currentRenderPass->objectList.size();
		for( int pElId = 0; pElId < numPassElements; pElId++)
		{
		
			this->renderGlobals->currentRenderPassElementId = pElId;
			// Now get the motionblur steps. Normally I get the shutter angle from the camera, but in case of the
			// a shadow map rendering I dont have a specific camera so I use the first one == defaultCamera
			// in most cases in a shadow map rendring I dont have motionblur so handle this in getMbSteps()
			if( this->renderGlobals->currentRenderPass->passType == RenderPass::ShadowMap)
				this->renderGlobals->getMbSteps(this->defaultCamera->mobject);

			if( this->renderGlobals->currentRenderPass->passType == RenderPass::Beauty)
			{
				MayaObject *obj = (MayaObject *)this->renderGlobals->currentRenderPass->objectList[pElId];
				this->renderGlobals->getMbSteps(this->defaultCamera->mobject);
			}

			if(this->renderGlobals->mbElementList.size() == 0)
			{
				logger.error(MString("no mb steps, somethings wrong."));
				return false;
			}

			int numMbSteps = (int)this->renderGlobals->mbElementList.size();
			logger.debug(MString("mbSteps: ") + numMbSteps);
			this->renderGlobals->currentMbStep = 0;

			// here we loop through the scene and update all elements before writing to disk
			// this contains updating transforms, writing deformed objects etc.
			for( int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
			{
				this->renderGlobals->isMbStartStep = this->renderGlobals->mbElementList[mbStepId].time == this->renderGlobals->mbElementList[0].time;
				this->renderGlobals->currentMbElement = this->renderGlobals->mbElementList[mbStepId];
				this->renderGlobals->currentFrameNumber = (float)(this->renderGlobals->currentFrame + this->renderGlobals->mbElementList[mbStepId].time);
				bool needView = true;
				if( mbStepId > 0)
				{
					if( this->renderGlobals->mbElementList[mbStepId].time != this->renderGlobals->mbElementList[mbStepId-1].time)
					{
						needView = true;
					}else{
						needView = false;
					}
				}
				if( needView )
				{
					logger.debug(MString("doFrameJobs() viewFrame: ") + this->renderGlobals->currentFrameNumber);
					MGlobal::viewFrame(this->renderGlobals->currentFrameNumber);
				}
				//if(numMbSteps > 1)
				//	this->runupDynamics();
				this->updateScene();
				logger.info(MString("update scene done"));
				this->renderGlobals->currentMbStep++;
			}
			// write all necessary informations into ifd file for this pass
			this->exportSceneForFrame();
		}		
	}
	return true;
}

bool mtm_MayaScene::doPreFrameJobs()
{
	return true;
}

bool mtm_MayaScene::doPostFrameJobs()
{
	logger.debug("mtm_MayaScene::doPostFrameJobs()");
	return true;
}

bool mtm_MayaScene::doPostRenderJobs()
{
	logger.debug("mtm_MayaScene::doPostRenderJobs()");
	return true;
}

bool mtm_MayaScene::renderScene()
{
	if(!this->doPreRenderJobs())
	{
		logger.error("doPreRenderJobs failed.");
		return false;
	}

	int numFrames = (int)this->renderGlobals->frameList.size();
	for( int frameNr = 0; frameNr < numFrames; frameNr++)
	{
		this->currentFrame = this->renderGlobals->frameList[frameNr];
		this->renderGlobals->currentFrame = this->currentFrame;
		if(!this->doPreFrameJobs())
		{
			logger.error("doPreFrameJobs failed.");
			return false;
		}
		if(!this->doFrameJobs())
		{
			logger.error("doFrameJobs failed.");
			return false;
		}
		// quit and close ifd file
		this->finalizeExportScene();
		// convert necessary geometry from hsff to geo format
		this->executeGeoConverter();
		// render the ifd file
		this->executeRenderCmd();

		if(!this->doPostFrameJobs())
		{
			logger.error("doPostFrameJobs failed.");
			return false;
		}
		logger.progress(MString("\n========== Rendering of frame ") + this->currentFrame + " done ==============\n");
	}

	if(!this->doPostRenderJobs())
	{
		logger.error("doPostRenderJobs failed.");
		return false;
	}
	logger.info("Mantra Rendering done.");
	return true;
}

