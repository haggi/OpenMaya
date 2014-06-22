#include "oslRenderer.h"

#include "OSL/oslexec.h"
#include "OSL/genclosure.h"
using namespace OSL;

OSL_NAMESPACE_ENTER

static ustring u_camera("camera"), u_screen("screen");
static ustring u_NDC("NDC"), u_raster("raster");
static ustring u_perspective("perspective");

SimpleRenderer::SimpleRenderer ()
{
    Matrix44 M;  M.makeIdentity();
    camera_params (M, u_perspective, 90.0f,
                   0.1f, 1000.0f, 256, 256);
}



void
SimpleRenderer::camera_params (const Matrix44 &world_to_camera,
                               ustring projection, float hfov,
                               float hither, float yon,
                               int xres, int yres)
{
    m_world_to_camera = world_to_camera;
    m_projection = projection;
    m_fov = hfov;
    m_hither = hither;
    m_yon = yon;
    m_xres = xres;
    m_yres = yres;
}



bool
SimpleRenderer::get_matrix (Matrix44 &result, TransformationPtr xform,
                            float time)
{
    // SimpleRenderer doesn't understand motion blur and transformations
    // are just simple 4x4 matrices.
    result = *(OSL::Matrix44 *)xform;
    return true;
}



bool
SimpleRenderer::get_matrix (Matrix44 &result, ustring from, float time)
{
    TransformMap::const_iterator found = m_named_xforms.find (from);
    if (found != m_named_xforms.end()) {
        result = *(found->second);
        return true;
    } else {
        return false;
    }
}



bool
SimpleRenderer::get_matrix (Matrix44 &result, TransformationPtr xform)
{
    // SimpleRenderer doesn't understand motion blur and transformations
    // are just simple 4x4 matrices.
    result = *(OSL::Matrix44 *)xform;
    return true;
}



bool
SimpleRenderer::get_matrix (Matrix44 &result, ustring from)
{
    // SimpleRenderer doesn't understand motion blur, so we never fail
    // on account of time-varying transformations.
    TransformMap::const_iterator found = m_named_xforms.find (from);
    if (found != m_named_xforms.end()) {
        result = *(found->second);
        return true;
    } else {
        return false;
    }
}



bool
SimpleRenderer::get_inverse_matrix (Matrix44 &result, ustring to, float time)
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



void
SimpleRenderer::name_transform (const char *name, const OSL::Matrix44 &xform)
{
    shared_ptr<Transformation> M (new OSL::Matrix44 (xform));
    m_named_xforms[ustring(name)] = M;
}

bool
SimpleRenderer::get_array_attribute (void *renderstate, bool derivatives, ustring object,
                                     TypeDesc type, ustring name,
                                     int index, void *val)
{
    return false;
}

bool
SimpleRenderer::get_attribute (void *renderstate, bool derivatives, ustring object,
                               TypeDesc type, ustring name, void *val)
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

bool
SimpleRenderer::get_userdata (bool derivatives, ustring name, TypeDesc type, void *renderstate, void *val)
{
    return false;
}

bool
SimpleRenderer::has_userdata (ustring name, TypeDesc type, void *renderstate)
{
    return false;
}

void OSLShadingNetworkRenderer::setShaderSearchPath(std::string path)
{
	this->shaderSearchPath = path;
}

void OSLShadingNetworkRenderer::addShaderSearchPath(std::string path)
{
	if( this->shaderSearchPath.size() > 0)
		this->shaderSearchPath = this->shaderSearchPath + ";" + path;
	else
		this->shaderSearchPath = path;
}

void OSLShadingNetworkRenderer::setResolution(int x, int y)
{
	this->resX = x;
	this->resY = y;
	this->renderer.resX = this->resX;
	this->renderer.resY = this->resY;
}

void SimpleRenderer::setup_transformations (OSL::Matrix44 &Mshad, OSL::Matrix44 &Mobj)
{
    Matrix44 M (1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    this->camera_params (M, ustring("perspective"), 90.0f, 0.1f, 1000.0f, this->resX, this->resY);

    // Make a "shader" space that is translated one unit in x and rotated
    // 45deg about the z axis.
    Mshad.makeIdentity ();
    Mshad.translate (OSL::Vec3 (1.0, 0.0, 0.0));
    Mshad.rotate (OSL::Vec3 (0.0, 0.0, M_PI_2 * 0.5));
    // std::cout << "shader-to-common matrix: " << Mshad << "\n";

    // Make an object space that is translated one unit in y and rotated
    // 90deg about the z axis.
    Mobj.makeIdentity ();
    Mobj.translate (OSL::Vec3 (0.0, 1.0, 0.0));
    Mobj.rotate (OSL::Vec3 (0.0, 0.0, M_PI_2));
    // std::cout << "object-to-common matrix: " << Mobj << "\n";
}

void OSLShadingNetworkRenderer::setup()
{
	this->shadingsys = OSL::ShadingSystem::create (&renderer, NULL, &this->errorHandler);
	this->shadingsys->attribute("lockgeom", 1);
	this->shadingsys->attribute ("searchpath:shader", this->shaderSearchPath);
	const char *n = "Cout";
	shadingsys->attribute("renderer_outputs", TypeDesc(TypeDesc::STRING,1),&n);

	//std::vector<const char *> aovnames(2);
	//aovnames[0] = "Cout";
	//aovnames[1] = "fout";
	//shadingsys->attribute ("renderer_outputs", TypeDesc(TypeDesc::STRING,(int)aovnames.size()), &aovnames[0]);

}

void OSLShadingNetworkRenderer::createDummyShader()
{
	this->shadergroup = this->shadingsys->ShaderGroupBegin();
	float colorA[3] = {1,0,0};
	float colorB[3] = {0,0,1};
	this->shadingsys->Parameter("colorA", TypeDesc::TypeColor, colorA);
	this->shadingsys->Parameter("colorB", TypeDesc::TypeColor, colorB);
    this->shadingsys->Shader("surface", "H:/UserDatenHaggi/Documents/coding/OpenShadingLanguage/testsuite/aastep/blubb", NULL);
    if(!this->shadingsys->ShaderGroupEnd ())
		std::cout << "error shading group\n";
}


OSL_NAMESPACE_EXIT
