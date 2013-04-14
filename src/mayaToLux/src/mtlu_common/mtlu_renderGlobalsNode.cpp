#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtlu_renderGlobalsNode.h"

MTypeId	MayaToLuxGlobals::id(0x0011CF49);

//	------------- automatically created attributes start ----------- // 

MObject MayaToLuxGlobals::renderer;
MObject MayaToLuxGlobals::hsConfigFile;
MObject MayaToLuxGlobals::hsOclPlatformIndex;
MObject MayaToLuxGlobals::hsOclGpuUse;
MObject MayaToLuxGlobals::hsOclWGroupSize;
MObject MayaToLuxGlobals::sampler;
MObject MayaToLuxGlobals::pixelSampler;
MObject MayaToLuxGlobals::numSamples;
MObject MayaToLuxGlobals::maxconsecrejects;
MObject MayaToLuxGlobals::largemutationprob;
MObject MayaToLuxGlobals::mutationrange;
MObject MayaToLuxGlobals::usevariance;
MObject MayaToLuxGlobals::initSamples;
MObject MayaToLuxGlobals::chainLength;
MObject MayaToLuxGlobals::mutationRange;
MObject MayaToLuxGlobals::imageFormat;
MObject MayaToLuxGlobals::premultiplyAlpha;
MObject MayaToLuxGlobals::pixelfilter;
MObject MayaToLuxGlobals::filterWidth;
MObject MayaToLuxGlobals::filterHeight;
MObject MayaToLuxGlobals::filterAlpha;
MObject MayaToLuxGlobals::B;
MObject MayaToLuxGlobals::C;
MObject MayaToLuxGlobals::mSupersample;
MObject MayaToLuxGlobals::sincTau;
MObject MayaToLuxGlobals::accelerator;
MObject MayaToLuxGlobals::kdIntersectcost;
MObject MayaToLuxGlobals::kdTraversalcost;
MObject MayaToLuxGlobals::kdEmptybonus;
MObject MayaToLuxGlobals::kdMaxprims;
MObject MayaToLuxGlobals::kdMaxdepth;
MObject MayaToLuxGlobals::maxprimsperleaf;
MObject MayaToLuxGlobals::fullsweepthreshold;
MObject MayaToLuxGlobals::skipfactor;
MObject MayaToLuxGlobals::refineimmediately;
MObject MayaToLuxGlobals::treetype;
MObject MayaToLuxGlobals::costsamples;
MObject MayaToLuxGlobals::surfaceIntegrator;
MObject MayaToLuxGlobals::lightStrategy;
MObject MayaToLuxGlobals::shadowraycount;
MObject MayaToLuxGlobals::eyedepth;
MObject MayaToLuxGlobals::lightdepth;
MObject MayaToLuxGlobals::eyerrthreshold;
MObject MayaToLuxGlobals::lightrrthreshold;
MObject MayaToLuxGlobals::pathMaxdepth;
MObject MayaToLuxGlobals::rrstrategy;
MObject MayaToLuxGlobals::rrcontinueprob;
MObject MayaToLuxGlobals::dlightMaxdepth;
MObject MayaToLuxGlobals::phRenderingmode;
MObject MayaToLuxGlobals::phCausticphotons;
MObject MayaToLuxGlobals::phIndirectphotons;
MObject MayaToLuxGlobals::phDirectphotons;
MObject MayaToLuxGlobals::phRadiancephotons;
MObject MayaToLuxGlobals::phNphotonsused;
MObject MayaToLuxGlobals::phMaxphotondist;
MObject MayaToLuxGlobals::phMaxdepth;
MObject MayaToLuxGlobals::phMaxphotondepth;
MObject MayaToLuxGlobals::phFinalgather;
MObject MayaToLuxGlobals::phFinalgathersamples;
MObject MayaToLuxGlobals::phGatherangle;
MObject MayaToLuxGlobals::phRrstrategy;
MObject MayaToLuxGlobals::phRrcontinueprob;
MObject MayaToLuxGlobals::phDistancethreshold;
MObject MayaToLuxGlobals::phPhotonmapsfile;
//	------------- automatically created attributes end ----------- // 


MayaToLuxGlobals::MayaToLuxGlobals()
{}

MayaToLuxGlobals::~MayaToLuxGlobals()
{}


void *MayaToLuxGlobals::creator()
{
	return new MayaToLuxGlobals();
}

MStatus	MayaToLuxGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 

	renderer = eAttr.create("renderer", "renderer", 0, &stat);
	stat = eAttr.addField( "sampler", 0 );
	stat = eAttr.addField( "hybridsampler", 1 );
	stat = eAttr.addField( "hybridsppm", 2 );
	CHECK_MSTATUS(addAttribute( renderer ));

	hsConfigFile = tAttr.create("hsConfigFile", "hsConfigFile",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( hsConfigFile ));

	hsOclPlatformIndex = nAttr.create("hsOclPlatformIndex", "hsOclPlatformIndex",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( hsOclPlatformIndex ));

	hsOclGpuUse = nAttr.create("hsOclGpuUse", "hsOclGpuUse",  MFnNumericData::kBoolean, 1);
	CHECK_MSTATUS(addAttribute( hsOclGpuUse ));

	hsOclWGroupSize = nAttr.create("hsOclWGroupSize", "hsOclWGroupSize",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( hsOclWGroupSize ));

	sampler = eAttr.create("sampler", "sampler", 0, &stat);
	stat = eAttr.addField( "random", 0 );
	stat = eAttr.addField( "low discrepancy", 1 );
	stat = eAttr.addField( "metropolis", 2 );
	stat = eAttr.addField( "rrpt", 3 );
	CHECK_MSTATUS(addAttribute( sampler ));

	pixelSampler = eAttr.create("pixelSampler", "pixelSampler", 2, &stat);
	stat = eAttr.addField( "hilbert", 0 );
	stat = eAttr.addField( "linear", 1 );
	stat = eAttr.addField( "vegas", 2 );
	stat = eAttr.addField( "lowdiscrepancy", 3 );
	stat = eAttr.addField( "tile", 4 );
	stat = eAttr.addField( "random", 5 );
	CHECK_MSTATUS(addAttribute( pixelSampler ));

	numSamples = nAttr.create("numSamples", "numSamples",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( numSamples ));

	maxconsecrejects = nAttr.create("maxconsecrejects", "maxconsecrejects",  MFnNumericData::kInt, 512);
	CHECK_MSTATUS(addAttribute( maxconsecrejects ));

	largemutationprob = nAttr.create("largemutationprob", "largemutationprob",  MFnNumericData::kFloat, 0.4);
	CHECK_MSTATUS(addAttribute( largemutationprob ));

	mutationrange = nAttr.create("mutationrange", "mutationrange",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( mutationrange ));

	usevariance = nAttr.create("usevariance", "usevariance",  MFnNumericData::kBoolean, 0);
	CHECK_MSTATUS(addAttribute( usevariance ));

	initSamples = nAttr.create("initSamples", "initSamples",  MFnNumericData::kInt, 100000);
	CHECK_MSTATUS(addAttribute( initSamples ));

	chainLength = nAttr.create("chainLength", "chainLength",  MFnNumericData::kInt, 2000);
	CHECK_MSTATUS(addAttribute( chainLength ));

	mutationRange = nAttr.create("mutationRange", "mutationRange",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( mutationRange ));

	imageFormat = eAttr.create("imageFormat", "imageFormat", 1, &stat);
	stat = eAttr.addField( "Exr", 0 );
	stat = eAttr.addField( "Png", 1 );
	stat = eAttr.addField( "Tga", 2 );
	CHECK_MSTATUS(addAttribute( imageFormat ));

	premultiplyAlpha = nAttr.create("premultiplyAlpha", "premultiplyAlpha",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( premultiplyAlpha ));

	pixelfilter = eAttr.create("pixelfilter", "pixelfilter", 3, &stat);
	stat = eAttr.addField( "box", 0 );
	stat = eAttr.addField( "trianlge", 1 );
	stat = eAttr.addField( "gauss", 2 );
	stat = eAttr.addField( "mitchell", 3 );
	stat = eAttr.addField( "sinc", 4 );
	CHECK_MSTATUS(addAttribute( pixelfilter ));

	filterWidth = nAttr.create("filterWidth", "filterWidth",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( filterWidth ));

	filterHeight = nAttr.create("filterHeight", "filterHeight",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( filterHeight ));

	filterAlpha = nAttr.create("filterAlpha", "filterAlpha",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( filterAlpha ));

	B = nAttr.create("B", "B",  MFnNumericData::kFloat, .33);
	CHECK_MSTATUS(addAttribute( B ));

	C = nAttr.create("C", "C",  MFnNumericData::kFloat, .33);
	CHECK_MSTATUS(addAttribute( C ));

	mSupersample = nAttr.create("mSupersample", "mSupersample",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( mSupersample ));

	sincTau = nAttr.create("sincTau", "sincTau",  MFnNumericData::kFloat, 3.0);
	CHECK_MSTATUS(addAttribute( sincTau ));

	accelerator = eAttr.create("accelerator", "accelerator", 0, &stat);
	stat = eAttr.addField( "kdtree", 0 );
	stat = eAttr.addField( "qbvh", 1 );
	stat = eAttr.addField( "grid (not thread-safe)", 2 );
	stat = eAttr.addField( "unsafekdtree (not thread-safe)", 3 );
	stat = eAttr.addField( "bvh (not thread-safe)", 4 );
	stat = eAttr.addField( "none", 5 );
	CHECK_MSTATUS(addAttribute( accelerator ));

	kdIntersectcost = nAttr.create("kdIntersectcost", "kdIntersectcost",  MFnNumericData::kInt, 80);
	CHECK_MSTATUS(addAttribute( kdIntersectcost ));

	kdTraversalcost = nAttr.create("kdTraversalcost", "kdTraversalcost",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( kdTraversalcost ));

	kdEmptybonus = nAttr.create("kdEmptybonus", "kdEmptybonus",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( kdEmptybonus ));

	kdMaxprims = nAttr.create("kdMaxprims", "kdMaxprims",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( kdMaxprims ));

	kdMaxdepth = nAttr.create("kdMaxdepth", "kdMaxdepth",  MFnNumericData::kInt, -1);
	CHECK_MSTATUS(addAttribute( kdMaxdepth ));

	maxprimsperleaf = nAttr.create("maxprimsperleaf", "maxprimsperleaf",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( maxprimsperleaf ));

	fullsweepthreshold = nAttr.create("fullsweepthreshold", "fullsweepthreshold",  MFnNumericData::kInt, 4 * 4);
	CHECK_MSTATUS(addAttribute( fullsweepthreshold ));

	skipfactor = nAttr.create("skipfactor", "skipfactor",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( skipfactor ));

	refineimmediately = nAttr.create("refineimmediately", "refineimmediately",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( refineimmediately ));

	treetype = eAttr.create("treetype", "treetype", 0, &stat);
	stat = eAttr.addField( "binary", 0 );
	stat = eAttr.addField( "quadtree", 1 );
	stat = eAttr.addField( "octree", 2 );
	CHECK_MSTATUS(addAttribute( treetype ));

	costsamples = nAttr.create("costsamples", "costsamples",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( costsamples ));

	surfaceIntegrator = eAttr.create("surfaceIntegrator", "surfaceIntegrator", 0, &stat);
	stat = eAttr.addField( "bidirectional (default)", 0 );
	stat = eAttr.addField( "path", 1 );
	stat = eAttr.addField( "exphotonmap", 2 );
	stat = eAttr.addField( "directlighting", 3 );
	stat = eAttr.addField( "distributedpath", 4 );
	CHECK_MSTATUS(addAttribute( surfaceIntegrator ));

	lightStrategy = eAttr.create("lightStrategy", "lightStrategy", 2, &stat);
	stat = eAttr.addField( "one", 0 );
	stat = eAttr.addField( "all", 1 );
	stat = eAttr.addField( "auto", 2 );
	stat = eAttr.addField( "importance", 3 );
	stat = eAttr.addField( "powerimp", 4 );
	stat = eAttr.addField( "allpowerimp", 5 );
	stat = eAttr.addField( "logpowerimp", 6 );
	CHECK_MSTATUS(addAttribute( lightStrategy ));

	shadowraycount = nAttr.create("shadowraycount", "shadowraycount",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( shadowraycount ));

	eyedepth = nAttr.create("eyedepth", "eyedepth",  MFnNumericData::kInt, 8);
	CHECK_MSTATUS(addAttribute( eyedepth ));

	lightdepth = nAttr.create("lightdepth", "lightdepth",  MFnNumericData::kInt, 8);
	CHECK_MSTATUS(addAttribute( lightdepth ));

	eyerrthreshold = nAttr.create("eyerrthreshold", "eyerrthreshold",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( eyerrthreshold ));

	lightrrthreshold = nAttr.create("lightrrthreshold", "lightrrthreshold",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( lightrrthreshold ));

	pathMaxdepth = nAttr.create("pathMaxdepth", "pathMaxdepth",  MFnNumericData::kInt, 16);
	CHECK_MSTATUS(addAttribute( pathMaxdepth ));

	rrstrategy = eAttr.create("rrstrategy", "rrstrategy", 2, &stat);
	stat = eAttr.addField( "none", 0 );
	stat = eAttr.addField( "probability", 1 );
	stat = eAttr.addField( "efficiency", 2 );
	CHECK_MSTATUS(addAttribute( rrstrategy ));

	rrcontinueprob = nAttr.create("rrcontinueprob", "rrcontinueprob",  MFnNumericData::kFloat, .65);
	CHECK_MSTATUS(addAttribute( rrcontinueprob ));

	dlightMaxdepth = nAttr.create("dlightMaxdepth", "dlightMaxdepth",  MFnNumericData::kInt, 5);
	CHECK_MSTATUS(addAttribute( dlightMaxdepth ));

	phRenderingmode = eAttr.create("phRenderingmode", "phRenderingmode", 0, &stat);
	stat = eAttr.addField( "directlighting", 0 );
	stat = eAttr.addField( "path", 1 );
	CHECK_MSTATUS(addAttribute( phRenderingmode ));

	phCausticphotons = nAttr.create("phCausticphotons", "phCausticphotons",  MFnNumericData::kInt, 20000);
	CHECK_MSTATUS(addAttribute( phCausticphotons ));

	phIndirectphotons = nAttr.create("phIndirectphotons", "phIndirectphotons",  MFnNumericData::kInt, 200000);
	CHECK_MSTATUS(addAttribute( phIndirectphotons ));

	phDirectphotons = nAttr.create("phDirectphotons", "phDirectphotons",  MFnNumericData::kInt, 200000);
	CHECK_MSTATUS(addAttribute( phDirectphotons ));

	phRadiancephotons = nAttr.create("phRadiancephotons", "phRadiancephotons",  MFnNumericData::kInt, 200000);
	CHECK_MSTATUS(addAttribute( phRadiancephotons ));

	phNphotonsused = nAttr.create("phNphotonsused", "phNphotonsused",  MFnNumericData::kInt, 50);
	CHECK_MSTATUS(addAttribute( phNphotonsused ));

	phMaxphotondist = nAttr.create("phMaxphotondist", "phMaxphotondist",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( phMaxphotondist ));

	phMaxdepth = nAttr.create("phMaxdepth", "phMaxdepth",  MFnNumericData::kInt, 5);
	CHECK_MSTATUS(addAttribute( phMaxdepth ));

	phMaxphotondepth = nAttr.create("phMaxphotondepth", "phMaxphotondepth",  MFnNumericData::kInt, 10);
	CHECK_MSTATUS(addAttribute( phMaxphotondepth ));

	phFinalgather = nAttr.create("phFinalgather", "phFinalgather",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( phFinalgather ));

	phFinalgathersamples = nAttr.create("phFinalgathersamples", "phFinalgathersamples",  MFnNumericData::kInt, 32);
	CHECK_MSTATUS(addAttribute( phFinalgathersamples ));

	phGatherangle = nAttr.create("phGatherangle", "phGatherangle",  MFnNumericData::kFloat, 10.0);
	CHECK_MSTATUS(addAttribute( phGatherangle ));

	phRrstrategy = eAttr.create("phRrstrategy", "phRrstrategy", 2, &stat);
	stat = eAttr.addField( "none", 0 );
	stat = eAttr.addField( "probability", 1 );
	stat = eAttr.addField( "efficiency", 2 );
	CHECK_MSTATUS(addAttribute( phRrstrategy ));

	phRrcontinueprob = nAttr.create("phRrcontinueprob", "phRrcontinueprob",  MFnNumericData::kFloat, 0.65);
	CHECK_MSTATUS(addAttribute( phRrcontinueprob ));

	phDistancethreshold = nAttr.create("phDistancethreshold", "phDistancethreshold",  MFnNumericData::kFloat, 1.25);
	CHECK_MSTATUS(addAttribute( phDistancethreshold ));

	phPhotonmapsfile = tAttr.create("phPhotonmapsfile", "phPhotonmapsfile",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( phPhotonmapsfile ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

