#ifndef _MAYA_TO_MANTRA
#define _MAYA_TO_MANTRA

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOMANTRANAME "mayatomantra"

class mayatomantra : public MPxCommand
{
public:
					mayatomantra() {};
	virtual			~mayatomantra(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
	MString basePath;
	MString imagePath;
	MString fileName;
};

#endif
	
