#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"
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
	//Corona::ICore::shutdownLib();
}

#ifdef CORONA_RELEASE_ASSERTS
#pragma comment(lib, "PrecompiledLibs_Assert.lib")
#pragma comment(lib, "Corona_Assert.lib")
#else
#pragma comment(lib, "PrecompiledLibs_Release.lib")
#pragma comment(lib, "Corona_Release.lib")
#endif

using namespace Corona;

void CoronaRenderer::saveImage()
{
	Corona::Bitmap<Corona::Rgb, false> bitmap(this->context.fb->getImageSize());
    Corona::Bitmap<float, false> alpha(this->context.fb->getImageSize());

	
	bool doToneMapping = true;
	bool showRenderStamp = true;
    for(int i = 0; i < bitmap.getHeight(); ++i) 
	{
        const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
        this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, &bitmap[pixel], &alpha[pixel]);
    }
	
	//this->context.fb->dumpExr()

    //// since we get the colors from frame buffer after color mapping, that includes gamma correction, they are not 
    //// in linear space (the "false" argument)
	this->mtco_renderGlobals->getImageName();
	Corona::String filename = this->mtco_renderGlobals->imageOutputFile.asChar();
	logger.debug(MString("Saving image as ") +  this->mtco_renderGlobals->imageOutputFile);
	
	bool isLinear = false;
	
	std::string imgFormatExt = this->mtco_renderGlobals->imageOutputFile.toLowerCase().asChar();
	std::vector<std::string> fileParts;
	pystring::split(imgFormatExt, fileParts, ".");
	std::string ext = fileParts.back();

	logger.debug(MString("Extension: ") + ext.c_str());
	if( (ext != "exr") && (ext != "png") && (ext != "bmp") && (ext != "jpg"))
	{
		logger.warning(MString("Filename does not contain a valid extension: ") + this->mtco_renderGlobals->imageOutputFile + " adding exr.");
		filename += ".exr";
	}
	Corona::saveImage(filename, Corona::RgbBitmapIterator<false>(bitmap, &alpha), isLinear, Corona::IMAGE_DETERMINE_FROM_EXT);
	
}

void CoronaRenderer::createScene()
{
	this->defineCamera();
	this->defineGeometry();
	this->defineLights();
	this->defineEnvironment();
}

//IMaterial* getNativeMtl() 
//{
//    static int created = 0;
//    NativeMtlData data;
//    data.components.diffuse.setColor(Rgb(FLT_RAND(), FLT_RAND(), FLT_RAND()));
//    if(created++ == 0) {    // first material created gets a texture
//        data.components.diffuse.setMap(new MyCheckerMap);
//    }
//    return data.createMaterial();
//}
//
//// fills the scene with some data (a triangle and a sphere, with 2 instances)
//void createScene(IScene* scene) {
//    // set the background for all types of rays
//    scene->setBackground(ColorOrMap(Rgb::WHITE));
//
//    // initialize the camera for a perspective view. 
//    // If we would use DOF, we would need to initialize additonal members
//    scene->getCamera().createPerspective(AnimatedPos(Pos(10, 10, 10)), AnimatedPos(Pos(-4.f, 0.f, 0.f)), AnimatedDir(Dir::UNIT_Z), AnimatedFloat(DEG_TO_RAD(45.f)));
//
//    // create the single geometry group we will use
//    IGeometryGroup* geom = scene->addGeomGroup();
//    
//    // vertices, normal, and single mapping coordinate for the triangle
//    geom->getVertices().push(Pos(-5, 0, 0));
//    geom->getVertices().push(Pos(-5, 3, 0));
//    geom->getVertices().push(Pos(-5, 3, 3));
//    geom->getNormals().push(Dir(0, 1, 0));
//    geom->getMapCoords().push(Pos(0, 0, 0));
//    geom->getMapCoordIndices().push(0);
//
//    // first instance with two materials
//    IInstance* instance = geom->addInstance(AffineTm::IDENTITY);
//    instance->addMaterial(IMaterialSet(getNativeMtl()));
//    instance->addMaterial(IMaterialSet(getNativeMtl()));
//    
//
//    // second instance with different scale, translation, and two different materials
//    AffineTm tm2 = AffineTm::IDENTITY;
//    tm2.scale(Dir(2.f));
//    tm2.translate(Dir(-2, -2, 1));
//    instance = geom->addInstance(tm2);
//    instance->addMaterial(IMaterialSet(getNativeMtl()));
//    instance->addMaterial(IMaterialSet(getNativeMtl()));
//
//    // create the objects - a triangle and a sphere
//    SphereData sphere;
//    sphere.materialId = 0;
//    geom->addPrimitive(sphere);
//
//    // triangle takes indices into the vertices/normals/mapcoords arrays as parameters
//    TriangleData tri;
//    tri.v = AnimatedPosI3(0, 1, 2);
//    tri.n = AnimatedDirI3(0, 0, 0);
//    tri.t[0] = tri.t[1] = tri.t[2] = 0;
//    tri.materialId = 1;
//    geom->addPrimitive(tri);
//}
//
//// function that gets called from another thread to do the heavy lifting - scene creating and rendering
//void workFunction(UINT_PTR ptr) {
//    //Context* context = (Context*)ptr;
//	CoronaRenderer *cr  = (CoronaRenderer*)ptr;
//    //createScene(cr->context.scene);
//	cr->createScene();
//
//    // test that the now ready scene and settings do not have any errors
//    cr->context.core->sanityCheck(cr->context.scene);
//    cr->context.core->sanityCheck(cr->context.settings);
//
//    cr->context.core->beginSession(cr->context.scene, cr->context.settings, cr->context.fb, cr->context.logger, "/");
//    
//    // run the rendering. This function blocks until it is done
//    cr->context.core->renderFrame();
//
//    cr->context.core->endSession();
//}
//
//void CoronaRenderer::doit()
//{
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    ////  INIT SHADING CORE + SCEME
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //ICore::initLib(false);
//    
//    //Context con;
//    context.core = ICore::createInstance();
//    context.scene = context.core->createScene();
//    context.logger = new mtco_Logger(context.core);
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    ////  SETTINGS
//    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    context.settings = new MySettings();
//
//    // populate the settings with parameters from a configuration file. If the file does not exist, a new one 
//    // is created with default values.
//	Corona::String confPath = (getRendererHome() + "bin/").asChar();
//    ConfParser parser;
//    parser.parseFile(confPath + CORONA_DEFAULT_CONF_FILENAME, context.settings, ConfParser::CREATE_IF_NONEXISTENT);
//
//	this->defineSettings();
//	this->definePasses();
//
//    context.fb = context.core->createFb();
//    context.fb->initFb(context.settings, context.renderPasses);
//
//    StandbyThread workThread;
//    workThread.invoke(workFunction, this, true);
//
//	workThread.waitForThis();
//    
//    delete context.logger;
//    delete context.settings;
//    
//    for(auto it = context.renderPasses.begin(); it != context.renderPasses.end(); ++it) {
//        context.core->destroyRenderPass(*it);
//    }
//    context.core->destroyScene(context.scene);
//    context.core->destroyFb(context.fb);
//    ICore::destroyInstance(context.core);
//    //ICore::shutdownLib();
//
//
//}

void CoronaRenderer::render()
{

	std::string oslShaderPath = (getRendererHome() + "shaders").asChar();
	logger.debug(MString("setting osl shader search path to: ") + oslShaderPath.c_str());
	this->oslRenderer.setShaderSearchPath(oslShaderPath);
	this->oslRenderer.setup();

	//doit();
	//return;
	logger.debug(MString("Starting corona rendering..."));

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  INIT SHADING CORE + SCEME
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Context context;
    context.core = ICore::createInstance();
    context.scene = context.core->createScene();
    context.logger = new mtco_Logger(context.core);

	logger.debug(MString("core/scene/logger..."));

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  SETTINGS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    context.settings = new MySettings();

    // populate the settings with parameters from a configuration file. If the file does not exist, a new one 
    // is created with default values.
    ConfParser parser;
	
	Corona::String resPath = (getRendererHome() + "ressources/").asChar();
	logger.debug(MString("parser: ") + (resPath + CORONA_DEFAULT_CONF_FILENAME).cStr());
    parser.parseFile(resPath + CORONA_DEFAULT_CONF_FILENAME, context.settings, ConfParser::CREATE_IF_NONEXISTENT);

	logger.debug(MString("defineSettings..."));

	this->clearMaterialLists();
	this->defineSettings();
	logger.debug(MString("definePasses..."));
	this->definePasses();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  INTERNAL FRAME BUFFER
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create a new framebuffer, and init it before the rendering
	logger.debug(MString("createFb..."));
    context.fb = context.core->createFb();

	//colorMappingData = context.settings->getColorMapping();
	//this->context.fb->setColorMapping(colorMappingData);

    // the settings and render passes need to contain final parameters of the rendering at the time of the call
	logger.debug(MString("initFb..."));
    context.fb->initFb(context.settings, context.renderPasses);
	//Corona::ColorMappingData cmData;
	//cmData.gamma = 1.0f;
	//context.fb->setColorMapping(cmData);
	logger.debug(MString("createScene..."));

	createScene();

	logger.debug(MString("createScene done..."));

    // test that the now ready scene and settings do not have any errors
	logger.debug(MString("sanityCheck scene..."));
    context.core->sanityCheck(context.scene);
	logger.debug(MString("sanityCheck settings..."));
	//this->sanityCheck(context.settings);
    context.core->sanityCheck(context.settings);

	Corona::String basePath = (this->mtco_renderGlobals->basePath + "/corona/").asChar();
	logger.debug(MString("beginSession..."));
	ICore::AdditionalInfo info;
	info.defaultFilePath = basePath;
    context.core->beginSession(context.scene, context.settings, context.fb, context.logger, info);
    
    // run the rendering. This function blocks until it is done
	logger.debug(MString("renderFrame..."));
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
}

void CoronaRenderer::initializeRenderer()
{}

void CoronaRenderer::updateShape(MayaObject *obj)
{
	if( obj->instanceNumber > 0)
		return;

	if( obj->mobject.hasFn(MFn::kMesh))
	{
		// the vertex positions are taken from the mesh for the start time, deformed or not
		if( this->mtco_renderGlobals->isMbStartStep)
		{
			this->updateMesh((mtco_MayaObject *)obj);
		}else{
		// further updates are only done if the mesh is deformed
		if( obj->isShapeConnected())
			this->updateMesh((mtco_MayaObject *)obj);
		}
	}
}

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

