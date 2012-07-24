#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MGlobal.h>
#include <maya/MFnPluginData.h>
#include "mtm_globalsNode.h"

MTypeId	mayaToMantraGlobals::id(0x00106EF0);

MObject mayaToMantraGlobals::detectShapeDeform;
MObject mayaToMantraGlobals::scaleFactor;
MObject mayaToMantraGlobals::output;
MObject mayaToMantraGlobals::singleFileOutput;
MObject mayaToMantraGlobals::motionblur;
MObject mayaToMantraGlobals::imagemotionblur;
MObject mayaToMantraGlobals::xftimesamples;
MObject mayaToMantraGlobals::geotimesamples;
MObject mayaToMantraGlobals::motionfactor;
MObject mayaToMantraGlobals::mbtype;
MObject mayaToMantraGlobals::dof;
MObject mayaToMantraGlobals::samples;
MObject mayaToMantraGlobals::samplelock;
MObject mayaToMantraGlobals::jitter;
MObject mayaToMantraGlobals::noiselevel;
MObject mayaToMantraGlobals::randomseed;
MObject mayaToMantraGlobals::rayvariance;
MObject mayaToMantraGlobals::minraysamples;
MObject mayaToMantraGlobals::maxraysamples;
MObject mayaToMantraGlobals::volumestepsize;
MObject mayaToMantraGlobals::decoupleshadowstep;
MObject mayaToMantraGlobals::shadowstepsize;
MObject mayaToMantraGlobals::filtertype;
MObject mayaToMantraGlobals::filtersize;
MObject mayaToMantraGlobals::bitdepth;
MObject mayaToMantraGlobals::renderengine;
MObject mayaToMantraGlobals::tilesize;
MObject mayaToMantraGlobals::opacitylimit;
MObject mayaToMantraGlobals::verbosity;
MObject mayaToMantraGlobals::translatorVerbosity;
MObject mayaToMantraGlobals::usemaxproc;
MObject mayaToMantraGlobals::threadcount;
MObject mayaToMantraGlobals::accerationtype;
MObject mayaToMantraGlobals::kdmemfac;
MObject mayaToMantraGlobals::geocachesize;
MObject mayaToMantraGlobals::texcachesize;
MObject mayaToMantraGlobals::renderviewcam;
MObject mayaToMantraGlobals::autogenenvmap;
MObject mayaToMantraGlobals::autogensmap;
MObject mayaToMantraGlobals::reflectlimit;
MObject mayaToMantraGlobals::refractlimit;
MObject mayaToMantraGlobals::glossylimit;
MObject mayaToMantraGlobals::diffuselimit;
MObject mayaToMantraGlobals::volumelimit;
MObject mayaToMantraGlobals::raytracebias;
MObject mayaToMantraGlobals::biasalongnormal;
MObject mayaToMantraGlobals::colorspace;
MObject mayaToMantraGlobals::raylimit;
MObject mayaToMantraGlobals::smoothgrid;
MObject mayaToMantraGlobals::colorlimit;
MObject mayaToMantraGlobals::minreflectratio;

MObject mayaToMantraGlobals::basePath;
MObject mayaToMantraGlobals::imagePath;
MObject mayaToMantraGlobals::imageName;

MObject mayaToMantraGlobals::geoFileType;


mayaToMantraGlobals::mayaToMantraGlobals()
{
}

mayaToMantraGlobals::~mayaToMantraGlobals()
{}

void *mayaToMantraGlobals::creator()
{
	return new mayaToMantraGlobals();
}

MStatus	mayaToMantraGlobals::initialize()
{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;

	MStatus stat = MStatus::kSuccess;

	output = eAttr.create( "output", "output", 0, &stat);
	stat = eAttr.addField( "Render To MPlay", 0 );
	stat = eAttr.addField( "Render To File", 1 );

	detectShapeDeform = nAttr.create("detectShapeDeform", "detectShapeDeform", MFnNumericData::kBoolean, true);
	scaleFactor = nAttr.create("scaleFactor", "scaleFactor", MFnNumericData::kFloat, 0.1);
	singleFileOutput = nAttr.create("singleFileOutput", "singleFileOutput", MFnNumericData::kBoolean, false);
	motionblur = nAttr.create("motionblur", "motionblur", MFnNumericData::kBoolean, false);
	imagemotionblur = nAttr.create("imagemotionblur", "imagemotionblur", MFnNumericData::kBoolean, true);
	xftimesamples = nAttr.create("xftimesamples", "xftimesamples", MFnNumericData::kInt, 2);
	geotimesamples = nAttr.create("geotimesamples", "geotimesamples", MFnNumericData::kInt, 1);
	motionfactor = nAttr.create("motionfactor", "motionfactor", MFnNumericData::kFloat, 0);
	mbtype = eAttr.create( "mbtype", "mbtype", 0, &stat);
	stat = eAttr.addField( "Leading Blur", 0 );
	stat = eAttr.addField( "Center Blur", 1 );
	stat = eAttr.addField( "Trailing Blur", 2 );
	eAttr.setDefault(1);
	dof = nAttr.create("dof", "dof", MFnNumericData::kBoolean, false);
	samples = nAttr.create("samples", "samples", MFnNumericData::k2Int);
	nAttr.setDefault(3,3);

	samplelock = nAttr.create("samplelock", "samplelock", MFnNumericData::kBoolean, true);
	jitter = nAttr.create("jitter", "jitter", MFnNumericData::kFloat, 1.0);
	noiselevel = nAttr.create("noiselevel", "noiselevel", MFnNumericData::kFloat, 0.05);
	randomseed = nAttr.create("randomseed", "randomseed", MFnNumericData::kInt, 0);
	rayvariance = nAttr.create("rayvariance", "rayvariance", MFnNumericData::kBoolean, false);
	minraysamples = nAttr.create("minraysamples", "minraysamples", MFnNumericData::kInt, 1);
	maxraysamples = nAttr.create("maxraysamples", "maxraysamples", MFnNumericData::kInt, 1);
	volumestepsize = nAttr.create("volumestepsize", "volumestepsize", MFnNumericData::kFloat, 0.1);
	decoupleshadowstep = nAttr.create("decoupleshadowstep", "decoupleshadowstep", MFnNumericData::kBoolean, false);
	shadowstepsize = nAttr.create("shadowstepsize", "shadowstepsize", MFnNumericData::kFloat, 1.0);
	filtertype = eAttr.create( "filtertype", "filtertype", 0, &stat);
	stat = eAttr.addField( "Unit Box Filter", 0 );
	stat = eAttr.addField( "Gauss", 1 );
	stat = eAttr.addField( "Bartlett", 2 );
	stat = eAttr.addField( "Catmull Rom", 3 );
	stat = eAttr.addField( "Hanning", 4 );
	stat = eAttr.addField( "Blackman", 5 );
	stat = eAttr.addField( "Sinc (sharpening)", 6 );
	stat = eAttr.addField( "Closest Sample Filtering", 7 );
	stat = eAttr.addField( "Farthest Samples Filtering", 8 );
	stat = eAttr.addField( "Disable edge Antialiasing", 9 );
	stat = eAttr.addField( "Object with most coverage", 10 );
	stat = eAttr.addField( "Object with most-nofilter", 11 );
	filtersize = nAttr.create("filtersize", "filtersize", MFnNumericData::k2Int);
	nAttr.setDefault(3,3);
	bitdepth = eAttr.create( "bitdepth", "bitdepth", 0, &stat);
	stat = eAttr.addField( "8 bit", 0 );
	stat = eAttr.addField( "16 bit", 1 );
	stat = eAttr.addField( "16 bit float", 2 );
	stat = eAttr.addField( "32 bit float", 3 );
	renderengine = eAttr.create( "renderengine", "renderengine", 0, &stat);
	stat = eAttr.addField( "Micropolygon Renderer", 0 );
	stat = eAttr.addField( "Raytracing", 1 );
	stat = eAttr.addField( "Micropolygon PBR", 2 );
	stat = eAttr.addField( "PBR", 3 );
	stat = eAttr.addField( "Photon Map Generation", 4 );
	tilesize = nAttr.create("tilesize", "tilesize", MFnNumericData::kInt, 16);
	opacitylimit = nAttr.create("opacitylimit", "opacitylimit", MFnNumericData::kFloat, 0.98);
	verbosity = tAttr.create("verbosity", "verbosity", MFnNumericData::kString);
	
	translatorVerbosity = eAttr.create( "translatorVerbosity", "translatorVerbosity", 0, &stat);
	stat = eAttr.addField( "None", 0 );
	stat = eAttr.addField( "Error", 1 );
	stat = eAttr.addField( "Warning", 2 );
	stat = eAttr.addField( "Info", 3 );
	stat = eAttr.addField( "Progress", 4 );
	stat = eAttr.addField( "Debug", 5 );
	eAttr.setDefault(3);		

	usemaxproc = nAttr.create("usemaxproc", "usemaxproc", MFnNumericData::kBoolean, true);
	threadcount = nAttr.create("threadcount", "threadcount", MFnNumericData::kInt, 1);
	accerationtype = eAttr.create( "accerationtype", "accerationtype", 0, &stat);
	stat = eAttr.addField( "KT Tree", 0 );
	stat = eAttr.addField( "BVH", 1 );
	kdmemfac = nAttr.create("kdmemfac", "kdmemfac", MFnNumericData::kFloat, 1.0);
	geocachesize = nAttr.create("geocachesize", "geocachesize", MFnNumericData::kInt, 256);
	texcachesize = nAttr.create("texcachesize", "texcachesize", MFnNumericData::kInt, 256);
	renderviewcam = nAttr.create("renderviewcam", "renderviewcam", MFnNumericData::kBoolean, true);
	autogenenvmap = nAttr.create("autogenenvmap", "autogenenvmap", MFnNumericData::kBoolean, true);
	autogensmap = nAttr.create("autogensmap", "autogensmap", MFnNumericData::kBoolean, true);
	reflectlimit = nAttr.create("reflectlimit", "reflectlimit", MFnNumericData::kInt, 10);
	refractlimit = nAttr.create("refractlimit", "refractlimit", MFnNumericData::kInt, 10);
	glossylimit = nAttr.create("glossylimit", "glossylimit", MFnNumericData::kInt, 1);
	diffuselimit = nAttr.create("diffuselimit", "diffuselimit", MFnNumericData::kInt, 0);
	volumelimit = nAttr.create("volumelimit", "volumelimit", MFnNumericData::kInt, 0);
	raytracebias = nAttr.create("raytracebias", "raytracebias", MFnNumericData::kFloat, 0.01);
	biasalongnormal = nAttr.create("biasalongnormal", "biasalongnormal", MFnNumericData::kBoolean, false);
	colorspace = eAttr.create( "colorspace", "colorspace", 0, &stat);
	stat = eAttr.addField( "Linear", 0 );
	stat = eAttr.addField( "Gamma 2.2", 1 );
	raylimit = eAttr.create( "raylimit", "raylimit", 0, &stat);
	stat = eAttr.addField( "Black Background", 0 );
	stat = eAttr.addField( "Direct Lighting BG", 1 );
	smoothgrid = nAttr.create("smoothgrid", "smoothgrid", MFnNumericData::kBoolean, true);
	colorlimit = nAttr.create("colorlimit", "colorlimit", MFnNumericData::kFloat, 10.0);
	minreflectratio = nAttr.create("minreflectratio", "minreflectratio", MFnNumericData::kFloat, 10.0);

	basePath = tAttr.create("basePath", "basePath", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	imagePath = tAttr.create("imagePath", "imagePath", MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	imageName = tAttr.create("imageName", "imageName", MFnNumericData::kString);

	geoFileType = eAttr.create( "geoFileType", "geoFileType", 0, &stat);
	stat = eAttr.addField( "Binary", 0 );
	stat = eAttr.addField( "Ascii", 1 );

	stat = addAttribute( geoFileType );

	stat = addAttribute( detectShapeDeform );
	stat = addAttribute( basePath );
	stat = addAttribute( imagePath );
	stat = addAttribute( imageName );

	stat = addAttribute( scaleFactor );
	stat = addAttribute( output );
	stat = addAttribute( singleFileOutput );
	stat = addAttribute( motionblur );
	stat = addAttribute( imagemotionblur );
	stat = addAttribute( xftimesamples );
	stat = addAttribute( geotimesamples );
	stat = addAttribute( motionfactor );
	stat = addAttribute( mbtype );
	stat = addAttribute( dof );
	stat = addAttribute( samples );
	stat = addAttribute( samplelock );
	stat = addAttribute( jitter );
	stat = addAttribute( noiselevel );
	stat = addAttribute( randomseed );
	stat = addAttribute( rayvariance );
	stat = addAttribute( minraysamples );
	stat = addAttribute( maxraysamples );
	stat = addAttribute( volumestepsize );
	stat = addAttribute( decoupleshadowstep );
	stat = addAttribute( shadowstepsize );
	stat = addAttribute( filtertype );
	stat = addAttribute( filtersize );
	stat = addAttribute( bitdepth );
	stat = addAttribute( renderengine );
	stat = addAttribute( tilesize );
	stat = addAttribute( opacitylimit );
	stat = addAttribute( verbosity );
	stat = addAttribute( translatorVerbosity );
	stat = addAttribute( usemaxproc );
	stat = addAttribute( threadcount );
	stat = addAttribute( accerationtype );
	stat = addAttribute( kdmemfac );
	stat = addAttribute( geocachesize );
	stat = addAttribute( texcachesize );
	stat = addAttribute( renderviewcam );
	stat = addAttribute( autogenenvmap );
	stat = addAttribute( autogensmap );
	stat = addAttribute( reflectlimit );
	stat = addAttribute( refractlimit );
	stat = addAttribute( glossylimit );
	stat = addAttribute( diffuselimit );
	stat = addAttribute( volumelimit );
	stat = addAttribute( raytracebias );
	stat = addAttribute( biasalongnormal );
	stat = addAttribute( colorspace );
	stat = addAttribute( raylimit );
	stat = addAttribute( smoothgrid );
	stat = addAttribute( colorlimit );
	stat = addAttribute( minreflectratio );

	return MStatus::kSuccess;
}
