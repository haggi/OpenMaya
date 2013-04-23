#include "Lux.h"
#include "LuxUtils.h"
#include <maya/MFnCamera.h>
#include "../mtlu_common/mtlu_renderGlobals.h"
#include "../mtlu_common/mtlu_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "utilities/logging.h"

static Logging logger;

const char *LensDistributions[] = {"uniform", "exponential", "inverse exponential", "gaussian", "inverse gaussian"};

void LuxRenderer::defineCamera()
{

	mtlu_MayaObject *mo = (mtlu_MayaObject *)this->mtlu_scene->camList[0];		
	MMatrix cm = mo->dagPath.inclusiveMatrix();
	MFnCamera camFn(mo->mobject);
	this->transformCamera(mo, this->mtlu_renderGlobals->doMb && (mo->transformMatrices.size() > 1));

	// lux uses the fov of the smallest image edge
	double hFov = RadToDeg(camFn.horizontalFieldOfView());
	double vFov = RadToDeg(camFn.verticalFieldOfView());
	float fov = hFov;
	if( this->mtlu_renderGlobals->imgHeight < this->mtlu_renderGlobals->imgWidth)
		fov = vFov;

	// focaldist
	float focusDist = (float)camFn.focusDistance() * this->mtlu_renderGlobals->sceneScale;
	float focalLen = (float)camFn.focalLength();
	float fStop = (float)camFn.fStop();

	bool useDOF = false;
	getBool(MString("depthOfField"), camFn, useDOF);
	useDOF = useDOF && this->mtlu_renderGlobals->doDof;

	// hither, yon
	float hither = (float)camFn.nearClippingPlane();
	float yon = (float)camFn.farClippingPlane();
	
	// render region
	int ybot = (this->mtlu_renderGlobals->imgHeight - this->mtlu_renderGlobals->regionBottom);
	int ytop = (this->mtlu_renderGlobals->imgHeight - this->mtlu_renderGlobals->regionTop);
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
	float shutterClose = this->mtlu_renderGlobals->mbLength;

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
	luxFile << "Camera \"perspective\" " << " \"float fov\" " << fov << " \"float focaldistance\" " << focusDist << " \"float hither\" " << hither << " \"float yon\" " << yon << "\n";
	lux->camera("perspective", boost::get_pointer(cp));

	//lux->pixelFilter("mitchell","xwidth", &size, "ywidth" , &size,LUX_NULL);

}