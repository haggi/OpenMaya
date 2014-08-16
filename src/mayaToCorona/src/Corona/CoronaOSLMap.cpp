#include "Corona.h"
#include "CoronaOSLMap.h"
#include "utilities/logging.h"

static Logging logger;

OSLMap::OSLMap()
{
	this->bumpType = NONE;
}

void OSLMap::setShadingGlobals(const Corona::IShadeContext& context, OSL::ShaderGlobals &sg, int x, int y, OSL::Matrix44& Mshad, OSL::Matrix44& Mobj)
{
	//this->coronaRenderer->oslRenderer.renderer.setup_transformations(Mshad, Mobj);

    // Just zero the whole thing out to start
    memset(&sg, 0, sizeof(OSL::ShaderGlobals));

    // Set "shader" space to be Mshad.  In a real renderer, this may be
    // different for each shader group.
    sg.shader2common = OSL::TransformationPtr(&Mshad);

    // Set "object" space to be Mobj.  In a real renderer, this may be
    // different for each object.
    sg.object2common = OSL::TransformationPtr(&Mobj);
	
    // Just make it look like all shades are the result of 'raytype' rays.
	std::string raytype = "camera";
	sg.raytype = this->coronaRenderer->oslRenderer.shadingsys->raytype_bit (OSL::ustring(raytype));
	
	Corona::Pos uvw = context.getMapCoords(0);
	Corona::Pos pos = context.getPosition();
    sg.P = OSL::Vec3 (pos.x(), pos.y(), pos.z());	
	sg.u = uvw.x();
	sg.v = uvw.y();
	
	Corona::Ray I = context.getRay();
	sg.I = OSL::Vec3(I.direction.x(), I.direction.y(), I.direction.z());

    sg.dPdx = OSL::Vec3 (sg.dudx, sg.dudy, 0.0f);
    sg.dPdy = OSL::Vec3 (sg.dvdx, sg.dvdy, 0.0f);
    sg.dPdz = OSL::Vec3 (0.0f, 0.0f, 0.0f);  // just use 0 for volume tangent

    // Tangents of P with respect to surface u,v
	Corona::Dir duvw = context.dUvw(0);

	Corona::Dir Ng = context.getGeometryNormal();
	Corona::Dir N = context.getShadingNormal();

	Corona::Matrix33 base = context.bumpBase(0);
	Corona::Dir T = base.getColumn(0);
	Corona::Dir No = base.getColumn(1);
	Corona::Dir C = base.getColumn(2);

	sg.dudx = duvw.x();
	sg.dudy = duvw.y();

	T *= duvw.x() * 0.01;
	C *= duvw.y() * 0.01;

	//sg.dPdv = OSL::Vec3(duvw.x(), duvw.y(), duvw.z());

	sg.dPdu = OSL::Vec3(T.x(), T.y(), T.z());
	sg.dPdv = OSL::Vec3(C.x(), C.y(), C.z());

    sg.N    = OSL::Vec3(N.x(), N.y(), N.z());
    sg.Ng   = OSL::Vec3(Ng.x(), Ng.y(), Ng.z());

    // Set the surface area of the patch to 1 (which it is).  This is
    // only used for light shaders that call the surfacearea() function.
    sg.surfacearea = 1;
}


Corona::Rgb OSLMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{

	int threadId = context.getThreadId();
	OSL::PerThreadInfo *thread_info = NULL;
	if( this->coronaRenderer->oslRenderer.thread_info[threadId] == NULL)
	{
		this->coronaRenderer->oslRenderer.thread_info[threadId] = this->coronaRenderer->oslRenderer.shadingsys->create_thread_info();
	}
	thread_info = this->coronaRenderer->oslRenderer.thread_info[threadId];
	OSL::ShadingContext *ctx = NULL;
	if( this->coronaRenderer->oslRenderer.ctx[threadId] == NULL)
	{
		this->coronaRenderer->oslRenderer.ctx[threadId] = this->coronaRenderer->oslRenderer.shadingsys->get_context(thread_info);
	}
	ctx = this->coronaRenderer->oslRenderer.ctx[threadId];

	//OSL::ShadingContext *ctx = this->coronaRenderer->oslRenderer.shadingsys->get_context(thread_info);
	//OSL::ShadingContext *ctx = this->coronaRenderer->oslRenderer.shadingsys->get_context();
	//OSL::ShadingContext *ctx = this->coronaRenderer->oslRenderer.ctx[threadId];

	OSL::Matrix44 Mshad;
	OSL::Matrix44 Mobj;
	OSL::ShaderGlobals sg;
	setShadingGlobals(context, sg, 0, 0, Mshad, Mobj);

	float rgb[3] = {0,0,0};

	OSL::TypeDesc t;
	if (this->coronaRenderer->oslRenderer.shadingsys->execute(*ctx, *this->shaderGroup, sg))
	{
		const void *data = this->coronaRenderer->oslRenderer.shadingsys->get_symbol(*ctx, this->coronaRenderer->oslRenderer.outputVar, t);
		if (data)
		{
			if (t.basetype == OSL::TypeDesc::FLOAT)
			{
				float r, g, b;
				float *d = (float *)data;
				r = *d++;
				g = *d++;
				b = *d;
				rgb[0] = r;
				rgb[1] = g;
				rgb[2] = b;
			}
		}
	}
    outAlpha = 1.f;
	Corona::Rgb col(rgb[0], rgb[1], rgb[2]);
	return col;	
}

float OSLMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
    return evalColor(context, cache, outAlpha).grayValue();
}


Corona::Dir OSLMap::evalBump(const Corona::IShadeContext& context, Corona::TextureCache* cache)
{
	Corona::Dir normal(0,1,0);

	if (this->bumpType == NONE)
		return context.getShadingNormal();

	if (this->bumpType == NORMALTANGENT)
	{
		Corona::Matrix33 base = context.bumpBase(0);
		float outAlpha = 0.0f;
		Corona::Rgb col = evalColor(context, cache, outAlpha);
		Corona::Dir normal;
		normal.x() = (col.r() - 0.5) * 2.0;
		normal.y() = (col.g() - 0.5) * 2.0;
		normal.z() = (col.b() - 0.5) * 2.0;
		normal = base.transform(normal);
		return normal;
	}
	if (this->bumpType == NORMALOBJECT)
	{
		float outAlpha = 0.0f;
		Corona::Rgb col = evalColor(context, cache, outAlpha);
		Corona::Dir normal;
		normal.x() = (col.r() - 0.5) * 2.0;
		normal.y() = (col.g() - 0.5) * 2.0;
		normal.z() = (col.b() - 0.5) * 2.0;
		return normal;
	}
	if (this->bumpType == BUMP)
	{
		float outAlpha = 0.0f;
		Corona::Rgb col = evalColor(context, cache, outAlpha);
		Corona::Dir normal;
		normal.x() = col.r();
		normal.y() = col.g();
		normal.z() = col.b();
		return normal;
	}
}

void OSLMap::renderTo(Corona::Bitmap<Corona::Rgb>& output) 
{
        STOP; //currently not supported
}

OSLMap::~OSLMap()
{
	logger.debug(MString("OSLMap resetting shader group"));
	this->shaderGroup.reset();
}
