#include "Lux.h"
#include <iostream>
#include <maya/MStringArray.h>

#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "renderGlobals.h"
#include "mayaScene.h"
#include "world.h"

#include "utilities/logging.h"
static Logging logger;

bool LuxRenderer::isRendering;


// create a new instance with proper destruction
Instance CreateInstance(const std::string name) {
    return Instance(CreateLuxInstance(name.c_str()), DestroyLuxInstance);
}

// create a new paramset with proper destruction
ParamSet CreateParamSet() {
    return ParamSet(CreateLuxParamSet(), DestroyLuxParamSet);
}

LuxRenderer::LuxRenderer()
{}

LuxRenderer::~LuxRenderer()
{}

void LuxRenderer::initializeRenderer()
{}

void LuxRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{
	if( obj->instanceNumber > 0)
		return;
	
	if( !obj->geometryShapeSupported() )
		return;
	
	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;		
}

void LuxRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{
	//	mtlu_MayaObject *obj = (mtlu_MayaObject *)mobj;
	//	//logger.trace(MString("mtlu_MayaScene::transformUpdateCallback"));
	//
	//	//logger.debug(MString("mtlu_updateObj: ") + mobj->dagPath.fullPathName());
	//
	//	if( isCamera(obj->mobject))
	//	{
	//		this->mtlu_renderer.updateTransform(obj);
	//		return;
	//	}
	//
	//	// if this is an instance of an object which has an assembly, then update it,
	//	// if not, ignore instance.
	//	if( obj->instanceNumber > 0)
	//	{
	//		if( obj->origObject == NULL)
	//			return;
	//
	//	}else{
	//		if( obj->instancerParticleId > -1)
	//		{
	//			if( obj->origObject == NULL)
	//			{
	//				logger.debug(MString("transformUpdateCallback: obj orig obj == NULL with instancerParticleId > -1"));
	//				return;
	//			}
	//		}else{
	//		}
	//	}
}

void LuxRenderer::IPRUpdateEntities()
{}

void LuxRenderer::reinitializeIPRRendering()
{}

void LuxRenderer::abortRendering()
{
	this->lux->abort();
}

void LuxRenderer::getMeshPoints(MPointArray& pointArray)
{
	for( int i = 0; i < 500; i++)
	{
		MPoint p;
		float radius = 20.0f * (float)rand()/(float)RAND_MAX;
		MVector v;
		v.x = ((float)rand()/(float)RAND_MAX - 0.5f) * 2.0;
		v.y = ((float)rand()/(float)RAND_MAX - 0.5f) * 2.0;
		v.z = ((float)rand()/(float)RAND_MAX - 0.5f) * 2.0;
		v.normalize();
		p.x =  v.x * radius;
		p.y =  v.y * radius;
		p.z =  v.z * radius;

		pointArray.append(p);
	}
}


void LuxRenderer::getFramebufferThread( void *pointer)
{
	LuxRenderer *luxRenderer = (LuxRenderer*)pointer;
	
	//Context::UpdateFramebuffer
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	int width, height;
	renderGlobals->getWidthHeight(width, height);

	EventQueue::Event e;

	while( isRendering )
	{
		//logger.debug("Update framebuffer...");

		luxRenderer->lux->updateFramebuffer();

		const unsigned char *fb = luxRenderer->lux->framebuffer();
		const float *ffb = luxRenderer->lux->floatFramebuffer();
		const float *fa = luxRenderer->lux->alphaBuffer();
		size_t numPixels = width * height;
		std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
		RV_PIXEL *pixels = pixelsPtr.get();
		unsigned int mayaPixel = 0;

		if( fb != NULL )
		{
			for( unsigned int y = height-1; y > 0; y--)
			{
				for( unsigned int x = 0; x < width; x++)
				{
					unsigned int index = y * width + x;
					pixels[mayaPixel].r = fb[index * 3];
					pixels[mayaPixel].g = fb[index * 3 + 1];
					pixels[mayaPixel].b = fb[index * 3 + 2];
					pixels[mayaPixel].a = 0.0;
					if( fa )
						pixels[mayaPixel].a = fa[index] * 255.0f;
					mayaPixel++;
				}
			}
		}else{
			logger.debug("framebuffer == NULL");
		}
		
		e.tile_xmin = e.tile_ymin = 0;
		e.tile_xmax = width - 1;
		e.tile_ymax = height - 1;
		e.pixelData = pixelsPtr;
		e.type = EventQueue::Event::FRAMEUPDATE;
		theRenderEventQueue()->push(e);
		std::this_thread::sleep_for(std::chrono::milliseconds(getIntAttr("uiupdateinterval", gFn, 100)));
	}

	logger.debug("Update framebuffer thread done.");

}

void LuxRenderer::defineSampling()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	const char *samplerNames[] = { "random", "lowdiscrepancy", "metropolis" };
	MStringArray samplers(samplerNames, 3);
	ParamSet samplerParams = CreateParamSet();
	bool noiseAware = getBoolAttr("noiseaware", gFn, true);
	samplerParams->AddBool("noiseaware", &noiseAware);

	MString sampler = samplers[getIntAttr("sampler", gFn, 0)];
	if ((sampler == "random") || (sampler == "lowdiscrepancy"))
	{
		int pixelsamples = getIntAttr("numSamples", gFn, 4);
		samplerParams->AddInt("pixelsamples", &pixelsamples);
		const char *pixelSamplerNames[] = {"hilbert", "linear", "vegas", "lowdiscrepancy", "tile", "random"};
		int samplerNameInt = getIntAttr("pixelSampler", gFn, 0);
		samplerParams->AddString("pixelsampler", &pixelSamplerNames[samplerNameInt]);
	}
	if ((sampler == "metropolis"))
	{
		int mcr = getIntAttr("maxconsecrejects", gFn, 1);
		samplerParams->AddInt("maxconsecrejects", &mcr);
		float fval = getFloatAttr("largemutationprob", gFn, 1.0);
		samplerParams->AddFloat("largemutationprob", &fval);
		fval = getFloatAttr("mutationrange", gFn, 1.0);
		samplerParams->AddFloat("mutationrange", &fval);
		bool bval = getBoolAttr("usevariance", gFn, true);
		samplerParams->AddBool("usevariance", &bval);
		bval = getBoolAttr("usecooldown", gFn, true);
		samplerParams->AddBool("usecooldown", &bval);
	}

	this->lux->sampler(sampler.asChar(), boost::get_pointer(samplerParams));
}

void LuxRenderer::defineAccelerator()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	const char *rendererNames[] = { "kdtree", "qbvh", "grid", "unsafekdtree", "bvh", "none" };
	const char *rendererName = rendererNames[getEnumInt("accelerator", gFn)];
	ParamSet accParams = CreateParamSet();

	if( MString(rendererName) == "kdtree")
	{
		int intVal = getIntAttr("kdIntersectcost", gFn, 80);
		accParams->AddInt("intersectcost", &intVal);
		intVal = getIntAttr("kdTraversalcost", gFn, 1);
		accParams->AddInt("traversalcost", &intVal);
		float floatVal = getFloatAttr("kdEmptybonus", gFn, 0.5f);
		accParams->AddFloat("emptybonus", &floatVal);
		intVal = getIntAttr("kdMaxprims", gFn, 1);
		accParams->AddInt("maxprims", &intVal);
		intVal = getIntAttr("kdMaxdepth", gFn, -1);
		accParams->AddInt("maxdepth", &intVal);
	}

	if( MString(rendererName) == "qbvh")
	{
		int intVal = getIntAttr("maxprimsperleaf", gFn, 4);
		accParams->AddInt("maxprimsperleaf", &intVal);
		intVal = getIntAttr("fullsweepthreshold", gFn, 4*4);
		accParams->AddInt("fullsweepthreshold", &intVal);
		intVal = getIntAttr("skipfactor", gFn, 1);
		accParams->AddInt("skipfactor", &intVal);
	}

	this->lux->accelerator(rendererName, boost::get_pointer(accParams));
}

void LuxRenderer::defineRenderer()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());
	const char *rendererNames[] = { "sampler", "hybrid", "sppm" };
	ParamSet rendererParams = CreateParamSet();
	this->lux->renderer(rendererNames[getEnumInt("renderer", gFn)], boost::get_pointer(rendererParams));
}

void LuxRenderer::definePixelFilter()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());
	const char *filterNames[] = {"box", "triangle", "gaussian", "mitchell", "sinc"};
	MStringArray filters(filterNames, 5);
	MString filter = filters[getEnumInt("pixelfilter", gFn)];
	ParamSet filterParams = CreateParamSet();
	float floatVal = getFloatAttr("filterWidth", gFn, 2.0f);
	filterParams->AddFloat("xwidth", &floatVal);
	floatVal = getFloatAttr("filterHeight", gFn, 2.0f);
	filterParams->AddFloat("ywidth", &floatVal);
	if(filter == "gaussian")
	{
		floatVal = getFloatAttr("filterAlpha", gFn, 2.0f);
		filterParams->AddFloat("alpha", &floatVal);
	}
	if(filter == "mitchell")
	{
		floatVal = getFloatAttr("B", gFn, .33f);
		filterParams->AddFloat("B", &floatVal);
		floatVal = getFloatAttr("C", gFn, .33f);
		filterParams->AddFloat("C", &floatVal);
		bool boolVar = getBoolAttr("mSupersample", gFn, false);
		filterParams->AddBool("supersample", &boolVar);
	}
	if (filter == "sinc")
	{
		floatVal = getFloatAttr("sincTau", gFn, 3.0f);
		filterParams->AddFloat("tau", &floatVal);
	}
	this->lux->pixelFilter(filter.asChar(), boost::get_pointer(filterParams));
}

void LuxRenderer::defineSurfaceIntegrator()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	bool boolValue;
	int intValue;
	float floatValue;

	const char *integratorNames[] = {"bidirectional", "path", "exphotonmap", "directlighting", "igi", "distributedpath", "sppm"}; 
	MStringArray integrators(integratorNames, 7);
	ParamSet iParams = CreateParamSet();
	int renderer = getEnumInt("renderer", gFn);
	MString integrator = integratorNames[getEnumInt("surfaceIntegrator", gFn)];
	const char *lStrategies[] = { "one", "all", "auto", "importance", "powerimp", "allpowerimp", "logpowerimp" };
	MString strategy = lStrategies[getEnumInt("lightStrategy", gFn)];
	const char *ls = strategy.asChar();

	if (integrator != "sppm")
	{
		iParams->AddString("lightstrategy", &ls);
		int intVal = getIntAttr("shadowraycount", gFn, 1);
		iParams->AddInt("shadowraycount", &intVal);
	}

	//// Bidirectional - hybrid or sampler only
	if (renderer < 2)
	{
		if (integrator == "bidirectional")
		{
			// hybrid requires "one"
			if (renderer == 1)
				ls = lStrategies[0];
			iParams->AddString("lightpathstrategy", &ls);
			intValue = getIntAttr("eyedepth", gFn, 8);
			iParams->AddInt("eyedepth", &intValue);
			floatValue = getFloatAttr("lightrrthreshold", gFn, 0.0);
			iParams->AddFloat("lightrrthreshold", &floatValue);
			floatValue = getFloatAttr("eyerrthreshold", gFn, 0.0);
			iParams->AddFloat("eyerrthreshold", &floatValue);
		}

		if (integrator == "path")
		{
			intValue = getIntAttr("phMaxdepth", gFn, 5);
			iParams->AddInt("phMaxdepth", &intValue);
			boolValue = getBoolAttr("includeenvironment", gFn, true);
			iParams->AddBool("includeenvironment", &boolValue);
			boolValue = getBoolAttr("directlightsampling", gFn, true);
			iParams->AddBool("directlightsampling", &boolValue);
			floatValue = getFloatAttr("rrcontinueprob", gFn, .65);
			iParams->AddFloat("rrcontinueprob", &floatValue);
			const char *rrstrategys[] = { "none", "probability", "efficiency" };
			const char *rrstrategy = rrstrategys[getEnumInt("rrstrategy", gFn)];
			iParams->AddString("rrstrategy", &rrstrategy);	
		}
	}

	// direct/photonmap - sampler only
	if (renderer == 0)
	{
		if (integrator == "direct")
		{
			intValue = getIntAttr("pathMaxdepth", gFn, 20000);
			iParams->AddInt("maxdepth", &intValue);
			const char *rrstrategys[] = {"none", "probability", "efficiency"};
			const char *rrstrategy = rrstrategys[getEnumInt("rrstrategy", gFn)];
			iParams->AddString("rrstrategy", &rrstrategy);						
		}

		if (integrator == "exphotonmap")
		{
			const char *phRenderingmodes[] =  { "directlighting","path" };
			const char *phRenderingmode = phRenderingmodes[getEnumInt("phRenderingmode", gFn)];
			iParams->AddString("renderingmode", &phRenderingmode);
			intValue = getIntAttr("phCausticphotons", gFn, 20000);
			iParams->AddInt("causticphotons", &intValue);
			intValue = getIntAttr("phIndirectphotons", gFn, 200000);
			iParams->AddInt("indirectphotons", &intValue);
			intValue = getIntAttr("phDirectphotons", gFn, 200000);
			iParams->AddInt("directphotons", &intValue);
			intValue = getIntAttr("phRadiancephotons", gFn, 200000);
			iParams->AddInt("radiancephotons", &intValue);
			intValue = getIntAttr("phNphotonsused", gFn, 50);
			iParams->AddInt("nphotonsused", &intValue);
			floatValue = getFloatAttr("phMaxphotondist", gFn, 0.5);
			iParams->AddFloat("maxphotondist", &floatValue);
			intValue = getIntAttr("phMaxdepth", gFn, 5);
			iParams->AddInt("maxdepth", &intValue);
			intValue = getIntAttr("maxphotondepth", gFn, 16);
			iParams->AddInt("maxphotondepth", &intValue);
			boolValue = getBoolAttr("phFinalgather", gFn, true);
			iParams->AddBool("finalgather", &boolValue);
			intValue = getIntAttr("phFinalgathersamples", gFn, 32);
			iParams->AddInt("finalgathersamples", &intValue);
			floatValue = getFloatAttr("phGatherangle", gFn, 10.0);
			iParams->AddFloat("gatherangle", &floatValue);
			floatValue = getFloatAttr("rrcontinueprob", gFn, .65);
			iParams->AddFloat("rrcontinueprob", &floatValue);
			floatValue = getFloatAttr("phDistancethreshold", gFn, 1.25);
			iParams->AddFloat("phDistancethreshold", &floatValue);

			const char *phRrstrategys[] =  { "none","probability","efficiency" };
			const char *phRrstrategy = phRrstrategys[getEnumInt("phRrstrategy", gFn)];
			iParams->AddString("rrstrategy", &phRrstrategy);
		}
		
		if (integrator == "distributedpath")
		{
			const char *renderingmodes[] =  { "directlighting","path" };
			const char *rmode = renderingmodes[getEnumInt("renderingmode", gFn)];
			iParams->AddString("renderingmode", &rmode);
			const char *strategys[] =  { "auto","all","one" };
			const char *strategy = strategys[getEnumInt("strategy", gFn)];
			iParams->AddString("strategy", &strategy);
			boolValue = getBoolAttr("directsampleall", gFn, true);
			iParams->AddBool("directsampleall", &boolValue);
			intValue = getIntAttr("directsamples", gFn, 1);
			iParams->AddInt("directsamples", &intValue);
			boolValue = getBoolAttr("indirectsampleall", gFn, false);
			iParams->AddBool("indirectsampleall", &boolValue);
			intValue = getIntAttr("indirectsamples", gFn, 1);
			iParams->AddInt("indirectsamples", &intValue);
			intValue = getIntAttr("diffusereflectdepth", gFn, 3);
			iParams->AddInt("diffusereflectdepth", &intValue);
			intValue = getIntAttr("diffusereflectsamples", gFn, 1);
			iParams->AddInt("diffusereflectsamples", &intValue);
			intValue = getIntAttr("diffuserefractdepth", gFn, 5);
			iParams->AddInt("diffuserefractdepth", &intValue);
			intValue = getIntAttr("diffuserefractsamples", gFn, 1);
			iParams->AddInt("diffuserefractsamples", &intValue);
			boolValue = getBoolAttr("directdiffuse", gFn, true);
			iParams->AddBool("directdiffuse", &boolValue);
			boolValue = getBoolAttr("indirectdiffuse", gFn, true);
			iParams->AddBool("indirectdiffuse", &boolValue);
			intValue = getIntAttr("glossyreflectdepth", gFn, 2);
			iParams->AddInt("glossyreflectdepth", &intValue);
			intValue = getIntAttr("glossyreflectsamples", gFn, 1);
			iParams->AddInt("glossyreflectsamples", &intValue);
			intValue = getIntAttr("glossyrefractdepth", gFn, 5);
			iParams->AddInt("glossyrefractdepth", &intValue);
			intValue = getIntAttr("glossyrefractsamples", gFn, 1);
			iParams->AddInt("glossyrefractsamples", &intValue);
			boolValue = getBoolAttr("directglossy", gFn, true);
			iParams->AddBool("directglossy", &boolValue);
			boolValue = getBoolAttr("indirectglossy", gFn, true);
			iParams->AddBool("indirectglossy", &boolValue);
			intValue = getIntAttr("specularreflectdepth", gFn, 3);
			iParams->AddInt("specularreflectdepth", &intValue);
			intValue = getIntAttr("specularrefractdepth", gFn, 3);
			iParams->AddInt("specularrefractdepth", &intValue);
			boolValue = getBoolAttr("diffusereflectreject", gFn, false);
			iParams->AddBool("diffusereflectreject", &boolValue);
			boolValue = getBoolAttr("diffuserefractreject", gFn, false);
			iParams->AddBool("diffuserefractreject", &boolValue);
			intValue = getIntAttr("diffusereflectreject_threshold", gFn, 10.0);
			iParams->AddInt("diffusereflectreject_threshold", &intValue);
			intValue = getIntAttr("diffuserefractreject_threshold", gFn, 10.0);
			iParams->AddInt("diffuserefractreject_threshold", &intValue);
			boolValue = getBoolAttr("glossyreflectreject", gFn, false);
			iParams->AddBool("glossyreflectreject", &boolValue);
			boolValue = getBoolAttr("glossyrefractreject", gFn, false);
			iParams->AddBool("glossyrefractreject", &boolValue);
			intValue = getIntAttr("glossyreflectreject_threshold", gFn, 10.0);
			iParams->AddInt("glossyreflectreject_threshold", &intValue);
			intValue = getIntAttr("glossyrefractreject_threshold", gFn, 10.0);
			iParams->AddInt("glossyrefractreject_threshold", &intValue);
		}

	}

	//// sppm only
	if (renderer == 2)
	{
		if (integrator == "sppm")
		{
			const char *photonsamplers[]= {"halton", "amc"};
			const char *photonsampler = photonsamplers[getEnumInt("photonsampler", gFn)];
			iParams->AddString("photonsampler", &photonsampler);
			const char *lookupaccels[] = {"hybridhashgrid","kdtree","grid","hashgrid","parallelhashgrid"};
			const char *lookupaccel = lookupaccels[getEnumInt("lookupaccel", gFn)];
			iParams->AddString("lookupaccel", &lookupaccel);
			const char *pixelsamplers[] = {"hilbert", "linear", "tile", "vegas"};
			const char *pixelsampler = pixelsamplers[getEnumInt("sppmpixelsampler", gFn)];
			iParams->AddString("pixelsampler", &pixelsampler);
			floatValue = getFloatAttr("parallelhashgridspare", gFn, 1.0);
			iParams->AddFloat("parallelhashgridspare", &floatValue);
			boolValue = getBoolAttr("includeenvironment", gFn, true);
			iParams->AddBool("includeenvironment", &boolValue);
			boolValue = getBoolAttr("directlightsampling", gFn, true);
			iParams->AddBool("directlightsampling", &boolValue);
			intValue = getIntAttr("photonperpass", gFn, 1000000);
			iParams->AddInt("photonperpass", &intValue);
			intValue = getIntAttr("startk", gFn, 0);
			iParams->AddInt("startk", &intValue);
			floatValue = getFloatAttr("alpha", gFn, 0.7);
			iParams->AddFloat("alpha", &floatValue);
			floatValue = getFloatAttr("glossythreshold", gFn, 100);
			iParams->AddFloat("glossythreshold", &floatValue);
			floatValue = getFloatAttr("startradius", gFn, 2.0);
			iParams->AddFloat("startradius", &floatValue);
			boolValue = getBoolAttr("useproba", gFn, true);
			iParams->AddBool("useproba", &boolValue);
			intValue = getIntAttr("wavelengthstratification", gFn, 8);
			iParams->AddInt("wavelengthstratification", &intValue);
			boolValue = getBoolAttr("debug", gFn, false);
			iParams->AddBool("debug", &boolValue);
			boolValue = getBoolAttr("storeglossy", gFn, false);
			iParams->AddBool("storeglossy", &boolValue);
			intValue = getIntAttr("maxeyedepth", gFn, 16);
			iParams->AddInt("maxeyedepth", &intValue);
			intValue = getIntAttr("maxphotondepth", gFn, 16);
			iParams->AddInt("maxphotondepth", &intValue);
		}
	}

	this->lux->surfaceIntegrator(integrator.asChar(), boost::get_pointer(iParams));
}


void LuxRenderer::render()
{
	logger.debug(MString("Render lux."));
	EventQueue::Event e;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());
	
	int width, height;
	renderGlobals->getWidthHeight(width, height);

	MPointArray pointArray;
	getMeshPoints(pointArray);
	isRendering = false;

	MString outputPath = renderGlobals->basePath + "/" + renderGlobals->imageName + "." + (int)renderGlobals->currentFrame + ".lxs";
	logger.debug(MString("Writing scene file to: ") + outputPath);
	luxFile.open(outputPath.asChar());

    try {

		float fov=30;
		std::string filename = "cpp_api_test";

		lux = CreateInstance("cpp_api_test");

		this->defineCamera();

		this->defineFilm();

		this->defineSampling();

		this->defineSurfaceIntegrator();

		this->defineAccelerator();

		this->defineRenderer();

		this->definePixelFilter();

		lux->worldBegin();
		if( renderGlobals->exportSceneFile)
			this->luxFile << "WorldBegin\n";

		this->defineShaders();

		this->defineLights();

		this->defineGeometry();

		lux->worldEnd();
		if( renderGlobals->exportSceneFile)
			this->luxFile << "WorldEnd\n";

		luxFile.close();

		isRendering = true;

		MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING);
		// wait for the WorldEnd thread to start running
		// this isn't terribly reliable, cpp_api should be modified
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		boost::thread(LuxRenderer::getFramebufferThread, this);

		for( int tid = 0; tid < renderGlobals->threads; tid++)
			lux->addThread();

		lux->wait();

		MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATENONE);

		isRendering = false;
		const unsigned char *fb = lux->framebuffer();
		const float *ffb = lux->floatFramebuffer();
		const float *fa = lux->alphaBuffer();
		size_t numPixels = width * height;
		std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
		RV_PIXEL *pixels = pixelsPtr.get();
		unsigned int mayaPixel = 0;

		if( fb != NULL )
		{
			for( unsigned int y = height-1; y > 0; y--)
			{
				for( unsigned int x = 0; x < width; x++)
				{
					unsigned int index = y * width + x;
					pixels[mayaPixel].r = fb[index * 3];
					pixels[mayaPixel].g = fb[index * 3 + 1];
					pixels[mayaPixel].b = fb[index * 3 + 2];
					pixels[mayaPixel].a = 0.0;
					if( fa )
						pixels[mayaPixel].a = fa[index] * 255.0f;
					mayaPixel++;
				}
			}
		}
		
		e.tile_xmin = e.tile_ymin = 0;
		e.tile_xmax = width - 1;
		e.tile_ymax = height - 1;
		e.pixelData = pixelsPtr;

		// saveFLM needs extension
		filename = filename + ".flm";
		lux->saveFLM(filename.c_str());
		

		logger.debug(MString("Render lux."));

		lux->cleanup();

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

