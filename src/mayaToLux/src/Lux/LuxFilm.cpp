#include "Lux.h"

#include "mayaScene.h"
#include "mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "world.h"

static Logging logger;

void LuxRenderer::defineFilm()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode depFn(getRenderGlobalsNode());

	int width, height;
	renderGlobals->getWidthHeight(width, height);

	MString outputPath = renderGlobals->basePath + "/" + renderGlobals->imageName + "." + (int)renderGlobals->currentFrame + ".lxs";
	// file path without extension, will be added automatically by the renderer
	MString fileName = renderGlobals->imagePath + "/" + renderGlobals->imageName + "." + (int)renderGlobals->currentFrame;
	const char *filename = fileName.asChar();
	const int xres = width;
	const int yres = height;
	const bool write_png = true;
	const int halttime = getIntAttr("halttime", depFn, 10);
	const int haltspp = getIntAttr("haltspp", depFn, 10);
	int displayinterval = 3;

	ParamSet fp = CreateParamSet();
	fp->AddInt("xresolution",&xres);
	fp->AddInt("yresolution",&yres);
	fp->AddBool("write_png",&write_png);
	fp->AddString("filename",&filename);
	if( halttime > 0)
		fp->AddInt("halttime", &halttime);
	if( haltspp > 0)
		fp->AddInt("haltspp", &haltspp);
	if( displayinterval > 0)
		fp->AddInt("displayinterval", &displayinterval);
	 
	lux->film("fleximage", boost::get_pointer(fp));
}

