#ifndef MTAP_CMD_H
#define MTAP_CMD_H

#include <maya/MGlobal.h>
#include "mayarendernodes/rendercmd.h"

#define MAYATOCMDNAME "mayatoappleseed"

class  MayaToAppleseed: public MayaToRendererCmd
{
public:
					MayaToAppleseed() {};
	virtual			~MayaToAppleseed(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
};

#endif