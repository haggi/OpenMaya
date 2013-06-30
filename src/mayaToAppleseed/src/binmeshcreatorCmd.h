#ifndef BIN_MESH_WRITER_H
#define BIN_MESH_WRITER_H

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MObject.h>

class  AppleseedBinMeshWriterCmd: public MPxCommand
{
public:
					AppleseedBinMeshWriterCmd();
	virtual			~AppleseedBinMeshWriterCmd(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();
	void			printUsage();
	bool			exportBinMesh(MObject meshObject);
private:
	MString			path;
	MString			meshName;
};

#endif