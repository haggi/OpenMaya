#ifndef TEST_OSL_H
#define TEST_OSL_H

#include "OSL/oslexec.h"

using namespace OSL;

typedef Matrix44 Transformation;

class TestRenderer : public RendererServices
{
public:
    TestRenderer();
    ~TestRenderer() { };

    virtual bool get_matrix (Matrix44 &result, TransformationPtr xform,  float time);
    virtual bool get_matrix (Matrix44 &result, ustring from, float time);

    virtual bool get_matrix (Matrix44 &result, TransformationPtr xform);
    virtual bool get_matrix (Matrix44 &result, ustring from);
    virtual bool get_inverse_matrix (Matrix44 &result, ustring to, float time);

    void name_transform (const char *name, const Transformation &xform);

    virtual bool get_array_attribute (void *renderstate, bool derivatives, ustring object, TypeDesc type, ustring name, int index, void *val );
    virtual bool get_attribute (void *renderstate, bool derivatives, ustring object,  TypeDesc type, ustring name, void *val);
    virtual bool get_userdata (bool derivatives, ustring name, TypeDesc type, void *renderstate, void *val);
    virtual bool has_userdata (ustring name, TypeDesc type, void *renderstate);

    void camera_params (const Matrix44 &world_to_camera, ustring projection,
                        float hfov, float hither, float yon,
                        int xres, int yres);

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
	TestRenderer renderer;
	OSL::ShadingSystem *shadingsys;
	int xres, yres;
	ErrorHandler errorHandler;
	ShaderGroupRef shadergroup;
	ShaderGlobals shaderglobals;

	OSLShadingNetworkRenderer()
	{
		shadingsys = NULL;
		xres = yres = 256;
	    //shadingsys = OSL::ShadingSystem::create (&renderer, NULL, &errhandler);
		//register_closures(shadingsys);
	};

	~OSLShadingNetworkRenderer(){};

	void setShaderglobals(int x, int y);
	void setupShadingSystem();
	void defineShaders();
	bool executeShader();
	bool executeShader(float *rgb);
};

#endif