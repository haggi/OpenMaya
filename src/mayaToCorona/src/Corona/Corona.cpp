#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"

#include "MyClasses.h"
#include "CoronaMap.h"

static Logging logger;

CoronaRenderer::CoronaRenderer()
{
	this->context.core = NULL;
	this->context.fb = NULL;
	this->context.scene = NULL;
	this->context.logger = NULL;
	this->context.settings = NULL;
	this->context.isCancelled = false;
}

CoronaRenderer::~CoronaRenderer()
{
	Corona::ICore::shutdownLib();
}


#ifdef CORONA_RELEASE_ASSERTS
#pragma comment(lib, "PrecompiledLibs_Assert.lib")
#pragma comment(lib, "Corona_Assert.lib")
#else
#pragma comment(lib, "../../lib/PrecompiledLibs_Release.lib")
#pragma comment(lib, "../../lib/Corona_Release.lib")
#endif

using namespace Corona;

void CoronaRenderer::saveImage()
{
	Corona::Bitmap<Corona::Rgb, false> bitmap(this->context.fb->getImageSize());
    Corona::Bitmap<float, false> alpha(this->context.fb->getImageSize());

    for(int i = 0; i < bitmap.getHeight(); ++i) 
	{
        const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
        this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), Corona::CHANNEL_BEAUTY, true, true, &bitmap[pixel], &alpha[pixel]);
    }

    //// since we get the colors from frame buffer after color mapping, that includes gamma correction, they are not 
    //// in linear space (the "false" argument)
	this->mtco_renderGlobals->getImageName();
	Corona::String filename = this->mtco_renderGlobals->imageOutputFile.asChar();
	logger.debug(MString("Saving image as ") +  this->mtco_renderGlobals->imageOutputFile);
	
	//if( this->mtco_renderGlobals->imageFormatString.toLowerCase() == "png")
	//    Corona::Wx::saveImage(filename+".png", Corona::RgbBitmapIterator<false>(bitmap, &alpha), false, Corona::IMAGE_DETERMINE_FROM_EXT);

	//if( this->mtco_renderGlobals->imageFormatString.toLowerCase() == "exr")
	Corona::saveImage(filename+".exr", Corona::RgbBitmapIterator<false>(bitmap, &alpha), false, Corona::IMAGE_DETERMINE_FROM_EXT);
}

void CoronaRenderer::createScene()
{
	this->defineCamera();
	this->defineGeometry();
	this->defineLights();
	this->defineEnvironment();
}


void CoronaRenderer::render()
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  INIT SHADING CORE + SCEME
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICore::initLib(false);
    
    //Context context;
    context.core = ICore::createInstance();
    context.scene = context.core->createScene();
    context.logger = new mtco_Logger(context.core);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  SETTINGS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    context.settings = new MySettings();

    // populate the settings with parameters from a configuration file. If the file does not exist, a new one 
    // is created with default values.
    ConfParser parser;
    parser.parseFile("./" CORONA_DEFAULT_CONF_FILENAME, context.settings, true);

    // lets change few parameters, for example resolution, and render stamp
	context.settings->set(PARAM_IMAGE_WIDTH, this->mtco_renderGlobals->imgWidth);
	context.settings->set(PARAM_IMAGE_HEIGHT, this->mtco_renderGlobals->imgHeight);
    context.settings->set(PARAM_IMAGE_REGION_END_X, this->mtco_renderGlobals->imgWidth);
    context.settings->set(PARAM_IMAGE_REGION_END_Y,  this->mtco_renderGlobals->imgHeight);
	context.settings->set(PARAM_PROGRESSIVE_MAX_PASSES, this->mtco_renderGlobals->progressive_maxPasses);
	context.settings->set(PARAM_NUM_THREADS, this->mtco_renderGlobals->threads);
	context.settings->set(PARAM_EXPORT_ONLY, this->mtco_renderGlobals->exportSceneFile);
	
 
    // add a custom string to the render stamp
    String renderStamp = context.settings->get(PARAM_RENDERSTAMP);
    renderStamp = "Corona API demo | " + renderStamp;
    context.settings->set(PARAM_RENDERSTAMP, renderStamp);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  RENDER PASSES
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // lets add few render passes. One render pass can be added multiple times with different names
    RenderPassData data;
    data.className = data.name = "NormalsDotProduct";
    context.renderPasses.push(context.core->createRenderPass(data));

    data.className = "zdepth";
    data.name = "Z depth render pass";
    data.zDepth.minDepth = 100;
    data.zDepth.maxDepth = 200;
    context.renderPasses.push(context.core->createRenderPass(data));

    data.name = "Second zdepth pass";
    data.zDepth.minDepth = 150;
    data.zDepth.maxDepth = 250;
    context.renderPasses.push(context.core->createRenderPass(data));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  INTERNAL FRAME BUFFER
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create a new framebuffer, and init it before the rendering
    context.fb = context.core->createFb();
    // the settings and render passes need to contain final parameters of the rendering at the time of the call
    context.fb->initFb(context.settings, context.renderPasses);

	createScene();

    // test that the now ready scene and settings do not have any errors
    context.core->sanityCheck(context.scene);
    context.core->sanityCheck(context.settings);

	Corona::String basePath = (this->mtco_renderGlobals->basePath + "/renderData/").asChar();
    context.core->beginSession(context.scene, context.settings, context.fb, context.logger, basePath);
    
    // run the rendering. This function blocks until it is done
    context.core->renderFrame();
	context.isCancelled = true;
    context.core->endSession();
    
	this->saveImage();

    // delete what we have created and call deallocation functions for objects the core has created
    delete context.logger;
    delete context.settings;
    
    for(auto it = context.renderPasses.begin(); it != context.renderPasses.end(); ++it) 
	{
        context.core->destroyRenderPass(*it);
    }

    context.core->destroyScene(context.scene);
    context.core->destroyFb(context.fb);
    ICore::destroyInstance(context.core);
    //ICore::shutdownLib();
	context.core = NULL;
	context.fb = NULL;
	context.renderPasses.clear();
	context.isCancelled = false;
	context.scene = NULL;

	// for sequence rendering, at the moment clean up pointers
	// will be removed if I restructure the geo updates

	for( size_t objId = 0; objId < this->mtco_scene->objectList.size(); objId++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)this->mtco_scene->objectList[objId];
		obj->geom = NULL;
		obj->instance = NULL;
	}

	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void CoronaRenderer::initializeRenderer()
{}
void CoronaRenderer::updateShape(MayaObject *obj)
{}
void CoronaRenderer::updateTransform(MayaObject *obj)
{}
void CoronaRenderer::IPRUpdateEntities()
{}
void CoronaRenderer::reinitializeIPRRendering()
{}

void CoronaRenderer::abortRendering()
{
	if( this->context.core != NULL)
		this->context.core->cancelRender();
}

