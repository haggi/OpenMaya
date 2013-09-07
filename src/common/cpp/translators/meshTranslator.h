#ifndef MT_MESH_TRANSLATOR_H
#define MT_MESH_TRANSLATOR_H

#include <string.h>
#include <maya/MString.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MObject.h>
#include <vector>

#define CHECK_MSTATUS_AND_RETURNONLY(_status)		\
{ 														\
	MStatus _maya_status = (_status);					\
	if ( MStatus::kSuccess != _maya_status ) 			\
	{													\
		cerr << "\nAPI error detected in " << __FILE__ 	\
			 <<	" at line "	<< __LINE__ << endl;		\
		_maya_status.perror ( "" );						\
		return;								\
	}													\
}

struct Triangle{
	int vtxIds[3];
	int normalIds[3];
};

struct Face{
	std::vector<int> vtxIds;
	std::vector<int> normalIds;
};


class MeshTranslator
{
public:
	MeshTranslator();
	MeshTranslator(MObject meshMObject);
	~MeshTranslator(); 
	bool isGood();

	std::vector<Triangle> triangleArray;
	std::vector<Face> faceArray;
private:
	MObject meshObject;
	MFloatVectorArray normals;
	MPointArray vertices;
	bool good;
};

#endif