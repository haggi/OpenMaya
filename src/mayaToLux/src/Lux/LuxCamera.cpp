#include "Lux.h"
#include "LuxUtils.h"
#include <maya/MFnCamera.h>
#include "../mtlu_common/mtlu_renderGlobals.h"
#include "../mtlu_common/mtlu_mayaScene.h"
#include "utilities/tools.h"

#include "utilities/logging.h"

static Logging logger;

void LuxRenderer::defineCamera()
{

	mtlu_MayaObject *mo = (mtlu_MayaObject *)this->mtlu_scene->camList[0];		
	MMatrix cm = mo->dagPath.inclusiveMatrix();
	MFnCamera camFn(mo->mobject);
	
	MPoint camEye = camFn.eyePoint(MSpace::kWorld);
	MPoint camInterest = camFn.centerOfInterestPoint(MSpace::kObject);
	// kworld seems not to work correctly so I multiply it by cam matrix
	camInterest *= cm;
	camEye.x = cm[3][0];
	camEye.y = cm[3][1];
	camEye.z = cm[3][2];

	MVector camUp = camFn.upDirection(MSpace::kWorld);
	setZUp(camEye);
	setZUp(camInterest);
	setZUp(camUp);

	logger.debug(MString("cam eye ") + camEye.x + " " + camEye.y + " " + camEye.z);
	logger.debug(MString("cam Int ") + camInterest.x + " " + camInterest.y + " " + camInterest.z);
	logger.debug(MString("cam Up ") + camUp.x + " " + camUp.y + " " + camUp.z);

	luxFile << "lookat " << camEye.x << " " << camEye.y << " " << camEye.z << " " << camInterest.x << " " << camInterest.y << " " << camInterest.z << " " << camUp.x << " " << camUp.y << " " <<  camUp.z << "\n";
	lux->lookAt(camEye.x, camEye.y, camEye.z, camInterest.x, camInterest.y, camInterest.z, camUp.x, camUp.y, camUp.z);
	
	// lux uses the fov of the smallest image edge
	double hFov = RadToDeg(camFn.horizontalFieldOfView());
	double vFov = RadToDeg(camFn.verticalFieldOfView());
	float fov = hFov;
	if( this->mtlu_renderGlobals->imgHeight > this->mtlu_renderGlobals->imgWidth)
		fov = vFov;

	// focaldist
	float focusDist = (float)camFn.focusDistance();
	// hither, yon
	float hither = (float)camFn.nearClippingPlane();
	float yon = (float)camFn.farClippingPlane();
	// render region
	int ybot = (this->mtlu_renderGlobals->imgHeight - this->mtlu_renderGlobals->regionBottom);
	int ytop = (this->mtlu_renderGlobals->imgHeight - this->mtlu_renderGlobals->regionTop);
	int ymin = ybot <  ytop ? ybot :  ytop;
	int ymax = ybot >  ytop ? ybot :  ytop;


	ParamSet cp = CreateParamSet();
	cp->AddFloat("fov", &fov);
	cp->AddFloat("focaldistance", &focusDist);
	cp->AddFloat("hither", &hither);
	cp->AddFloat("yon", &yon);
	luxFile << "Camera \"perspective\" " << " \"float fov\" " << fov << " \"float focaldistance\" " << focusDist << " \"float hither\" " << hither << " \"float yon\" " << yon << "\n";
	lux->camera("perspective", boost::get_pointer(cp));

	//lux->pixelFilter("mitchell","xwidth", &size, "ywidth" , &size,LUX_NULL);

}