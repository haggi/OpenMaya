#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"

#include "MyClasses.h"

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
{}


#ifdef CORONA_RELEASE_ASSERTS
#pragma comment(lib, "PrecompiledLibs_Assert.lib")
#pragma comment(lib, "Corona_Assert.lib")
#else
#pragma comment(lib, "../../lib/PrecompiledLibs_Release.lib")
#pragma comment(lib, "../../lib/Corona_Release.lib")
#endif

using namespace Corona;

// creates a new native material with a random diffuse color
IMaterial* getNativeMtl(Abstract::Settings* settings) 
{
    static int created = 0;
    NativeMtlData data;
    data.components.diffuse.setColor(Rgb(FLT_RAND(), FLT_RAND(), FLT_RAND()));
    if(created++ == 0) {    // first material created gets a texture
        data.components.diffuse.setMap(new MyCheckerMap);
    }
    return data.createMtl(settings);
}

void CoronaRenderer::createScene()
{
    // set the background for all types of rays
    //this->context.scene->setBackground(ColorOrMap(Rgb::WHITE));

	this->defineCamera();
	this->defineGeometry();

    //// create the single geometry group we will use
    //IGeometryGroup* geom = this->context.scene->addGeomGroup();
    //
    //// vertices, normal, and single mapping coordinate for the triangle
    //geom->getVertices().push(Pos(-50, 0, 0));
    //geom->getVertices().push(Pos(-50, 30, 0));
    //geom->getVertices().push(Pos(-50, 30, 30));
    //geom->getNormals().push(Dir(0, 1, 0));
    //geom->getMapCoords().push(Pos(0, 0, 0));
    //geom->getMapCoordIndices().push(0);

    //// first instance with two materials
    //IInstance* instance = geom->addInstance(AffineTm::IDENTITY);
    //instance->addMaterial(IMaterialSet(getNativeMtl(this->context.settings)));
    //instance->addMaterial(IMaterialSet(getNativeMtl(this->context.settings)));
    //

    //// second instance with different scale, translation, and two different materials
    //AffineTm tm2 = AffineTm::IDENTITY;
    //tm2.scale(Dir(0.2f));
    //tm2.translate(Dir(40, 40, 0));
    //instance = geom->addInstance(tm2);
    //instance->addMaterial(IMaterialSet(getNativeMtl(this->context.settings)));
    //instance->addMaterial(IMaterialSet(getNativeMtl(this->context.settings)));

    //// create the objects - a triangle and a sphere
    //SphereData sphere;
    //sphere.materialId = 0;
    //sphere.tm() = AffineTm::IDENTITY;
    //sphere.tm().scale(2);
    //geom->addPrimitive(sphere);

    //// triangle takes indices into the vertices/normals/mapcoords arrays as parameters
    //TriangleData tri;
    //tri.v = AnimatedPosI3(0, 1, 2);
    //tri.n = AnimatedDirI3(0, 0, 0);
    //tri.t[0] = tri.t[1] = tri.t[2] = 0;
    //tri.materialId = 1;
    //geom->addPrimitive(tri);

}


void CoronaRenderer::render()
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  INIT SHADING CORE + SCEME
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICore::initLib();
    
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
    parser.parse("./" CORONA_DEFAULT_CONF_FILENAME, context.settings, true);

    // lets change few parameters, for example resolution, and render stamp
	context.settings->set(PARAM_IMAGE_WIDTH, this->mtco_renderGlobals->imgWidth);
	context.settings->set(PARAM_IMAGE_HEIGHT, this->mtco_renderGlobals->imgHeight);
    context.settings->set(PARAM_IMAGE_REGION_END_X, this->mtco_renderGlobals->imgWidth);
    context.settings->set(PARAM_IMAGE_REGION_END_Y,  this->mtco_renderGlobals->imgHeight);
    context.settings->set(PARAM_PROGRESSIVE_MAX_PASSES, 20);
 
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

    context.core->beginSession(context.scene, context.settings, context.fb, context.logger, "/");
    
    // run the rendering. This function blocks until it is done
    context.core->renderFrame();
	context.isCancelled = true;
    context.core->endSession();
    

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
    ICore::shutdownLib();
	context.core = NULL;

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

