#include "mtco_swatchRenderer.h"

#include <maya/MImage.h>
#include <maya/MDistance.h>
#include <maya/MColor.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "swatchesRenderer/swatchGeometry.h"
#include "../Corona/CoronaShaders.h"

static Logging logger;
static bool done;

mtco_SwatchRendererInterface::mtco_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
{
#ifdef _DEBUG
	logger.setLogLevel(Logging::Debug);
#endif

	logger.debug("------------mtco_SwatchRendererInterface created.");
	this->imgDone = false;
	this->inProgress = false;

	this->dependNode = dependNode;
	this->renderNode = renderNode;
	this->swatchWidth = imageResolution;
	this->imageData = (float *)malloc(sizeof(float)* this->swatchWidth * this->swatchWidth * 4);

	// testing
	int index = 0;
	for (int y = 0; y < this->swatchWidth; y++)
	{
		for (int x = 0; x < this->swatchWidth; x++)
		{
			//int index = y * this->swatchWidth + x;
			this->imageData[index++] = rnd();
			this->imageData[index++] = rnd();
			this->imageData[index++] = rnd();
			this->imageData[index++] = 1.0f;
		}
	}
}

mtco_SwatchRendererInterface::~mtco_SwatchRendererInterface()
{
	if (this->imageData != NULL)
		free(this->imageData);
	this->imageData = NULL;
}

void mtco_SwatchRendererInterface::defineSettings()
{
	context.settings->set(Corona::PARAM_IMAGE_WIDTH, this->swatchWidth);
	context.settings->set(Corona::PARAM_IMAGE_HEIGHT, this->swatchWidth);
	context.settings->set(Corona::PARAM_PROGRESSIVE_TIME_LIMIT, 1000);
	context.settings->set(Corona::PARAM_PROGRESSIVE_MAX_PASSES, 3);
	context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_PROGRESSIVE);

	//context.settings->set(Corona::PARAM_EXPORT_PATH, "C:/daten/3dprojects/mayaToCorona/data/swatch.scn");
}

void mtco_SwatchRendererInterface::init()
{
	logger.debug("SwatchRendererInterface init called.");
}

void mtco_SwatchRendererInterface::loadGeometry(){}

void mtco_SwatchRendererInterface::fillDummySwatch(MImage& image)
{
	const int res(swatchWidth);
	float rndR = rnd();
	float rndG = rnd();
	float rndB = rnd();

	float *pixels = image.floatPixels();
	int index = 0;
	for (int y = 0; y < res; y++)
	{
		for (int x = 0; x < res; x++)
		{
			float fac = float(y) / res;
			pixels[index++] = fac * rndR;
			pixels[index++] = fac * rndG;
			pixels[index++] = fac * rndB;
			pixels[index++] = 1.0f;
		}
	}
}

void mtco_SwatchRendererInterface::createCoronaScene()
{
	context.scene->setBackground(Corona::ColorOrMap(Corona::Rgb::WHITE*0.5f));
	Corona::AnimatedPos origin = Corona::AnimatedPos(Corona::Pos(0.0f, -2.4f, 4.f));
	Corona::AnimatedPos target = Corona::AnimatedPos(Corona::Pos(0.0f, 0.0f, 0.0f));
	context.scene->getCamera().createPerspective(origin, target, Corona::AnimatedDir(Corona::Dir::UNIT_Z), Corona::AnimatedFloat(Corona::DEG_TO_RAD(30.0f)));

	Corona::IGeometryGroup* sphere = context.scene->addGeomGroup();

	int pindex = 0, nindex = 0;
	for (int i = 0; i < sphere_numPoints; i++)
	{
		int index = i * 3;
		MPoint p(sphere_points[index], sphere_points[index + 1], sphere_points[index + 2]);
		sphere->getVertices().push(Corona::Pos(p.x, p.y, p.z));
	}
	for (int i = 0; i < sphere_numNormals; i++)
	{
		int index = i * 3;
		MVector n(sphere_normals[index], sphere_normals[index + 1], sphere_normals[index + 2]);
		sphere->getNormals().push(Corona::Dir(n.x, n.y, n.z));
	}
	int uvindex = 0;
	for (int i = 0; i < sphere_numUvs; i++)
	{
		int index = i * 2;
		sphere->getMapCoords().push(Corona::Pos(sphere_uvs[index], sphere_uvs[index + 1], 0.0));
		sphere->getMapCoordIndices().push(sphere->getMapCoordIndices().size());
	}

	pindex = 0;
	nindex = 0;
	uvindex = 0;
	for (int i = 0; i < sphere_numTris; i++)
	{
		Corona::TriangleData tri;
		int index = i * 3;
		tri.materialId = 0;
		tri.v[0][0] = sphere_triPointIds[index];
		tri.v[0][1] = sphere_triPointIds[index + 1];
		tri.v[0][2] = sphere_triPointIds[index + 2];
		tri.t[0] = sphere_triUvIds[index];
		tri.t[1] = sphere_triUvIds[index + 1];
		tri.t[2] = sphere_triUvIds[index + 2];
		tri.n[0][0] = sphere_triNormalIds[index];
		tri.n[0][1] = sphere_triNormalIds[index + 1];
		tri.n[0][2] = sphere_triNormalIds[index + 2];
		sphere->addPrimitive(tri);
	}
	//Corona::AffineTm atm(Corona::AffineTm::IDENTITY);
	float f[4][4] = { 1.0, 0, 0.0, 0,
					  0.0, 0, 1.0, 0,
					    0, -1.0, 0, 0,
					  0, 0, 0, 1.0 };
	MMatrix m(f);
	Corona::Float4 row1(1.0, 0.0, 0.0, 0.0);
	Corona::Float4 row2(0.0, 0.0, -1.0, 0.0);
	Corona::Float4 row3(0.0, 1.0, 0.0, 0.0);
	Corona::AffineTm atm = Corona::AffineTm(row1, row2, row3);
	Corona::IInstance* instance = sphere->addInstance(atm);

	// ------------ Shader assignment ----------------

	//MFnDependencyNode depFn(dependNode);
	////MColor col(1.0,0.0,0.0);
	//MColor col = getColorAttr("diffuse", depFn);
	//Corona::NativeMtlData sdata;
	//sdata.components.diffuse.setColor(Corona::Rgb(col.r, col.g, col.b));
	//sdata.components.reflect.setColor(1.0f);
	//col = getColorAttr("emissionColor", depFn);
	//Corona::ColorOrMap com  = Corona::ColorOrMap(Corona::Rgb(col.r, col.g, col.b), NULL);
	//sdata.emission.color.setColor(Corona::Rgb(rnd(), rnd(), rnd()));
	Corona::SharedPtr<Corona::IMaterial> mat = defineCoronaMaterial(dependNode);

	//Corona::SharedPtr<Corona::IMaterial> mat = sdata.createMaterial();
	instance->addMaterial(Corona::IMaterialSet(mat));

	// -------------- box ----------------------

	Corona::IGeometryGroup* box = context.scene->addGeomGroup();
	
	for (int i = 0; i < box_numPoints; i++)
	{
		int index = i * 3;
		MPoint p(box_points[index], box_points[index + 1], box_points[index + 2]);
		box->getVertices().push(Corona::Pos(p.x, p.y, p.z));
	}
	for (int i = 0; i < box_numNormals; i++)
	{
		int index = i * 3;
		MVector n(box_normals[index], box_normals[index + 1], box_normals[index + 2]);
		box->getNormals().push(Corona::Dir(n.x, n.y, n.z));
	}
	
	for (int i = 0; i < box_numUvs; i++)
	{
		int index = i * 2;
		box->getMapCoords().push(Corona::Pos(box_uvs[index], box_uvs[index + 1], 0.0));
		box->getMapCoordIndices().push(box->getMapCoordIndices().size());
	}

	pindex = 0;
	nindex = 0;
	uvindex = 0;
	int box_numTris = 36/3;
	for (int i = 0; i < box_numTris; i++)
	{
		int index = i * 3;
		Corona::TriangleData tri;
		tri.materialId = 0;
		tri.v[0][0] = box_triPointIds[index];
		tri.v[0][1] = box_triPointIds[index + 1];
		tri.v[0][2] = box_triPointIds[index + 2];
		tri.t[0] = box_triUvIds[index];
		tri.t[1] = box_triUvIds[index + 1];
		tri.t[2] = box_triUvIds[index + 2];
		tri.n[0][0] = box_triNormalIds[index];
		tri.n[0][1] = box_triNormalIds[index + 1];
		tri.n[0][2] = box_triNormalIds[index + 2];
		box->addPrimitive(tri);
	}

	Corona::IInstance* binstance = box->addInstance(atm, NULL, NULL);

	Corona::NativeMtlData data;
	data.components.diffuse.setMap(new CheckerMap);
	Corona::SharedPtr<Corona::IMaterial> bmat = data.createMaterial();
	binstance->addMaterial(Corona::IMaterialSet(bmat));
	context.scene->addLightShader(new Light);
}

void mtco_SwatchRendererInterface::getImageData(MImage& imageRef)
{
	float *img = imageRef.floatPixels();
	memcpy(img, this->imageData, sizeof(float) * this->swatchWidth * this->swatchWidth * 4);
	return;
}

void mtco_SwatchRendererInterface::saveImageData()
{

	Corona::Pixel p = this->context.fb->getImageSize();

	int width = p.x;
	int height = p.y;
	size_t numPixels = width * height;

	uint numPixelsInRow = width;
	bool doToneMapping = true;
	Corona::Pixel firstPixelInRow(0, 0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	int index = 0;
	for (uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		try{
			this->context.fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, false, outColors, outAlpha);
		}
		catch (char *errorMsg){
			logger.error(errorMsg);
			break;
		}
		uint rowPos = rowId * numPixelsInRow;
		for (int x = 0; x < numPixelsInRow; x++)
		{
			index = (rowPos + x) * 4;
			imageData[index] = outColors[x].r();
			imageData[index + 1] = outColors[x].g();
			imageData[index + 2] = outColors[x].b();
			imageData[index + 3] = 1.0f;
		}
	}
	delete[] outColors;
	delete[] outAlpha;
}

void mtco_SwatchRendererInterface::renderSwatch()
{
	this->inProgress = true;
	context.core = Corona::ICore::createInstance();
	context.scene = context.core->createScene();
	context.logger = new mtco_Logger(context.core);
	context.settings = new Settings();

	Corona::ConfParser parser;
	Corona::String resPath = (getRendererHome() + "ressources/").asChar();
	Corona::String defPath = "Config-" + Corona::toStr(Corona::PropertyDescriptor::descriptorsHash()) + ".conf";
	logger.debug(MString("parser: ") + (resPath + defPath).cStr());
	parser.parseFile(resPath + defPath, context.settings, Corona::ConfParser::CREATE_IF_NONEXISTENT);

	defineSettings();
	context.fb = context.core->createFb();
	context.fb->initFb(context.settings, context.renderPasses);

	this->createCoronaScene();

	// test that the now ready scene and settings do not have any errors
	context.core->sanityCheck(context.scene);
	context.core->sanityCheck(context.settings);

	context.core->beginSession(context.scene, context.settings, context.fb, context.logger, Corona::ICore::AdditionalInfo());

	context.core->renderFrame();
	context.core->endSession();
	this->saveImageData();

	delete context.logger;
	delete context.settings;

	context.core->destroyScene(context.scene);
	context.core->destroyFb(context.fb);
	Corona::ICore::destroyInstance(context.core);
	this->imgDone = true;
	this->inProgress = false;
}

void mtco_SwatchRendererInterface::getImage(MImage& imageRef)
{
}

void mtco_SwatchRendererInterface::swatchRenderThread(mtco_SwatchRendererInterface* me)
{

}

void mtco_SwatchRendererInterface::initializeStaticData()
{
#ifdef _DEBUG
	logger.setLogLevel(Logging::Debug);
#endif
	logger.debug("------------initializeStaticData called.");
}

void mtco_SwatchRendererInterface::cleanUpStaticData()
{
	logger.debug("------------cleanUpStaticData called.");
}