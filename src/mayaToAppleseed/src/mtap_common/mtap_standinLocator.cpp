#include "mtap_standinLocator.h"

#include <maya/MFnDependencyNode.h>

MTypeId	mtap_StandinLocator::id(0x0011CF41);

mtap_StandinLocator::mtap_StandinLocator()
{}

mtap_StandinLocator::~mtap_StandinLocator()
{}

MStatus mtap_StandinLocator::initialize()
{
	StandinLocatorNode::initialize();
	return MStatus::kSuccess;
}

void *mtap_StandinLocator::creator()
{
	return new mtap_StandinLocator();
}

void mtap_StandinLocator::postConstructor( )
{
	setMPSafe(true);
	MFnDependencyNode nodefn(thisMObject());
	nodefn.setName("mtap_standinShape#");	
}

MStatus   mtap_StandinLocator::compute( const MPlug&, MDataBlock& )
{
	return MStatus::kSuccess; 
}