#include "Lux.h"

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "utilities/logging.h"
static Logging logger;

void LuxRenderer::defineFilm()
{
	int width = this->mtlu_renderGlobals->imgWidth;
	int height = this->mtlu_renderGlobals->imgHeight;
	MString outputPath = this->mtlu_renderGlobals->basePath + "/" + this->mtlu_renderGlobals->imageName + "." + (int)this->mtlu_renderGlobals->currentFrame + ".lxs";
	MString fileName = this->mtlu_renderGlobals->basePath + "/" + this->mtlu_renderGlobals->imageName + "." + (int)this->mtlu_renderGlobals->currentFrame + ".png";
	const char *filename = fileName.asChar();
	const int xres = width;
	const int yres = height;
	const bool write_png = true;
	const int halttime = this->mtlu_renderGlobals->halttime;
	const int haltspp = this->mtlu_renderGlobals->haltspp;
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

