#ifndef mtco_SWATCH_RENDERER
#define mtco_SWATCH_RENDERER

#include <maya/MMatrix.h>
#include "swatchesRenderer/SwatchRendererInterface.h"
#include <map>
#include "../coronaOSL/oslRenderer.h"
#include "CoronaCore/api/Api.h"


void coronaMatrix(Corona::AffineTm& atm, MMatrix& origMatrix, MMatrix& conversionMatrix);


class Light : public Corona::Abstract::LightShader, public Corona::Noncopyable {
public:

	const Corona::Pos LIGHT_POS;

	Light() : LIGHT_POS(Corona::Pos(-4.4 * 15.0, -6.4 * 15.0, 4.4 * 15.0)) { }

	// simulate spot light with colorful directional falloff
	virtual Corona::BsdfComponents getIllumination(Corona::IShadeContext& context, Corona::Spectrum& transport) const {

		float distance;
		Corona::BsdfComponents brdf;
		const Corona::Spectrum diffuse(Corona::rgb2Radiance(Corona::Rgb(0.8f, 0.8f, 0.8f)));		
		const Corona::Pos P = context.getPosition();
		const Corona::Dir toLight = (LIGHT_POS - P).getNormalized(distance);
		transport = Corona::Spectrum::WHITE;
		float dummy;
		context.forwardBsdfCos(toLight, brdf, dummy);
		return brdf * diffuse * Corona::PI * 2.0;
	}
};

class CheckerMap : public Corona::Abstract::Map {
public:
	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
		const Corona::Pos pos = context.getPosition() * 1.0f;
		const int tmp = int(floor(pos.x())) + int(floor(pos.y())) + int(floor(pos.z()));
		outAlpha = 1.f;
		if (tmp % 2) {
			return Corona::Rgb::BLACK;
		}
		else {
			return Corona::Rgb::WHITE * .8;
		}
	}

	virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
		return evalColor(context, cache, outAlpha).grayValue();
	}

	virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*) {
		STOP; //currently not supported
	}

	virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output) {
		STOP; //currently not supported
	}
	virtual void getChildren(Corona::Stack<Resource*>& output) { }
};

class mtco_Logger : public Corona::Abstract::Logger
{
public:

	mtco_Logger(Corona::ICore* core) : Corona::Abstract::Logger(&core->getStats()) { };

	virtual void logMsg(const Corona::LogMessage& message)
	{
#ifdef _DEBUG
		std::cout << message.text.cStr() << std::endl;
#endif
	}
	virtual void setProgress(const float progress)
	{
		std::cout << "Progress: " << progress << std::endl;
	}
};

class Settings : public Corona::Abstract::Settings {
protected:
	std::map<int, Corona::Abstract::Settings::Property> values;
public:
	virtual Corona::Abstract::Settings::Property get(const int id) const {
		const std::map<int, Corona::Abstract::Settings::Property>::const_iterator result = values.find(id);
		if (result != values.end()) {
			return result->second;
		}
		else {
			throw Corona::Exception::propertyNotFound(Corona::PropertyDescriptor::get(id)->name);
		}
	}
	virtual void set(const int id, const Corona::Abstract::Settings::Property& property) {
		values[id] = property;
	}
};

struct Context {
	Context()
	{
		isCancelled = false;
		isRendering = false;
		core = nullptr;
		fb = nullptr;
		scene = nullptr;
		settings = nullptr;
	}
	Corona::ICore* core;
	Corona::IFrameBuffer* fb;
	Corona::IScene* scene;
	mtco_Logger* logger;
	Corona::Abstract::Settings* settings;
	Corona::Stack<Corona::SharedPtr<Corona::IRenderPass>> renderPasses;
	bool isCancelled;
	bool isRendering;
};

static Context swatchRendererContext;

class mtco_SwatchRendererInterface : SwatchRendererInterface
{
public:
	mtco_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution);
	virtual ~mtco_SwatchRendererInterface();
	MObject renderNode;
	MObject dependNode;

	virtual void init();
	virtual void loadGeometry();
	virtual void renderSwatch();
	virtual void getImage(MImage& imageRef);
	virtual void getImageData(MImage& imageRef);

	virtual void fillDummySwatch(MImage& image);

	// corona specific
	Corona::ICore::MtlSceneConfig config;
	//Context context;
	void defineSettings();
	void createCoronaScene();
	void saveImageData(Context *c);
	static void initializeStaticData();
	static void cleanUpStaticData();
	static void swatchRenderThread(mtco_SwatchRendererInterface* me);
};
#endif