#include "Corona.h"
#include "CoronaOSLMap.h"
#include "utilities/logging.h"

static Logging logger;

OSLMap::OSLMap()
{
	this->bumpType = NONE;
	this->worldScale = 1.0f;
	this->isEnvMap = false;
}

void OSLMap::setShadingGlobals(const Corona::IShadeContext& context, OSL::ShaderGlobals &sg, int x, int y, OSL::Matrix44& Mshad, OSL::Matrix44& Mobj)
{
	//this->oslRenderer->renderer.setup_transformations(Mshad, Mobj);

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
	sg.raytype = this->oslRenderer->shadingsys->raytype_bit (OSL::ustring(raytype));
	
	Corona::Pos uvw = context.getMapCoords(0);
	Corona::Pos pos = context.getPosition();
    sg.P = OSL::Vec3 (pos.x(), pos.y(), pos.z());	
	sg.u = uvw.x();
	sg.v = uvw.y();
	Corona::Ray I = context.getRay();
	sg.I = OSL::Vec3(I.direction.x(), I.direction.y(), I.direction.z());

    // Tangents of P with respect to surface u,v
	Corona::Dir duvw = context.dUvw(0);		

	Corona::Dir Ng = context.getGeometryNormal();
	Corona::Dir N = context.getShadingNormal();

	Corona::Matrix33 base = context.bumpBase(0);
	Corona::Dir T = base.tangent();
	
	Corona::Dir No = N;
	Corona::Dir C = base.cotangent();

	float pixelWorldRatio = context.pixelToWorldRatio();

	T *= duvw.x() * pixelWorldRatio;// / pixelWorldRatio;
	C *= duvw.y() * pixelWorldRatio;// / pixelWorldRatio;

	sg.dudx = duvw.x() * pixelWorldRatio;
	sg.dudy = duvw.y() * pixelWorldRatio;

	//sg.dudx = OSL::Vec3(T.x(), T.y(), T.z()).length() * 0.0;
	//sg.dudy = OSL::Vec3(C.x(), C.y(), C.z()).length() * 0.0;

	sg.dvdx = sg.dudx;
	sg.dvdy = sg.dudy;

	sg.dPdx = OSL::Vec3(sg.dudx, sg.dudy, 0.0f);
	sg.dPdy = OSL::Vec3(sg.dvdx, sg.dvdy, 0.0f);
	//sg.dPdz = OSL::Vec3(0.0f, 0.0f, 0.0f);  // just use 0 for volume tangent

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
	int threadId = 0;
	threadId = context.getThreadId();
	OSL::PerThreadInfo *thread_info = NULL;
	if( this->oslRenderer->thread_info[threadId] == NULL)
	{
		this->oslRenderer->thread_info[threadId] = this->oslRenderer->shadingsys->create_thread_info();
	}
	thread_info = this->oslRenderer->thread_info[threadId];
	OSL::ShadingContext *ctx = NULL;
	if( this->oslRenderer->ctx[threadId] == NULL)
	{
		this->oslRenderer->ctx[threadId] = this->oslRenderer->shadingsys->get_context(thread_info);
	}
	ctx = this->oslRenderer->ctx[threadId];

	OSL::Matrix44 Mshad;
	OSL::Matrix44 Mobj;
	OSL::ShaderGlobals sg;
	setShadingGlobals(context, sg, 0, 0, Mshad, Mobj);

	float rgb[3] = {0,0,0};

	OSL::TypeDesc t;
	if (this->oslRenderer->shadingsys->execute(*ctx, *this->shaderGroup, sg))
	{
		const void *data = this->oslRenderer->shadingsys->get_symbol(*ctx, this->oslRenderer->outputVar, t);
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

Corona::Rgb OSLMap::evalColorBump(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha, float u, float v)
{
	int threadId = 0;
	threadId = context.getThreadId();
	OSL::PerThreadInfo *thread_info = NULL;
	if (this->oslRenderer->thread_info[threadId] == NULL)
	{
		this->oslRenderer->thread_info[threadId] = this->oslRenderer->shadingsys->create_thread_info();
	}
	thread_info = this->oslRenderer->thread_info[threadId];
	OSL::ShadingContext *ctx = NULL;
	if (this->oslRenderer->ctx[threadId] == NULL)
	{
		this->oslRenderer->ctx[threadId] = this->oslRenderer->shadingsys->get_context(thread_info);
	}
	ctx = this->oslRenderer->ctx[threadId];

	OSL::Matrix44 Mshad;
	OSL::Matrix44 Mobj;
	OSL::ShaderGlobals sg;
	setShadingGlobals(context, sg, 0, 0, Mshad, Mobj);
	
	sg.u = u;
	sg.v = v;

	float rgb[3] = { 0, 0, 0 };

	OSL::TypeDesc t;
	if (this->oslRenderer->shadingsys->execute(*ctx, *this->shaderGroup, sg))
	{
		const void *data = this->oslRenderer->shadingsys->get_symbol(*ctx, this->oslRenderer->outputVar, t);
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

INLINE float rgb2bumpVal(const Corona::Rgb& in)
{
	if (in.r() < 0.0)
		return -1.0 * in.powFast(1.f / 2.2f).grayValue();
	else
		return in.powFast(1.f / 2.2f).grayValue();
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
		//normal = normal.getNormalized() - context.getShadingNormal();
		normal = normal - context.getShadingNormal();
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
		normal = normal.getNormalized();
		return normal;
	}
	if (this->bumpType == BUMP)
	{
		//context.worldToObjectTm;
		const Corona::Matrix33 base = context.bumpBase(0);
		const float pixel2world = context.pixelToWorldRatio();
		const Corona::Dir ddUvw = context.dUvw(0);
		const Corona::Dir dUvw = ddUvw * (pixel2world);
		float outAlpha = 1.0f;
		Corona::Pos uvw = context.getMapCoords(0);
		float multi = 0.01;
		float dx = dUvw.x() * 0.5;
		float dy = dUvw.y() * 0.5;
		float uPlus = rgb2bumpVal(evalColorBump(context, cache, outAlpha, uvw.x() + dx, uvw.y())) * multi;
		float uMinus = rgb2bumpVal(evalColorBump(context, cache, outAlpha, uvw.x() - dx, uvw.y())) * multi;
		float vPlus = rgb2bumpVal(evalColorBump(context, cache, outAlpha, uvw.x(), uvw.y() + dy)) * multi;
		float vMinus = rgb2bumpVal(evalColorBump(context, cache, outAlpha, uvw.x(), uvw.y() - dy)) * multi;

		const float dU = (uMinus - uPlus) / Corona::Utils::max(Corona::EPS, 2 * dUvw.x());
		const float dV = (vMinus - vPlus) / Corona::Utils::max(Corona::EPS, 2 * dUvw.y());
		const Corona::Dir bumpedNormal = (dU*base.tangent() - dV*base.cotangent() + base.mainDir()).getNormalizedApprox();
		//return (bumpedNormal - base.mainDir());
		return (bumpedNormal - context.getShadingNormal());
	}
	return context.getShadingNormal();
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
