#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnFluid.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MBoundingBox.h>
#include <maya/MDoubleArray.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "exporter/hsff.h"

static Logging logger;

#include "mtm_fluidExporter.h"

FluidExporter::FluidExporter(MObject& fluidObject, MString& fileName)
{
	this->fluidObject = fluidObject;
	this->fileName = fileName;
}

FluidExporter::~FluidExporter()
{}

MBoundingBox FluidExporter::exportIt()
{
	MBoundingBox bbox;
	MFnFluid fluidFn(this->fluidObject);
	std::string fname(this->fileName.asChar());
	fname = pystring::replace(fname, ".geo", ".hsff"); // hsff == haggis super file format
	fname = pystring::replace(fname, ".bgeo", ".hsff"); // hsff == haggis super file format
	
	logger.debug(MString("Exporting particle system to.") + fname.c_str());
	
	Hsff outFile(fname);
	
	if( !outFile.good() )
	{
		MGlobal::displayError(MString("Unable to open ") + MString(fname.c_str()) + " for export.");
		return bbox;
	}
	outFile.write(HSFF); // file identifier
	outFile.write(FLUID); // particle identifier
	
	MFnFluid::FluidMethod densityMethod, fuelMethod, velocityMethod, temperatureMethod;
	MFnFluid::FluidGradient densityGradient, fuelGradient, velocityGradient, temperatureGradient;

	fluidFn.getDensityMode( densityMethod, densityGradient);
	fluidFn.getVelocityMode( velocityMethod, velocityGradient);
	fluidFn.getFuelMode( fuelMethod, fuelGradient);
	fluidFn.getTemperatureMode(temperatureMethod, temperatureGradient);

	uint gridSize = fluidFn.gridSize();
	uint xres = 0, yres = 0, zres = 0;
	fluidFn.getResolution(xres, yres, zres);
	double xdim, ydim, zdim;
	fluidFn.getDimensions(xdim, ydim, zdim);
	bbox.expand(MPoint(-xdim/2.0, -ydim/2.0, -zdim/2.0));
	bbox.expand(MPoint( xdim/2.0,  ydim/2.0,  zdim/2.0));
	int numAttr = 0;
	MStatus stat;
	float *density = fluidFn.density(&stat);
	if( stat == MStatus::kSuccess) numAttr++;
	float *temperature = fluidFn.temperature(&stat);
	if( stat == MStatus::kSuccess) numAttr++;
	float *fuel = fluidFn.fuel(&stat);
	if( stat == MStatus::kSuccess) numAttr++;

	//fluidFn.getVelocity
	//MFn::Type pType = particleFn.type();
	//uint numParticles = particleFn.count();
	outFile.write(numAttr); // uint
	outFile.write(gridSize); // uint
	outFile.write(xres); // uint
	outFile.write(yres); // uint
	outFile.write(zres); // uint
	outFile.write(xdim); // double
	outFile.write(ydim); // double
	outFile.write(zdim); // double
	if( densityMethod != MFnFluid::kZero )
	{
		float densityScale = 1.0;
		getFloat(MString("densityScale"), fluidFn, densityScale);
		outFile.write(MString("DENSITY"));
		outFile.write(density, gridSize, densityScale);
	}
	if( fuelMethod != MFnFluid::kZero )
	{
		outFile.write(MString("FUEL"));
		outFile.write(fuel, gridSize);
	}
	if( temperatureMethod != MFnFluid::kZero )
	{
		outFile.write(MString("TEMPERATURE"));
		outFile.write(temperature, gridSize);
	}
	outFile.close();
	return bbox;
}
