#include "CoronaCore/api/Api.h"
#include "CoronaTestClasses.h"
#include "Corona.h"

//#ifdef CORONA_RELEASE_ASSERTS
//#   pragma comment(lib, "../../lib/PrecompiledLibs_Assert.lib")
//#   pragma comment(lib, "../../lib/Corona_Assert.lib")
//#else
//#   pragma comment(lib, "../../lib/PrecompiledLibs_Release.lib")
//#   pragma comment(lib, "../../lib/Corona_Release.lib")
//#endif

// creates a new native material with a random diffuse color
SharedPtr<IMaterial> getNativeMtl() {
	static int created = 0;
	NativeMtlData data;
	data.components.diffuse.setColor(Rgb(FLT_RAND(), FLT_RAND(), FLT_RAND()));
	if (created++ == 0) {    // first material created gets a texture
		data.components.diffuse.setMap(new MyCheckerMap);
	}
	return data.createMaterial();
}

// fills the scene with some data
void createScene(IScene* scene) {
	// set the background to near white
	scene->setBackground(ColorOrMap(Rgb::WHITE*0.75f));

	// initialize the camera for a perspective view. 
	// If we would use DOF, we would need to initialize additonal members
	scene->getCamera().createPerspective(AnimatedPos(Pos(-25, 25, 12)), AnimatedPos(Pos(0.f, 0.f, -5.f)), AnimatedDir(Dir::UNIT_Z), AnimatedFloat(DEG_TO_RAD(45.f)));

	// create the single geometry group we will use
	IGeometryGroup* geom = scene->addGeomGroup();

	// vertices, normal, and single mapping coordinate for the triangle
	geom->getVertices().push(Pos(-5, -5, 0));
	geom->getVertices().push(Pos(-5, 5, 0));
	geom->getVertices().push(Pos(5, -5, 0));
	geom->getVertices().push(Pos(5, 5, 0));
	geom->getVertices().push(Pos(5, 5, 2));


	geom->getNormals().push(Dir(0, 0, 1));
	geom->getMapCoords().push(Pos(0, 0, 0));
	geom->getMapCoordIndices().push(0);

	// first instance with two materials
	IInstance* instance = geom->addInstance(AffineTm::IDENTITY);
	instance->addMaterial(IMaterialSet(getNativeMtl()));
	instance->addMaterial(IMaterialSet(getNativeMtl()));


	// second instance with different scale, translation, and two different materials
	AffineTm tm2 = AffineTm::IDENTITY;
	tm2.scale(Dir(1.5f));
	tm2.translate(Dir(0, 0, -12));
	instance = geom->addInstance(tm2);
	instance->addMaterial(IMaterialSet(getNativeMtl()));
	instance->addMaterial(IMaterialSet(getNativeMtl()));

	// create the objects - a triangle and a sphere
	SphereData sphere;
	sphere.materialId = 1;
	geom->addPrimitive(sphere);

	// triangle takes indices into the vertices/normals/mapcoords arrays as parameters first create nonanimated one
	TriangleData tri;
	tri.materialId = 0;
	tri.t[0] = tri.t[1] = tri.t[2] = 0;
	tri.v[0][0] = 0; tri.v[0][1] = 2; tri.v[0][2] = 1;
	tri.n[0][0] = 0; tri.n[0][1] = 0; tri.n[0][2] = 0;
	geom->addPrimitive(tri);

	// then animated one
	tri.v.setSegments(1);
	tri.n.setSegments(1);
	tri.v[0][0] = 2; tri.v[0][1] = 3; tri.v[0][2] = 1;
	tri.n[0][0] = 0; tri.n[0][1] = 0; tri.n[0][2] = 0;
	tri.v[1][0] = 2; tri.v[1][1] = 4; tri.v[1][2] = 1;
	tri.n[1][0] = 0; tri.n[1][1] = 0; tri.n[1][2] = 0;
	geom->addPrimitive(tri);

	// one custom light
	scene->addLightShader(new MyLight);

}

void createSceneMini(IScene* scene) {
	scene->setBackground(ColorOrMap(Rgb::WHITE*0.75f));
	scene->getCamera().createPerspective(AnimatedPos(Pos(-25, 25, 12)), AnimatedPos(Pos(0.f, 0.f, -5.f)), AnimatedDir(Dir::UNIT_Z), AnimatedFloat(DEG_TO_RAD(45.f)));
	IGeometryGroup* geom = scene->addGeomGroup();
	SphereData sphere;
	geom->addPrimitive(sphere);
	AffineTm tm2 = AffineTm::IDENTITY;
	IInstance* instance = geom->addInstance(tm2);
	NativeMtlData data;
	data.components.diffuse.setColor(Rgb(FLT_RAND(), FLT_RAND(), FLT_RAND()));
	instance->addMaterial(IMaterialSet(data.createMaterial()));

	// one custom light
	scene->addLightShader(new MyLight);
}


// function that gets called from another thread to do the heavy lifting - scene creating and rendering
void workFunction(Context *context) {

	createScene(context->scene);

	// test that the now ready scene and settings do not have any errors
	context->core->sanityCheck(context->scene);
	context->core->sanityCheck(context->settings);

	context->core->beginSession(context->scene, context->settings, context->fb, context->logger, ICore::AdditionalInfo());

	// run the rendering. This function blocks until it is done
	context->core->renderFrame();

	context->core->endSession();
}

// do actual work here, because of memory leak bookkeeping
void CoronaRenderer::createTestScene() 
{
	Context context;
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
	parser.parseFile("./" CORONA_DEFAULT_CONF_FILENAME, context.settings, ConfParser::CREATE_IF_NONEXISTENT);

	// lets change few parameters, for example resolution, and render stamp
	context.settings->set(PARAM_IMAGE_WIDTH, 800);
	context.settings->set(PARAM_IMAGE_HEIGHT, 600);
	context.settings->set(PARAM_PROGRESSIVE_PASS_LIMIT, 100);

	// add a custom string to the render stamp
	const String renderStamp = "Corona API sample app | Time: %pt | Passes: %pp | Primitives: %si | Rays/s : %pr";

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  RENDER PASSES
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// lets add few render passes. One render pass can be added multiple times with different names
	RenderPassData data;
	data.className = data.name = "NormalsDotProduct";
	context.renderPasses.push(context.core->createRenderPass(data));

	data.className = "zdepth";
	data.name = "Z depth render pass";
	data.zDepth.minDepth = 0;
	data.zDepth.maxDepth = 50;
	context.renderPasses.push(context.core->createRenderPass(data));

	data.name = "Normals discrepancy pass";
	data.className = "NormalsDiscrepancy";
	context.renderPasses.push(context.core->createRenderPass(data));

	data.className = "zdepth";
	data.name = "Second zdepth pass";
	data.zDepth.minDepth = 40;
	data.zDepth.maxDepth = 20;
	context.renderPasses.push(context.core->createRenderPass(data));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  INTERNAL FRAME BUFFER
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create a new framebuffer, and init it before the rendering
	context.fb = context.core->createFb();
	// the settings and render passes need to contain final parameters of the rendering at the time of the call
	context.fb->initFb(context.settings, context.renderPasses);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  GUI FRAME BUFFER - optional
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create a class used for communication with the frame buffer
	MyVfbCallbacks callbacks;
	callbacks.core = context.core;
	// create and display VFB
	//context.core->getWxVfb().renderStarted(context.fb, &callbacks, IWxVfb::EnviroConfig());


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  HEAVY LIFTING IN A SEPARATE THREAD
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create a new, reusable thread, and use it to do the rest, to keep the GUI responsive while we do the heavy lifting
	//StandbyThread workThread;
	//workThread.invoke(workFunction, &context, true);
	
	workFunction(&context);
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, TRUE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  MESSAGE LOOP
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// run the message loop. Handle the framebuffer from time to time (2000ms)
	//Timer vfbTimer(2000);
	//MSG msg;
	//while (!callbacks.isWindowClosed) {

	//	if (vfbTimer.check()) {
	//		// update the renderstamp with an actual version
	//		context.fb->updateRenderStamp(renderStamp, true);
	//		// force VFB redraw
	//		context.core->getWxVfb().refresh();
	//		vfbTimer.reset();
	//	}

	//	// standard windows message dispatch
	//	while (PeekMessage(&msg, NULL, 0, 0, TRUE)) {
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}

	//	// sleep so we dont waste too much computing time with GUI
	//	Sleep(30);
	//}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////  CLEANUP
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// necessary so we dont delete anything while other threads are still running
	//workThread.waitForThis();

	// dispose of GUI framebuffer
	//context.core->getWxVfb().renderFinished();
	//context.core->getWxVfb().showWindow(false);

	// delete what we have created and call deallocation functions for objects the core has created
	delete context.logger;
	delete context.settings;

	context.renderPasses.clear();
	context.core->destroyScene(context.scene);
	context.core->destroyFb(context.fb);
	ICore::destroyInstance(context.core);
}

//int main() {
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	////  INIT SHADING CORE + SCEME
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ICore::initLib(APP_API_SAMPLES);
//
//	// Try to obtain license if there is none
//	if (!ICore::getLicenseInfo().isUsable()) {
//		// this should be called ONLY when running with GUI, never when running in idle mode, as it brings out a blocking UI dialog
//		ICore::doLicensePopup();
//	}
//
//	LicenseInfo license = ICore::getLicenseInfo();
//	if (!license.isUsable()) {
//		std::cerr << "ERROR: License is needed to run Corona.\n" << license.getDescription() << std::endl;
//		return 1;
//	}
//	else {
//		std::cout << license.getDescription() << std::endl;
//	}
//
//	main2();
//
//	ICore::shutdownLib();
//	return 0;
//}