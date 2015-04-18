#ifndef RV_CMD_H
#define RV_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MTimerMessage.h>

class  RvCmd : public MPxCommand
{
public:
	RvCmd();
	virtual			~RvCmd();
	//static MSyntax	newSyntax();
	MStatus     	doIt(const MArgList& args);
	static void*	creator();

private:
	MCallbackId	renderCallbackId;
	MCallbackId	postRenderCallbackId;
};

#endif