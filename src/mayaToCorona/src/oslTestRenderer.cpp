#include "oslTestRenderer.h"

using namespace OSL;

static ustring u_camera("camera"), u_screen("screen");
static ustring u_NDC("NDC"), u_raster("raster");
static ustring u_perspective("perspective");

TestRenderer::TestRenderer ()
{
    Matrix44 M;  M.makeIdentity();
    camera_params (M, u_perspective, 90.0f, 0.1f, 1000.0f, 256, 256);
}



void TestRenderer::camera_params (const Matrix44 &world_to_camera, ustring projection, float hfov, float hither, float yon, int xres, int yres)
{
    m_world_to_camera = world_to_camera;
    m_projection = projection;
    m_fov = hfov;
    m_hither = hither;
    m_yon = yon;
    m_xres = xres;
    m_yres = yres;
}



bool TestRenderer::get_matrix (Matrix44 &result, TransformationPtr xform, float time)
{
    result = *(OSL::Matrix44 *)xform;
    return true;
}



bool TestRenderer::get_matrix (Matrix44 &result, ustring from, float time)
{
    TransformMap::const_iterator found = m_named_xforms.find (from);
    if (found != m_named_xforms.end()) {
        result = *(found->second);
        return true;
    } else {
        return false;
    }
}



bool TestRenderer::get_matrix (Matrix44 &result, TransformationPtr xform)
{
    result = *(OSL::Matrix44 *)xform;
    return true;
}



bool TestRenderer::get_matrix (Matrix44 &result, ustring from)
{
    TransformMap::const_iterator found = m_named_xforms.find (from);
    if (found != m_named_xforms.end()) {
        result = *(found->second);
        return true;
    } else {
        return false;
    }
}



bool TestRenderer::get_inverse_matrix (Matrix44 &result, ustring to, float time)
{
    if (to == u_camera || to == u_screen || to == u_NDC || to == u_raster) {
        Matrix44 M = m_world_to_camera;
        if (to == u_screen || to == u_NDC || to == u_raster) {
            float depthrange = (double)m_yon-(double)m_hither;
            static ustring u_perspective("perspective");
            if (m_projection == u_perspective) {
                float tanhalffov = tanf (0.5f * m_fov * M_PI/180.0);
                Matrix44 camera_to_screen (1/tanhalffov, 0, 0, 0,
                                           0, 1/tanhalffov, 0, 0,
                                           0, 0, m_yon/depthrange, 1,
                                           0, 0, -m_yon*m_hither/depthrange, 0);
                M = M * camera_to_screen;
            } else {
                Matrix44 camera_to_screen (1, 0, 0, 0,
                                           0, 1, 0, 0,
                                           0, 0, 1/depthrange, 0,
                                           0, 0, -m_hither/depthrange, 1);
                M = M * camera_to_screen;
            }
            if (to == u_NDC || to == u_raster) {
                float screenleft = -1.0, screenwidth = 2.0;
                float screenbottom = -1.0, screenheight = 2.0;
                Matrix44 screen_to_ndc (1/screenwidth, 0, 0, 0,
                                        0, 1/screenheight, 0, 0,
                                        0, 0, 1, 0,
                                        -screenleft/screenwidth, -screenbottom/screenheight, 0, 1);
                M = M * screen_to_ndc;
                if (to == u_raster) {
                    Matrix44 ndc_to_raster (m_xres, 0, 0, 0,
                                            0, m_yres, 0, 0,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1);
                    M = M * ndc_to_raster;
                }
            }
        }
        result = M;
        return true;
    }

    TransformMap::const_iterator found = m_named_xforms.find (to);
    if (found != m_named_xforms.end()) {
        result = *(found->second);
        result.invert();
        return true;
    } else {
        return false;
    }
}



void TestRenderer::name_transform (const char *name, const OSL::Matrix44 &xform)
{
    shared_ptr<Transformation> M (new OSL::Matrix44 (xform));
    m_named_xforms[ustring(name)] = M;
}

bool TestRenderer::get_array_attribute (void *renderstate, bool derivatives, ustring object,
                                     TypeDesc type, ustring name,
                                     int index, void *val)
{
    return false;
}

bool TestRenderer::get_attribute (void *renderstate, bool derivatives, ustring object, TypeDesc type, ustring name, void *val)
{
    // In order to test getattribute(), respond positively to
    // "options"/"blahblah"
    if (object == "options" && name == "blahblah" &&
        type == TypeDesc::TypeFloat) {
        *(float *)val = 3.14159;
        return true;
    }
    return false;
}

bool TestRenderer::get_userdata (bool derivatives, ustring name, TypeDesc type, void *renderstate, void *val)
{
    return false;
}

bool TestRenderer::has_userdata (ustring name, TypeDesc type, void *renderstate)
{
    return false;
}

void OSLShadingNetworkRenderer::setShaderglobals(int x, int y)
{
    memset(&shaderglobals, 0, sizeof(ShaderGlobals));

	float xres = 256;
	float yres = 256;
    
	shaderglobals.u = (float) (x + 0.5f) / xres;
    shaderglobals.v = (float) (y + 0.5f) / yres;
    shaderglobals.dudx = 1.0f / xres;
    shaderglobals.dvdy = 1.0f / yres;

    shaderglobals.P = Vec3(shaderglobals.u, shaderglobals.v, 1.0f);

    shaderglobals.dPdx = Vec3(shaderglobals.dudx, shaderglobals.dudy, 0.0f);
    shaderglobals.dPdy = Vec3(shaderglobals.dvdx, shaderglobals.dudy, 0.0f);
    shaderglobals.dPdz = Vec3(0.0f, 0.0f, 0.0f);

    shaderglobals.dPdu = Vec3(1.0f, 0.0f, 0.0f);
    shaderglobals.dPdu = Vec3(0.0f, 1.0f, 0.0f);

    shaderglobals.N = Vec3(0, 0, 1);
    shaderglobals.Ng = Vec3(0, 0, 1);

    shaderglobals.surfacearea = 1;
}

void OSLShadingNetworkRenderer::setupShadingSystem()
{
	shadingsys = ShadingSystem::create(&this->renderer, NULL, &this->errorHandler);
    shadingsys->attribute("lockgeom", 1);
	shadingsys->attribute ("searchpath:shader", "H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToCorona/mtco_devmodule/shader");
}

void OSLShadingNetworkRenderer::defineShaders()
{
    shadergroup = shadingsys->ShaderGroupBegin();
    //shadingsys->Parameter("paramname", TypeDesc paramtype, void *value);
    if(shadingsys->Shader("surface", "test", NULL))
		std::cout << "Failed to define shader test" << std::endl;
    shadingsys->ShaderGroupEnd();
}

bool OSLShadingNetworkRenderer::executeShader()
{
	ShadingContext *ctx = shadingsys->get_context();
	return shadingsys->execute(*ctx, *shadergroup, shaderglobals);
}

bool OSLShadingNetworkRenderer::executeShader(float *rgb)
{
	ShadingContext *ctx = shadingsys->get_context();
	if( shadingsys->execute(*ctx, *shadergroup, shaderglobals) )
	{
		TypeDesc t;
		OIIO::ustring dataName("Cout");
		const void *data = shadingsys->get_symbol (*ctx, dataName, t);
		if (data)
		{
			if (t.basetype == TypeDesc::FLOAT) 
			{
				if(t.vecsemantics == TypeDesc::COLOR)
				{
					rgb[0] = ((float *)data)[0];
					rgb[1] = ((float *)data)[1];
					rgb[2] = ((float *)data)[2];
					return true;
				}else{
					std::cout << "t.vecsemantics not color\n";
				}
			}else{
				std::cout << "TypeDesc::FLOAT not float\n";
			}
		}else{
			std::cout << "shadingsys->get_symbol data == NULL\n";
		}
	}else{
		std::cout << "shadingsys->execute failed\n";
	}
	return false;
}

