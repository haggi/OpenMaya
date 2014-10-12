#include "world.h"

void *getObjPtr(MString name)
{
	if (worldPointer != NULL)
		return worldPointer->getObjPtr(name);
	return NULL;
}

static void addObjectPtr(MString name, void *ptr)
{
	if (worldPointer != NULL)
		worldPointer->addObjectPtr(name, ptr);
}

void deleteWorld()
{
	if (worldPointer != NULL)
		delete worldPointer;
	worldPointer = NULL;
}

void defineWorld()
{
	deleteWorld();
	worldPointer = new MayaToWorld();
}

MayaToWorld *getWorldPtr()
{
	return worldPointer;
}