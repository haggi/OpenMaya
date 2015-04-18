#include "Lux.h"
#include "LuxUtils.h"
#include <maya/MFnCamera.h>
#include "renderGlobals.h"
#include "mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "world.h"

static Logging logger;

const char *LensDistributions[] = {"uniform", "exponential", "inverse exponential", "gaussian", "inverse gaussian"};

void LuxRenderer::defineCamera()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<MayaObject> mo = mayaScene->camList[0];

	MMatrix cm = mo->dagPath.inclusiveMatrix();
	MFnCamera camFn(mo->mobject);
	this->transformCamera(mo.get(), renderGlobals->doMb && (mo->transformMatrices.size() > 1));

	// lux uses the fov of the smallest image edge
	double hFov = RadToDeg(camFn.horizontalFieldOfView());
	double vFov = RadToDeg(camFn.verticalFieldOfView());
	float fov = hFov;
	int width, height;
	renderGlobals->getWidthHeight(width, height);
	if( height < width)
		fov = vFov;

	// focaldist
	float focusDist = (float)camFn.focusDistance() * renderGlobals->sceneScale;
	float focalLen = (float)camFn.focalLength();
	float fStop = (float)camFn.fStop();

	bool useDOF = false;
	getBool(MString("depthOfField"), camFn, useDOF);
	useDOF = useDOF && renderGlobals->doDof;

	// hither, yon
	float hither = (float)camFn.nearClippingPlane();
	float yon = (float)camFn.farClippingPlane();
	
	// render region
	int left, bottom, right, top;
	renderGlobals->getRenderRegion(left, bottom, right, top);
	int ybot = (height - bottom);
	int ytop = (height - top);
	int ymin = ybot <  ytop ? ybot :  ytop;
	int ymax = ybot >  ytop ? ybot :  ytop;
	float lensradius = (focalLen / 1000.0) / ( 2.0 * fStop );

	int blades = 0;
	getInt(MString("mtlu_diaphragm_blades"), camFn, blades);
	bool autofocus = false;
	getBool(MString("mtlu_autofocus"), camFn, autofocus);
	int dist = 0;
	getInt(MString("mtlu_distribution"), camFn, dist);
	logger.debug(MString("Lens distribution: ") + dist + " " + LensDistributions[dist]);
	float power = 1.0f;
	getFloat(MString("mtlu_power"), camFn, power);
	const char *lensdistribution =  LensDistributions[dist];

	float shutterOpen = 0.0f;
	float shutterClose = renderGlobals->mbLength;

	ParamSet cp = CreateParamSet();
	cp->AddFloat("fov", &fov);
	cp->AddFloat("focaldistance", &focusDist);
	cp->AddFloat("hither", &hither);
	cp->AddFloat("yon", &yon);
	cp->AddFloat("shutteropen", &shutterOpen);
	cp->AddFloat("shutterclose", &shutterClose);
	if( blades > 0)
		cp->AddInt("blades", &blades);
	if( useDOF )
	{
		cp->AddFloat("lensradius", &lensradius);
		cp->AddBool("autofocus", &autofocus);
		cp->AddString("distribution", &lensdistribution);
		cp->AddFloat("power", &power);
	}
	lux->camera("perspective", boost::get_pointer(cp));
	
	if( renderGlobals->exportSceneFile)
		this->luxFile << "Camera \"perspective\" "<< "\"float fov\" [" << fov << "]"  <<"\n";

}