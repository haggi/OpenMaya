#include "oslRenderer.h"
#include "world.h"
#include "OSL/oslexec.h"
#include "OSL/genclosure.h"
#include "utilities/logging.h"

using namespace OSL;

static Logging logger;

OSL_NAMESPACE_ENTER

static ustring u_camera("camera"), u_screen("screen");
static ustring u_NDC("NDC"), u_raster("raster");
static ustring u_perspective("perspective");
static ustring u_s("s"), u_t("t");
static TypeDesc TypeFloatArray2(TypeDesc::FLOAT, 2);
static TypeDesc TypeFloatArray4(TypeDesc::FLOAT, 4);
static TypeDesc TypeIntArray2(TypeDesc::INT, 2);

SimpleRenderer::SimpleRenderer()
{
	Matrix44 M;  M.makeIdentity();
	camera_params(M, u_perspective, 90.0f, 0.1f, 1000.0f, 256, 256);

	// Set up getters
	m_attr_getters[ustring("camera:resolution")] = &SimpleRenderer::get_camera_resolution;
	m_attr_getters[ustring("camera:projection")] = &SimpleRenderer::get_camera_projection;
	m_attr_getters[ustring("camera:pixelaspect")] = &SimpleRenderer::get_camera_pixelaspect;
	m_attr_getters[ustring("camera:screen_window")] = &SimpleRenderer::get_camera_screen_window;
	m_attr_getters[ustring("camera:fov")] = &SimpleRenderer::get_camera_fov;
	m_attr_getters[ustring("camera:clip")] = &SimpleRenderer::get_camera_clip;
	m_attr_getters[ustring("camera:clip_near")] = &SimpleRenderer::get_camera_clip_near;
	m_attr_getters[ustring("camera:clip_far")] = &SimpleRenderer::get_camera_clip_far;
	m_attr_getters[ustring("camera:shutter")] = &SimpleRenderer::get_camera_shutter;
	m_attr_getters[ustring("camera:shutter_open")] = &SimpleRenderer::get_camera_shutter_open;
	m_attr_getters[ustring("camera:shutter_close")] = &SimpleRenderer::get_camera_shutter_close;
}



void
SimpleRenderer::camera_params(const Matrix44 &world_to_camera, ustring projection, float hfov, float hither, float yon, int xres, int yres)
{
	m_world_to_camera = world_to_camera;
	m_projection = projection;
	m_fov = hfov;
	m_pixelaspect = 1.0f; // hard-coded
	m_hither = hither;
	m_yon = yon;
	m_shutter[0] = 0.0f; m_shutter[1] = 1.0f;  // hard-coded
	float frame_aspect = float(xres) / float(yres) * m_pixelaspect;
	m_screen_window[0] = -frame_aspect;
	m_screen_window[1] = -1.0f;
	m_screen_window[2] = frame_aspect;
	m_screen_window[3] = 1.0f;
	m_xres = xres;
	m_yres = yres;
}



bool
SimpleRenderer::get_matrix(ShaderGlobals *sg, Matrix44 &result,
TransformationPtr xform,
float time)
{
	// SimpleRenderer doesn't understand motion blur and transformations
	// are just simple 4x4 matrices.
	result = *reinterpret_cast<const Matrix44*>(xform);
	return true;
}



bool
SimpleRenderer::get_matrix(ShaderGlobals *sg, Matrix44 &result,
ustring from, float time)
{
	TransformMap::const_iterator found = m_named_xforms.find(from);
	if (found != m_named_xforms.end()) {
		result = *(found->second);
		return true;
	}
	else {
		return false;
	}
}



bool
SimpleRenderer::get_matrix(ShaderGlobals *sg, Matrix44 &result,
TransformationPtr xform)
{
	// SimpleRenderer doesn't understand motion blur and transformations
	// are just simple 4x4 matrices.
	result = *reinterpret_cast<const Matrix44*>(xform);
	return true;
}



bool
SimpleRenderer::get_matrix(ShaderGlobals *sg, Matrix44 &result,
ustring from)
{
	// SimpleRenderer doesn't understand motion blur, so we never fail
	// on account of time-varying transformations.
	TransformMap::const_iterator found = m_named_xforms.find(from);
	if (found != m_named_xforms.end()) {
		result = *(found->second);
		return true;
	}
	else {
		return false;
	}
}



bool
SimpleRenderer::get_inverse_matrix(ShaderGlobals *sg, Matrix44 &result,
ustring to, float time)
{
	if (to == u_camera || to == u_screen || to == u_NDC || to == u_raster) {
		Matrix44 M = m_world_to_camera;
		if (to == u_screen || to == u_NDC || to == u_raster) {
			float depthrange = (double)m_yon - (double)m_hither;
			if (m_projection == u_perspective) {
				float tanhalffov = tanf(0.5f * m_fov * M_PI / 180.0);
				Matrix44 camera_to_screen(1 / tanhalffov, 0, 0, 0,
					0, 1 / tanhalffov, 0, 0,
					0, 0, m_yon / depthrange, 1,
					0, 0, -m_yon*m_hither / depthrange, 0);
				M = M * camera_to_screen;
			}
			else {
				Matrix44 camera_to_screen(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1 / depthrange, 0,
					0, 0, -m_hither / depthrange, 1);
				M = M * camera_to_screen;
			}
			if (to == u_NDC || to == u_raster) {
				float screenleft = -1.0, screenwidth = 2.0;
				float screenbottom = -1.0, screenheight = 2.0;
				Matrix44 screen_to_ndc(1 / screenwidth, 0, 0, 0,
					0, 1 / screenheight, 0, 0,
					0, 0, 1, 0,
					-screenleft / screenwidth, -screenbottom / screenheight, 0, 1);
				M = M * screen_to_ndc;
				if (to == u_raster) {
					Matrix44 ndc_to_raster(m_xres, 0, 0, 0,
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

	TransformMap::const_iterator found = m_named_xforms.find(to);
	if (found != m_named_xforms.end()) {
		result = *(found->second);
		result.invert();
		return true;
	}
	else {
		return false;
	}
}



void
SimpleRenderer::name_transform(const char *name, const OSL::Matrix44 &xform)
{
	shared_ptr<Transformation> M(new OSL::Matrix44(xform));
	m_named_xforms[ustring(name)] = M;
}



bool
SimpleRenderer::get_array_attribute(ShaderGlobals *sg, bool derivatives, ustring object,
TypeDesc type, ustring name,
int index, void *val)
{
	AttrGetterMap::const_iterator g = m_attr_getters.find(name);
	if (g != m_attr_getters.end()) {
		AttrGetter getter = g->second;
		return (this->*(getter)) (sg, derivatives, object, type, name, val);
	}

	// If no named attribute was found, allow userdata to bind to the
	// attribute request.
	if (object.empty() && index == -1)
		return get_userdata(derivatives, name, type, sg, val);

	return false;
}



bool
SimpleRenderer::get_attribute(ShaderGlobals *sg, bool derivatives, ustring object,
TypeDesc type, ustring name, void *val)
{
	return get_array_attribute(sg, derivatives, object, type, name, -1, val);
}



bool
SimpleRenderer::get_userdata(bool derivatives, ustring name, TypeDesc type,
ShaderGlobals *sg, void *val)
{
	// Just to illustrate how this works, respect s and t userdata, filled
	// in with the uv coordinates.  In a real renderer, it would probably
	// look up something specific to the primitive, rather than have hard-
	// coded names.

	if (name == u_s && type == TypeDesc::TypeFloat) {
		((float *)val)[0] = sg->u;
		if (derivatives) {
			((float *)val)[1] = sg->dudx;
			((float *)val)[2] = sg->dudy;
		}
		return true;
	}
	if (name == u_t && type == TypeDesc::TypeFloat) {
		((float *)val)[0] = sg->v;
		if (derivatives) {
			((float *)val)[1] = sg->dvdx;
			((float *)val)[2] = sg->dvdy;
		}
		return true;
	}

	return false;
}



bool
SimpleRenderer::has_userdata(ustring name, TypeDesc type, ShaderGlobals *sg)
{
	return false;
}



bool
SimpleRenderer::get_camera_resolution(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeIntArray2) {
		((int *)val)[0] = m_xres;
		((int *)val)[1] = m_yres;
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_projection(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeString) {
		((ustring *)val)[0] = m_projection;
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_fov(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	// N.B. in a real rederer, this may be time-dependent
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_fov;
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_pixelaspect(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_pixelaspect;
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_clip(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeFloatArray2) {
		((float *)val)[0] = m_hither;
		((float *)val)[1] = m_yon;
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_clip_near(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_hither;
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_clip_far(ShaderGlobals *sg, bool derivs, ustring object, TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_yon;
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}



bool
SimpleRenderer::get_camera_shutter(ShaderGlobals *sg, bool derivs, ustring object,
TypeDesc type, ustring name, void *val)
{
	if (type == TypeFloatArray2) {
		((float *)val)[0] = m_shutter[0];
		((float *)val)[1] = m_shutter[1];
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_shutter_open(ShaderGlobals *sg, bool derivs, ustring object,
TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_shutter[0];
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_shutter_close(ShaderGlobals *sg, bool derivs, ustring object,
TypeDesc type, ustring name, void *val)
{
	if (type == TypeDesc::TypeFloat) {
		((float *)val)[0] = m_shutter[1];
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
	return false;
}


bool
SimpleRenderer::get_camera_screen_window(ShaderGlobals *sg, bool derivs, ustring object,
TypeDesc type, ustring name, void *val)
{
	// N.B. in a real rederer, this may be time-dependent
	if (type == TypeFloatArray4) {
		((float *)val)[0] = m_screen_window[0];
		((float *)val)[1] = m_screen_window[1];
		((float *)val)[2] = m_screen_window[2];
		((float *)val)[3] = m_screen_window[3];
		if (derivs)
			memset((char *)val + type.size(), 0, 2 * type.size());
		return true;
	}
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

//void SimpleRenderer::setup_transformations (OSL::Matrix44 &Mshad, OSL::Matrix44 &Mobj)
//{
//    Matrix44 M (1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
//    this->camera_params (M, ustring("perspective"), 90.0f, 0.1f, 1000.0f, this->resX, this->resY);
//
//    // Make a "shader" space that is translated one unit in x and rotated
//    // 45deg about the z axis.
//    Mshad.makeIdentity ();
//    Mshad.translate (OSL::Vec3 (1.0, 0.0, 0.0));
//    Mshad.rotate (OSL::Vec3 (0.0, 0.0, M_PI_2 * 0.5));
//    // std::cout << "shader-to-common matrix: " << Mshad << "\n";
//
//    // Make an object space that is translated one unit in y and rotated
//    // 90deg about the z axis.
//    Mobj.makeIdentity ();
//    Mobj.translate (OSL::Vec3 (0.0, 1.0, 0.0));
//    Mobj.rotate (OSL::Vec3 (0.0, 0.0, M_PI_2));
//    // std::cout << "object-to-common matrix: " << Mobj << "\n";
//}

void OSLShadingNetworkRenderer::setup()
{
	if (this->shadingsys != nullptr)
		delete this->shadingsys;
	this->shadingsys = OSL::ShadingSystem::create (&renderer, nullptr, &this->errorHandler);
	OIIO::TextureSystem *ts = this->shadingsys->texturesys();
	ts->attribute("grey_to_rgb", 1);
	this->shadingsys->attribute("lockgeom", 1);
	this->shadingsys->attribute("searchpath:shader", this->shaderSearchPath);
	//this->shadingsys->attribute("strict_messages", true);
	//this->shadingsys->attribute("debug", 2);
	//this->shadingsys->attribute("buffer_printf", 0);
	
	const char *n = "Cout";
	shadingsys->attribute("renderer_outputs", TypeDesc(TypeDesc::STRING,1),&n);

	//std::vector<const char *> aovnames(2);
	//aovnames[0] = "Cout";
	//aovnames[1] = "fout";
	//shadingsys->attribute ("renderer_outputs", TypeDesc(TypeDesc::STRING,(int)aovnames.size()), &aovnames[0]);

	for (int i = 0; i < 256; i++)
	{
		if (ctx[i] != nullptr)
		{
			shadingsys->release_context(ctx[i]);
			ctx[i] = nullptr;
		}

		if (thread_info[i] != nullptr)
		{
			this->shadingsys->destroy_thread_info(thread_info[i]);
			thread_info[i] = nullptr;
		}
	}
}

void OSLShadingNetworkRenderer::createDummyShader()
{
	this->shadergroup = this->shadingsys->ShaderGroupBegin();
	float colorA[3] = {1,0,0};
	float colorB[3] = {0,0,1};
	this->shadingsys->Parameter("colorA", TypeDesc::TypeColor, colorA);
	this->shadingsys->Parameter("colorB", TypeDesc::TypeColor, colorB);
    this->shadingsys->Shader("surface", "H:/UserDatenHaggi/Documents/coding/OpenShadingLanguage/testsuite/aastep/blubb", nullptr);
    if(!this->shadingsys->ShaderGroupEnd ())
		std::cout << "error shading group\n";
}


OSL_NAMESPACE_EXIT

namespace MAYATO_OSL
{
	void createOSLShader(MString& shadingNodeType, MString& shaderName, OSLParamArray& paramArray, MString type)
	{
		//OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslRenderer");
		OSL::OSLShadingNetworkRenderer *r;
		MayaTo::MayaToWorld::WorldRenderType rType = MayaTo::getWorldPtr()->getRenderType();
		if ((rType == MayaTo::MayaToWorld::WorldRenderType::BATCHRENDER) || (rType == MayaTo::MayaToWorld::WorldRenderType::UIRENDER))
		{
			r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslRenderer");
		}
		if ((rType == MayaTo::MayaToWorld::WorldRenderType::SWATCHRENDER))
		{
			r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslSwatchRenderer");
		}

		if (r == nullptr)
		{
			std::cerr << "error createOSLShader: OSL renderer == nullptr\n";
			return;
		}

		for (size_t i = 0; i < paramArray.size(); i++)
		{
			MString name = paramArray[i].name;
			OSL::TypeDesc type = paramArray[i].type;
			std::string tmp;
			float vec[3];
			float m[4][4];
			void *val = nullptr;
			if (type == OSL::TypeDesc::TypeFloat)
				val = boost::get<float>(&paramArray[i].value);
			if (type == OSL::TypeDesc::TypeInt)
				val = boost::get<int>(&paramArray[i].value);
			if (type == OSL::TypeDesc::TypeVector)
			{
				SimpleVector &v = boost::get<SimpleVector>(paramArray[i].value);
				vec[0] = v.f[0];
				vec[1] = v.f[1];
				vec[2] = v.f[2];
				val = vec;
			}
			if (type == OSL::TypeDesc::TypeString)
			{
				tmp = boost::get<std::string>(paramArray[i].value);
				const OIIO::string_ref p = tmp;
				if (!r->shadingsys->Parameter(name.asChar(), type, &p))
					Logging::debug(MString("Problem! createOSLShader defining string parameter: ") + name);
			}
			if (type == OSL::TypeDesc::TypeMatrix)
			{
				SimpleMatrix &v = boost::get<SimpleMatrix>(paramArray[i].value);
				val = v.f;
			}
			if (type != OSL::TypeDesc::TypeString)
				if (!r->shadingsys->Parameter(name.asChar(), type, val))
					Logging::debug(MString("Problem! createOSLShader defining parameter: ") + name);
		}
		if(!r->shadingsys->Shader("surface", shadingNodeType.asChar(), shaderName.asChar()))
			Logging::debug(MString("Problem! createOSLShader defining shader: ") + shaderName);

	}

	void connectOSLShaders(ConnectionArray& ca)
	{
		//OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslRenderer");
		OSL::OSLShadingNetworkRenderer *r;
		MayaTo::MayaToWorld::WorldRenderType rType = MayaTo::getWorldPtr()->getRenderType();
		if ((rType == MayaTo::MayaToWorld::WorldRenderType::BATCHRENDER) || (rType == MayaTo::MayaToWorld::WorldRenderType::UIRENDER))
		{
			r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslRenderer");
		}
		if ((rType == MayaTo::MayaToWorld::WorldRenderType::SWATCHRENDER))
		{
			r = (OSL::OSLShadingNetworkRenderer *)MayaTo::getObjPtr("oslSwatchRenderer");
		}
		if (r == nullptr)
		{
			std::cerr << "error connectOSLShaders: OSL renderer == nullptr\n";
			return;
		}
		for (size_t i = 0; i < ca.size(); i++)
		{
			if (!r->shadingsys->ConnectShaders(ca[i].sourceNode.asChar(), ca[i].sourceAttribute.asChar(), ca[i].destNode.asChar(), ca[i].destAttribute.asChar()))
				Logging::debug(MString("Problem! connectOSLShaders defining connection: ") + ca[i].sourceNode + " to " + ca[i].destNode);
		}
	}
};