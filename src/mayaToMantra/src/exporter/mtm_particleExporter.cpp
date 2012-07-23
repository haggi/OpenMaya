#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnParticleSystem.h>
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

#include "mtm_particleExporter.h"

ParticleExporter::ParticleExporter(MObject& particleObject, MString& fileName)
{
	this->particleObject = particleObject;
	this->fileName = fileName;
}

ParticleExporter::~ParticleExporter()
{}

MBoundingBox ParticleExporter::exportIt()
{
	MBoundingBox bbox;
	MFnParticleSystem particleFn(this->particleObject);
	uint numParticles = particleFn.count();
	if( numParticles == 0)
		return bbox;
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
	outFile.write('HSFF');
	outFile.write('PART');
	//outFile << 'HSFF'; // file identifier
	//outFile << 'PART'; // particle identifier

	MFn::Type pType = particleFn.type();

	outFile.write(numParticles); // uint
	
	MFnParticleSystem::RenderType rType = particleFn.renderType();
	MVectorArray pos;
	//particleFn.position(pos);
	MVectorArray vel;
	//particleFn.velocity(vel);
	MDoubleArray radius;
	//particleFn.radius(radius);
	MVectorArray rgbPP;
	MDoubleArray opPP;
	MDoubleArray lifespanPP;

	uint attrNum = particleFn.getPerParticleAttribute("position", pos);
	uint velNum = particleFn.getPerParticleAttribute("velocity", vel);
	uint radiusNum = particleFn.getPerParticleAttribute("radiusPP", radius);
	uint rgbNum = particleFn.getPerParticleAttribute("rgbPP", rgbPP);
	uint opaNum = particleFn.getPerParticleAttribute("opacityPP", opPP);
	uint lifeNum = particleFn.getPerParticleAttribute("lifespanPP", lifespanPP);

	if( !particleFn.isValid())
	{
		logger.debug(MString("Particle system is invalid."));
		return bbox;
	}

	outFile.writeParticleAttr(MString("position"), VECTOR);
	MPoint min, max;
	outFile.write(pos, min, max); // save size in bounding box

	outFile.writeParticleAttr(MString("velocity"), VECTOR);
	outFile.write(vel);
	outFile.writeParticleAttr(MString("radiusPP"), DOUBLE);
	float maxRadius = 0.0;
	for( uint i = 0; i < radiusNum; i++)
	{
		radius[i] *= 20.0;
		if( radius[i] > maxRadius)
			maxRadius = (float)radius[i];
	}
	outFile.write(radius);
	
	// add radius
	min.x -= maxRadius;
	min.y -= maxRadius;
	min.z -= maxRadius;
	max.x += maxRadius;
	max.y += maxRadius;
	max.z += maxRadius;
	bbox.expand(max);
	bbox.expand(min);

	if( rgbNum > 0 )
	{
		outFile.writeParticleAttr(MString("rgbPP"), COLOR);
		outFile.write(rgbPP);
	}
	if( opaNum > 0 )
	{
		outFile.writeParticleAttr(MString("opacityPP"), DOUBLE);
		outFile.write(opPP);
	}
	if( lifeNum > 0)
	{
		outFile.writeParticleAttr(MString("lifespanPP"), DOUBLE);
		outFile.write(lifespanPP);
	}

	outFile.close();
	return bbox;
}

