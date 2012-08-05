#ifndef MTAP_IPR_CMD_H
#define MTAP_IPR_CMD_H

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>

#define MAYATOCMDIPRNAME "iprtest"

class  IprTestCmd: public MPxCommand
{
public:
					IprTestCmd() {};
	virtual			~IprTestCmd(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();
	static void		startIPR(int dummy);
	static void		addCallbacks();
	static void		removeCallbacks();

private:
};

#endif