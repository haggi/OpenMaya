#include "mtco_mayaRenderer.h"
#include "../Corona/CoronaShaders.h"
#include "../Corona/CoronaCamera.h"
#include "../Corona/CoronaGeometry.h"
#include "../Corona/CoronaUtils.h"
#include "../Corona/CoronaMap.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "world.h"

#if MAYA_API_VERSION >= 201600

#include <vector>
#include <maya/MPxRenderer.h>
#include <maya/MGlobal.h>

#define kNumChannels 4
#define CORONA_DEFAULT_CONFIG_FILENAME    "Config-" + Corona::toStr(Corona::PropertyDescriptor::descriptorsHash()) + ".conf"

float FLT_RAND() {
	return float(rand()) / RAND_MAX;
}

mtco_MayaRenderer::mtco_MayaRenderer()
{
	width = height = initialSize;
	renderBuffer = (float*)malloc(width*height*kNumChannels*sizeof(float));

	Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
	if (!li.isUsable())
	{
		return;
	}
	oslRenderer->setup();

	context = new Context();

	context->core = Corona::ICore::createInstance();
	context->scene = context->core->createScene();

	context->logger = new mtco_Logger(context->core);
	context->settings = new Settings();

	Corona::ConfParser parser;
	Corona::String resPath = (getRendererHome() + "resources/").asChar();
	parser.parseFile(resPath + CORONA_DEFAULT_CONFIG_FILENAME, context->settings, Corona::ConfParser::CREATE_IF_NONEXISTENT);

	Corona::ICore::MtlSceneConfig config;
	config.lightsSize = 1.0f;
	config.lightsIntensity = 1.0f;

	context->settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_PROGRESSIVE);
	context->settings->set(Corona::PARAM_MINIMIZE_PRECOMP, true);
	//limit the passes
	context->settings->set(Corona::PARAM_PROGRESSIVE_PASS_LIMIT, 10);

	context->settings->set(Corona::PARAM_IMAGE_WIDTH, width);
	context->settings->set(Corona::PARAM_IMAGE_HEIGHT, height);

	context->core->sanityCheck(context->settings);
	context->fb = context->core->createFb();
	context->fb->initFb(context->settings, context->renderPasses);

	context->scene->getCamera().createPerspective(Corona::AnimatedPos(Corona::Pos(-25, 25, 12)), Corona::AnimatedPos(Corona::Pos(0.f, 0.f, -5.f)), Corona::AnimatedDir(Corona::Dir::UNIT_Z), Corona::AnimatedFloat(Corona::DEG_TO_RAD(45.f)));

	context->core->sanityCheck(context->scene);

	context->core->beginSession(context->scene, context->settings, context->fb, context->logger, Corona::ICore::AdditionalInfo());
	good = true;
}

mtco_MayaRenderer::~mtco_MayaRenderer()
{
	if (!good)return;
	delete oslRenderer;
	finishRendering();
	context->core->endSession();
	delete context->logger;
	delete context->settings;
	context->core->destroyScene(context->scene);
	context->core->destroyFb(context->fb);
	Corona::ICore::destroyInstance(context->core);
}

bool mtco_MayaRenderer::isRunningAsync()
{
	Logging::debug("isRunningAsync");
	return true;
}

void* mtco_MayaRenderer::creator()
{
	return new mtco_MayaRenderer();
}

void mtco_MayaRenderer::render()
{
	Logging::debug("Rendering...");
	ProgressParams progressParams;
	progressParams.progress = 0.0f;
	progress(progressParams);
	isRendering = true;
	context->core->renderFrame();
	isRendering = false;
	progressParams.progress = 1.0f;
	progress(progressParams);
}

static void startRenderThread(mtco_MayaRenderer* renderPtr)
{
	renderPtr->render();
}

static void startFbThread(mtco_MayaRenderer* renderPtr)
{
	if (!renderPtr->good)return;
	while (renderPtr->isRendering)
	{
		Logging::debug("FbThread sleeping...");
		std::this_thread::sleep_for(std::chrono::milliseconds(renderPtr->refreshInteraval * 1000));
		renderPtr->framebufferCallback();
	}
}

void mtco_MayaRenderer::finishRendering()
{
	if (good)
		this->context->core->cancelRender();
	if (this->renderThread.joinable())
		this->renderThread.join();
	if (this->fbThread.joinable())
		this->fbThread.join();
}

MStatus mtco_MayaRenderer::startAsync(const JobParams& params)
{
	//mtco_MayaRenderer::running = true;
	//render();
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::stopAsync()
{
	Logging::debug("stopAsync");
	finishRendering();
	return MS::kSuccess;
}

MStatus mtco_MayaRenderer::beginSceneUpdate()
{
	Logging::debug("beginSceneUpdate");
	finishRendering();
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::endSceneUpdate()
{
	Logging::debug("endSceneUpdate");
	completlyInitialized = true;
	if (good)
		this->context->core->uncancelRender();
	this->renderThread = std::thread(startRenderThread, this);
	this->fbThread = std::thread(startFbThread, this);
	return MStatus::kSuccess;
};

static float demoRot = 0.0f;

MStatus mtco_MayaRenderer::translateMesh(const MUuid& id, const MObject& node)
{
	Logging::debug("translateMesh");

	Corona::IGeometryGroup *group = nullptr;
	// remove geogroup if it exists
	for (auto& gg : geometryGroupArray)
	{
		if (gg.id == id)
		{
			Logging::debug(MString("Found existing geometry group for mesh ") + getObjectName(node));
			context->scene->deleteGeomGroup(gg.geoGrp);
			gg.geoGrp = context->scene->addGeomGroup();
			group = gg.geoGrp;
		}
	}
	
	// if we do not have an entry, create a new one
	if (group == nullptr)
	{
		Logging::debug(MString("Found no existing geometry group for mesh ") + getObjectName(node) + " creating new one.");
		GeoGroupStruct gg;
		gg.geoGrp = context->scene->addGeomGroup();
		gg.mobject = node;
		gg.id = id;
		geometryGroupArray.push_back(gg);
		group = gg.geoGrp;
	}

	Logging::debug(MString("Scene has ") + context->scene->numGeomGroups() + " geometry groups.");

	//Corona::SphereData sphere;
	//sphere.materialId = 0;
	//group->addPrimitive(sphere);	
	defineMesh(group, node);
	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "mesh";
	objectArray.push_back(idn);
	lastShape = node;

	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
	Logging::debug("translateLightSource");

	Corona::IGeometryGroup *group = nullptr;
	for (auto gg : geometryGroupArray)
	{
		if (gg.mobject == node)
		{
			Logging::debug(MString("Found existing geometry group for mesh ") + getObjectName(node));
			context->scene->deleteGeomGroup(gg.geoGrp);
			gg.geoGrp = context->scene->addGeomGroup();
			group = gg.geoGrp;
		}
	}
	if (group == nullptr)
	{
		GeoGroupStruct gg;
		gg.geoGrp = context->scene->addGeomGroup();
		gg.mobject = node;
		geometryGroupArray.push_back(gg);
		group = gg.geoGrp;
	}

	defineStdPlane(group);
	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "light";
	objectArray.push_back(idn);
	lastShape = node;

	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateCamera(const MUuid& id, const MObject& node)
{
	Logging::debug("translateCamera");
	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "camera";
	objectArray.push_back(idn);
	lastShape = node;
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
	Logging::debug("translateEnvironment");
	IdNameStruct idn;
	idn.id = id;
	idn.name = "environment";
	objectArray.push_back(idn);
	lastShape = MObject::kNullObj;
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
	Logging::debug("translateTransform");
	IdNameStruct ids;
	Corona::AnimatedAffineTm atm;
	MMatrix m = matrix;
	setAnimatedTransformationMatrix(atm, m);

	for (auto idn : objectArray)
	{
		//Logging::debug(MString("Searching id in obj array: idn.id ") + idn.id + " id " + id + " childId " + childId);
		if (idn.mobject == lastShape)
		{
			Logging::debug(MString("Found id for obj ") + idn.name);
			if (idn.mobject.hasFn(MFn::kCamera))
			{			
				Logging::debug(MString("Camera Transform"));
				Corona::CameraData& camera = this->context->scene->getCamera();
				float deviceAspect = width / height;
				updateCamera(camera, idn.mobject, matrix, deviceAspect);
			}
			if (idn.mobject.hasFn(MFn::kMesh))
			{
				Logging::debug(MString("Mesh Transform"));
				for (auto& gg : geometryGroupArray)
				{
					if (gg.mobject == lastShape)
					{
						// let's see if we have a predfined shader
						Corona::SharedPtr<Corona::IMaterial> mat;
						for (auto sidn : objectArray)
						{
							if (sidn.name == "coronaMaterial")
							{
								if (sidn.id == id)
								{
									mat = sidn.material;
								}
							}
						}

						gg.geoGrp->deleteAllInstances();

						Corona::NativeMtlData data;
						data.components.diffuse.setColor(Corona::Rgb(FLT_RAND(), FLT_RAND(), FLT_RAND()));
						if (mat.getReference() == nullptr)
							mat = data.createMaterial();
						Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
						gg.instance = gg.geoGrp->addInstance(atm, nullptr, nullptr);
						gg.instance->addMaterial(ms);
						Logging::debug(MString("Mesh has ") + gg.geoGrp->numInstances() + " instances");
						ids.mobject = gg.mobject;
						lastShape = MObject::kNullObj;
					}
				}
			}

			if (idn.mobject.hasFn(MFn::kAreaLight))
			{
				Logging::debug(MString("Area Light Transform"));
				Corona::NativeMtlData data;
				MFnDependencyNode depFn(idn.mobject);
				MColor lightColor = getColorAttr("color", depFn);
				float intensity = getFloatAttr("intensity", depFn, 1.0f);
				lightColor *= intensity;
				data.emission.color.setColor(Corona::Rgb(lightColor.r, lightColor.g, lightColor.b ));
				data.castsShadows = false; // a light should never cast shadows
				data.emission.disableSampling = false;
				Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
				Corona::IMaterialSet ms = Corona::IMaterialSet(mat);

				Corona::IGeometryGroup *group = nullptr;
				GeoGroupStruct *gg = nullptr;

				for (uint i = 0; i < geometryGroupArray.size(); i++)
				{
					gg = &geometryGroupArray[i];
					if (gg->mobject == idn.mobject)
					{
						Logging::debug(MString("Found existing geometry group for areaLight transform translation "));
						group = gg->geoGrp;
					}
				}
				if (group != nullptr)
				{
					gg->instance = group->addInstance(atm, nullptr, nullptr);
					gg->instance->addMaterial(ms);
				}

			}
		}
	}
	ids.id = id;
	ids.name = "transform";
	objectArray.push_back(ids);

	ids.id = childId;
	ids.name = "transformChild";
	objectArray.push_back(ids);
	return MStatus::kSuccess;

	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateShader(const MUuid& id, const MObject& node)
{
	Logging::debug(MString("translateShader: "));
	IdNameStruct idn;
	idn.id = id;
	idn.name = "material";
	idn.mobject = node;
	objectArray.push_back(idn);
	lastShape = node;
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
	Logging::debug(MString("setProperty bool: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
	Logging::debug(MString("setProperty int: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
	Logging::debug(MString("setProperty float: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
	mtco_FileLoader fl;
	Logging::debug(MString("setProperty string: ") + name + " " + value);

	MString mayaRoot = getenv("MAYA_LOCATION");
	MString blackImage = mayaRoot + "/presets/Assets/IBL/black.exr";

	IdNameStruct idNameObj;
	for (auto idobj : objectArray)
	{
		if (idobj.id == id)
		{
			Logging::debug(MString("Found id object for string property: ") + idobj.name);
			if (idobj.name == "environment")
			{
				if (name == "imageFile")
				{
					Logging::debug(MString("Setting environment image file to: ") + value);
					Corona::SharedPtr<Corona::Abstract::Map> texmap;
					if (value.length() == 0)
						texmap = fl.loadBitmap(blackImage.asChar());
					else
						texmap = fl.loadBitmap(value.asChar());
					context->scene->setBackground(Corona::ColorOrMap(0.0f, texmap));
				}
			}
		}
	}

	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
	Logging::debug("setShader");
	IdNameStruct idnShader;
	MObject meshObject;

	// first find the shader node
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == shaderId)
		{
			idnShader = objectArray[i];
			break;
		}
	}
	
	IdNameStruct idnGeo;
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == id)
		{
			idnGeo = objectArray[i];
			break;
		}
	}

	if ((idnShader.mobject == MObject::kNullObj) || (idnGeo.mobject == MObject::kNullObj))
	{
		Logging::debug(MString("Object not fund for shader update."));
		return MS::kFailure;
	}

	Corona::IGeometryGroup *group = nullptr;
	GeoGroupStruct *gg = nullptr;

	for (uint i = 0; i < geometryGroupArray.size(); i++)
	{
		gg = &geometryGroupArray[i];
		if (gg->mobject == idnGeo.mobject)
		{
			Logging::debug(MString("Found existing geometry group for shader update "));
			break;
		}
	}
	
	if ( gg == nullptr)
	{
		Logging::debug(MString("Geo Group not fund for shader update."));
		return MS::kFailure;
	}

	gg->instance->clearMaterials();
	Corona::SharedPtr<Corona::IMaterial> mat = defineCoronaMaterial(idnShader.mobject, nullptr, oslRenderer);
	

	// first find the shader node
	bool alreadyExists = false;
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == id)
		{
			if (objectArray[i].name == "coronaMaterial")
			{
				objectArray[i].material = mat;
				alreadyExists = true;
				break;
			}
		}
	}
	if (!alreadyExists)
	{
		IdNameStruct idn;
		idn.id = id;
		idn.name = "coronaMaterial";
		idn.mobject = idnGeo.mobject;
		idn.material = mat;
		objectArray.push_back(idn);
	}
	Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
	gg->instance->addMaterial(ms);
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::setResolution(unsigned int w, unsigned int h)
{
	Logging::debug(MString("setResolution ") + w + " " + h);
	this->width = w;
	this->height = h;
	this->renderBuffer = (float*)realloc(this->renderBuffer, w*h*kNumChannels*sizeof(float));

	if (good)
	{
		context->settings->set(Corona::PARAM_IMAGE_WIDTH, width);
		context->settings->set(Corona::PARAM_IMAGE_HEIGHT, height);
		context->core->destroyFb(context->fb);
		context->fb = context->core->createFb();
		context->fb->initFb(context->settings, context->renderPasses);
	}
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::destroyScene()
{
	Logging::debug("destroyScene");
	finishRendering();
	ProgressParams progressParams;
	progressParams.progress = -1.0f;
	progress(progressParams);
	objectArray.clear();
	for (auto& gg : geometryGroupArray)
	{
		context->scene->deleteGeomGroup(gg.geoGrp);
	}
	geometryGroupArray.clear();
	return MStatus::kSuccess;
};
bool mtco_MayaRenderer::isSafeToUnload()
{
	Logging::debug("isSafeToUnload");
	return true;
};

void mtco_MayaRenderer::framebufferCallback()
{
	Logging::debug("framebufferCallback");

	Corona::Pixel p = context->fb->getImageSize();
	if ((p.x != width) || (p.y != height))
	{
		Logging::error("ImageSize is different");
		return;
	}

	size_t numPixels = width * height;
	uint numPixelsInRow = width;
	bool doToneMapping = false;
	Corona::Pixel firstPixelInRow(0, 0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	for (uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		try{
			context->fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, false, outColors, outAlpha);
		}
		catch (char *errorMsg){
			Logging::error(errorMsg);
			break;
		}
		uint rowPos = (height - 1 - rowId) * numPixelsInRow;
		for (int x = 0; x < numPixelsInRow; x++)
		{
			int index = (rowPos + x) * kNumChannels;
			*(renderBuffer + index) = outColors[x].r();
			*(renderBuffer + index + 1) = outColors[x].g();
			*(renderBuffer + index + 2) = outColors[x].b();
			*(renderBuffer + index + 3) = 1.0f;
		}
	}
	delete[] outColors;
	delete[] outAlpha;

	refreshParams.bottom = 0;
	refreshParams.top = height - 1;
	refreshParams.bytesPerChannel = sizeof(float);
	refreshParams.channels = kNumChannels;
	refreshParams.left = 0;
	refreshParams.right = width - 1;
	refreshParams.width = width;
	refreshParams.height = height;
	refreshParams.data = renderBuffer;
	refresh(refreshParams);
}
#endif
