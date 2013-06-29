#ifndef BIN_MESH_WRITER_H
#define BIN_MESH_WRITER_H

#include "foundation/mesh/imeshwalker.h"
#include "foundation/mesh/binarymeshfilewriter.h"

#include <string.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>

#include <vector>


namespace asf = foundation;

struct Face{
	MIntArray vtxIds;
	MIntArray normalIds;
	MIntArray uvIds;
};

class MeshWalker
      : public asf::IMeshWalker
{
public:
	MeshWalker(MObject meshMObject);
	MFnMesh			meshFn;
	
	// mesh data
	MFloatArray		u,v;
	MPointArray		points;
	MFloatVectorArray normals;

	std::vector<Face> faceList;

    // Return the name of the mesh.
    virtual const char* get_name() const = 0;

    // Return vertices.
    virtual size_t get_vertex_count() const = 0;
    virtual asf::Vector3d get_vertex(const size_t i) const = 0;

    // Return vertex normals.
    virtual size_t get_vertex_normal_count() const = 0;
    virtual asf::Vector3d get_vertex_normal(const size_t i) const = 0;

    // Return texture coordinates.
    virtual size_t get_tex_coords_count() const = 0;
    virtual asf::Vector2d get_tex_coords(const size_t i) const = 0;

    // Return material slots.
    virtual size_t get_material_slot_count() const = 0;
    virtual const char* get_material_slot(const size_t i) const = 0;

    // Return the number of faces.
    virtual size_t get_face_count() const = 0;

    // Return the number of vertices in a given face.
    virtual size_t get_face_vertex_count(const size_t face_index) const = 0;

    // Return data for a given vertex of a given face.
    virtual size_t get_face_vertex(const size_t face_index, const size_t vertex_index) const = 0;
    virtual size_t get_face_vertex_normal(const size_t face_index, const size_t vertex_index) const = 0;
    virtual size_t get_face_tex_coords(const size_t face_index, const size_t vertex_index) const = 0;

    // Return the material assigned to a given face.
    virtual size_t get_face_material(const size_t face_index) const = 0;
};

class MeshWriter : public asf::BinaryMeshFileWriter
{
};

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