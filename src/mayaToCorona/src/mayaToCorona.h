#ifndef mtco_CMD_H
#define mtco_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOCMDNAME "MayaToCorona"

class  MayaToCorona: public MPxCommand
{
public:
					MayaToCorona();
	virtual			~MayaToCorona(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
};

#endif