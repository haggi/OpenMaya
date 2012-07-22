#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include "utilities/logging.h"
#include "rendercmd.h"

static Logging logger;

void* MayaToRendererCmd::creator()
{
	return new MayaToRendererCmd();
}

MayaToRendererCmd::~MayaToRendererCmd() {}

MSyntax MayaToRendererCmd::newSyntax()
{
	MSyntax				syntax;

	return syntax;
}

MStatus MayaToRendererCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	
	return MStatus::kSuccess;
}
