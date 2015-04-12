#ifndef MTAP_CMD_H
#define MTAP_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOCMDNAME "mayatoappleseed"

class  MayaToAppleseed: public MPxCommand
{
public:
					MayaToAppleseed();
	virtual			~MayaToAppleseed(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();
	void setLogLevel();

private:
};

#endif