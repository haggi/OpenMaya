#ifndef MT_CMD_H
#define MT_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

class MayaToRendererCmd : public MPxCommand
{
public:
					MayaToRendererCmd(){};
	virtual			~MayaToRendererCmd(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();

private:
	MString basePath;
	MString imagePath;
	MString fileName;
};

#endif
	