#include "Lux.h"
#include <maya/MFnLight.h>

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "utilities/logging.h"
static Logging logger;

void LuxRenderer::defineDirectionalLight(mtlu_MayaObject *obj)
{
	MFnLight lightFn(obj->mobject);
	float from[3]= {0,20,50};
	float to[3]= {0,0,0};
	MVector lightDir(0,0,1);
	lightDir *= obj->dagPath.inclusiveMatrix();
	logger.debug(MString("Translating directional light ") + obj->shortName);
	logger.debug(MString("from orig ") + lightDir.x + " " + lightDir.y + " " + lightDir.z);
	setZUp(lightDir);
	from[0] = lightDir.x;
	from[1] = lightDir.y;
	from[2] = lightDir.z;
	logger.debug(MString("from up ") + lightDir.x + " " + lightDir.y + " " + lightDir.z);
	
	MColor lightColor(1,0,0);
	getColor(MString("color"), lightFn, lightColor);
	float L[3];
	L[0] = lightColor.r;
	L[1] = lightColor.g;
	L[2] = lightColor.b;

	float theta = 0.0f;
	getFloat(MString("mtlu_dirLight_theta"), lightFn, theta);

	ParamSet lp = CreateParamSet();
	lp->AddPoint("from", from, 3);
	lp->AddPoint("to", to, 3);
	lp->AddRGBColor("L", L, 3);
	lp->AddFloat("theta", &theta, 1);

	lux->lightSource("distant", boost::get_pointer(lp));

	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "LightSource \"distant\" \"color L\" [" << "\n"; 
}

void LuxRenderer::defineSpotLight(mtlu_MayaObject *obj)
{}
void LuxRenderer::defineSunLight(mtlu_MayaObject *obj)
{}
void LuxRenderer::defineOmniLight(mtlu_MayaObject *obj)
{}

void LuxRenderer::defineLights()
{
	for( size_t lightId = 0; lightId < this->mtlu_scene->lightList.size(); lightId++)
	{
		mtlu_MayaObject *obj = (mtlu_MayaObject *)this->mtlu_scene->lightList[lightId];

		if( obj->mobject.hasFn(MFn::kDirectionalLight))
		{
			this->defineDirectionalLight(obj);
		}
	}
}
