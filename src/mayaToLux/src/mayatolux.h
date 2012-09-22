#ifndef MTL_CMD_H
#define MTL_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOCMDNAME "mayatolux"

class  MayaToLux: public MPxCommand
{
public:
					MayaToLux();
	virtual			~MayaToLux(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
};

#endif