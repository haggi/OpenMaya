#ifndef LAMBTST_H
#define LAMBTST_H

#include <maya/MIOStream.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MDrawRegistry.h>

class TestShader : public MPxNode
{
public:
	TestShader();
	virtual			~TestShader();

	static  void *	creator();
	virtual MStatus	compute(const MPlug&, MDataBlock&);
	static  MStatus	initialize();
	virtual void	postConstructor();

	static  MTypeId   id;  // The IFF type id

protected:

	static MObject  aColor;
	static MObject  aOutColor;
};
#endif