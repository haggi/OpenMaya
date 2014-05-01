#pragma once

#include <map>

#include "OSL/oslexec.h"

OSL_NAMESPACE_ENTER

//void register_closures(OSL::ShadingSystem* shadingsys);

class SimpleRenderer : public RendererServices
{
public:
    // Just use 4x4 matrix for transformations
    typedef Matrix44 Transformation;
	int resX, resY;

    SimpleRenderer ();
    ~SimpleRenderer () { }

    virtual bool get_matrix (Matrix44 &result, TransformationPtr xform,
                             float time);
    virtual bool get_matrix (Matrix44 &result, ustring from, float time);

    virtual bool get_matrix (Matrix44 &result, TransformationPtr xform);
    virtual bool get_matrix (Matrix44 &result, ustring from);
    virtual bool get_inverse_matrix (Matrix44 &result, ustring to, float time);

    void name_transform (const char *name, const Transformation &xform);

    virtual bool get_array_attribute (void *renderstate, bool derivatives, 
                                      ustring object, TypeDesc type, ustring name,
                                      int index, void *val );
    virtual bool get_attribute (void *renderstate, bool derivatives, ustring object,
                                TypeDesc type, ustring name, void *val);
    virtual bool get_userdata (bool derivatives, ustring name, TypeDesc type, 
                               void *renderstate, void *val);
    virtual bool has_userdata (ustring name, TypeDesc type, void *renderstate);

    // Super simple camera and display parameters.  Many options not
    // available, no motion blur, etc.
    void camera_params (const Matrix44 &world_to_camera, ustring projection,
                        float hfov, float hither, float yon,
                        int xres, int yres);
	void setup_transformations (OSL::Matrix44 &Mshad, OSL::Matrix44 &Mobj);
                        
private:
    typedef std::map <ustring, shared_ptr<Transformation> > TransformMap;
    TransformMap m_named_xforms;
    Matrix44 m_world_to_camera;
    ustring m_projection;
    float m_fov, m_hither, m_yon;
    int m_xres, m_yres;
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

	OSLShadingNetworkRenderer()
	{
		shadingsys = NULL;
		this->setResolution(256, 256);
		outputVar = "Cout";
	    //shadingsys = OSL::ShadingSystem::create (&renderer, NULL, &errhandler);
	};

	~OSLShadingNetworkRenderer(){};

	void setResolution(int x, int y);
	void setup();
	void createDummyShader();
};


OSL_NAMESPACE_EXIT
