#ifndef MAYATO_WORLD_H
#define MAYATO_WORLD_H

#include <maya/MFnDependencyNode.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MImage.h>
#include <vector>


class MayaToWorld;
static MayaToWorld *worldPointer = NULL;

class RenderGlobals;

class MayaToWorld
{
public:
	MayaToWorld() { 
		initialize();
		state = NONE;
		worldRenderGlobals = NULL;
	};
	~MayaToWorld(){ cleanUp(); };

	enum WorldState{
		NONE = 0,
		SWATCHRENDER,
		UIRENDER,
		BATCHRENDER
	};
	WorldState state;
	RenderGlobals *worldRenderGlobals;

	MStringArray objectNames;
	std::vector<void *> objectPtr;
	MImage previousRenderedImage;

	MStringArray shaderSearchPath;

	void *getObjPtr(MString name)
	{
		for (uint i = 0; i < objectNames.length(); i++)
			if (objectNames[i] == name)
				return objectPtr[i];
		return NULL;
	}

	void addObjectPtr(MString name, void *ptr)
	{
		objectNames.append(name);
		objectPtr.push_back(ptr);
	}

	virtual void initialize();
	virtual void cleanUp();
};

void *getObjPtr(MString name);
static void addObjectPtr(MString name, void *ptr);

void deleteWorld();
void defineWorld();
MayaToWorld *getWorldPtr();

#endif // !MAYATO_WORLD_H
