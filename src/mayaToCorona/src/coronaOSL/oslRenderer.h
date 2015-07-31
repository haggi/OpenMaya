#pragma once

#include <map>
#include "OSL/oslexec.h"
#include "osl/oslUtils.h"
#include <OpenImageIO/hash.h>
#include "OSL/oslexec.h"

OSL_NAMESPACE_ENTER

class SimpleRenderer : public RendererServices
{
public:
	// Just use 4x4 matrix for transformations
	typedef Matrix44 Transformation;
	int resX, resY;

	SimpleRenderer();
	~SimpleRenderer() { }

	virtual bool get_matrix(ShaderGlobals *sg, Matrix44 &result,
		TransformationPtr xform,
		float time);
	virtual bool get_matrix(ShaderGlobals *sg, Matrix44 &result,
		ustring from, float time);
	virtual bool get_matrix(ShaderGlobals *sg, Matrix44 &result,
		TransformationPtr xform);
	virtual bool get_matrix(ShaderGlobals *sg, Matrix44 &result,
		ustring from);
	virtual bool get_inverse_matrix(ShaderGlobals *sg, Matrix44 &result,
		ustring to, float time);

	void name_transform(const char *name, const Transformation &xform);

	virtual bool get_array_attribute(ShaderGlobals *sg, bool derivatives,
		ustring object, TypeDesc type, ustring name,
		int index, void *val);
	virtual bool get_attribute(ShaderGlobals *sg, bool derivatives, ustring object,
		TypeDesc type, ustring name, void *val);
	virtual bool get_userdata(bool derivatives, ustring name, TypeDesc type,
		ShaderGlobals *sg, void *val);
	virtual bool has_userdata(ustring name, TypeDesc type, ShaderGlobals *sg);

	// Super simple camera and display parameters.  Many options not
	// available, no motion blur, etc.
	void camera_params(const Matrix44 &world_to_camera, ustring projection,
		float hfov, float hither, float yon,
		int xres, int yres);

private:
	// Camera parameters
	Matrix44 m_world_to_camera;
	ustring m_projection;
	float m_fov, m_pixelaspect, m_hither, m_yon;
	float m_shutter[2];
	float m_screen_window[4];
	int m_xres, m_yres;

	// Named transforms
	typedef std::map <ustring, shared_ptr<Transformation> > TransformMap;
	TransformMap m_named_xforms;

	// Attribute and userdata retrieval -- for fast dispatch, use a hash
	// table to map attribute names to functions that retrieve them. We
	// imagine this to be fairly quick, but for a performance-critical
	// renderer, we would encourage benchmarking various methods and
	// alternate data structures.
	typedef bool (SimpleRenderer::*AttrGetter)(ShaderGlobals *sg, bool derivs,
		ustring object, TypeDesc type,
		ustring name, void *val);
	typedef boost::unordered_map<ustring, AttrGetter, ustringHash> AttrGetterMap;
	AttrGetterMap m_attr_getters;

	// Attribute getters
	bool get_camera_resolution(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_projection(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_fov(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_pixelaspect(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_clip(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_clip_near(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_clip_far(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_shutter(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_shutter_open(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_shutter_close(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);
	bool get_camera_screen_window(ShaderGlobals *sg, bool derivs, ustring object,
		TypeDesc type, ustring name, void *val);

};

class OSLShadingNetworkRenderer
{
public:
	SimpleRenderer renderer;
	OSL::ShadingSystem *shadingsys;
	
	int resX, resY;
	ErrorHandler errorHandler;
	ShaderGroupRef shadergroup;
	ShaderGlobals shaderglobals;
	ustring outputVar;
	std::string shaderSearchPath;

	OSL::PerThreadInfo *thread_info[256];
	OSL::ShadingContext *ctx[256];

	OSLShadingNetworkRenderer()
	{
		shadingsys = nullptr;
		this->setResolution(256, 256);
		outputVar = "Cout";
		for( int i = 0; i < 256; i++)
		{
			thread_info[i] = nullptr;
			ctx[i] = nullptr;
		}
	};

	~OSLShadingNetworkRenderer()
	{
		for( int i = 0; i < 256; i++)
		{
			if( ctx[i] != nullptr )
			{
				shadingsys->release_context(ctx[i]);
				ctx[i] = nullptr;
			}

			if( thread_info[i] != nullptr )
			{
				this->shadingsys->destroy_thread_info(thread_info[i]);
				thread_info[i] = nullptr;
			}
		}
		OSL::ShadingSystem::destroy(this->shadingsys);
	};

	void setResolution(int x, int y);
	void setup();
	void createDummyShader();
	void setShaderSearchPath(std::string path);
	void addShaderSearchPath(std::string path);
};


OSL_NAMESPACE_EXIT

namespace MAYATO_OSL
{
	void createOSLShader(MString& shaderName, OSLParamArray& paramArray, MString type = "shader"); //overwrite this in renderer specific version
	void connectOSLShaders(ConnectionArray& ca); //overwrite this in renderer specific version
};



