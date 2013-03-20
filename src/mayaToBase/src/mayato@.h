#ifndef MTAP_CMD_H
#define MTAP_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOCMDNAME "mayato@"

class  MayaTo@: public MPxCommand
{
public:
					MayaTo@();
	virtual			~MayaTo@(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
};

#endif