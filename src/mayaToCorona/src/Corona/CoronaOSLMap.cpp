#include "Corona.h"
#include "CoronaOSLMap.h"

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

	sg.u = uvw.x() * 5;
	sg.v = uvw.y() * 5;

    // Assume that position P is simply (u,v,1), that makes the patch lie
    // on [0,1] at z=1.
    // Derivatives with respect to x,y
    sg.dPdx = OSL::Vec3 (sg.dudx, sg.dudy, 0.0f);
    sg.dPdy = OSL::Vec3 (sg.dvdx, sg.dvdy, 0.0f);
    sg.dPdz = OSL::Vec3 (0.0f, 0.0f, 0.0f);  // just use 0 for volume tangent
    // Tangents of P with respect to surface u,v
    sg.dPdu = OSL::Vec3 (1.0f, 0.0f, 0.0f);
    sg.dPdv = OSL::Vec3 (0.0f, 1.0f, 0.0f);
    // That also implies that our normal points to (0,0,1)
    sg.N    = OSL::Vec3 (0, 0, 1);
    sg.Ng   = OSL::Vec3 (0, 0, 1);

    // Set the surface area of the patch to 1 (which it is).  This is
    // only used for light shaders that call the surfacearea() function.
    sg.surfacearea = 1;
}


Corona::Rgb OSLMap::evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
	OSL::ShadingContext *ctx = this->coronaRenderer->oslRenderer.shadingsys->get_context();

	OSL::Matrix44 Mshad;
	OSL::Matrix44 Mobj;
	OSL::ShaderGlobals sg;
	setShadingGlobals(context, sg, 0, 0, Mshad, Mobj);

	float rgb[3] = {0,0,0};

	if(this->coronaRenderer->oslRenderer.shadingsys->execute (*ctx, *this->coronaRenderer->oslRenderer.shadergroup, sg))
	{
		OSL::TypeDesc t;
		const void *data = this->coronaRenderer->oslRenderer.shadingsys->get_symbol (*ctx, this->coronaRenderer->oslRenderer.outputVar, t);
		if (!data)
		{
			
		}else{
			if(t.basetype == OSL::TypeDesc::FLOAT)
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

	this->coronaRenderer->oslRenderer.shadingsys->release_context (ctx);

    outAlpha = 1.f;
	Corona::Rgb col(rgb[0], rgb[1], rgb[2]);
	return col;	
	//return Corona::Rgb::BLUE;
}

float OSLMap::evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha)
{
    return evalColor(context, cache, outAlpha).grayValue();
}


Corona::Dir OSLMap::evalBump(const Corona::IShadeContext&, Corona::TextureCache*) 
{
        STOP; //currently not supported
}

void OSLMap::renderTo(Corona::Bitmap<Corona::Rgb>& output) 
{
        STOP; //currently not supported
}

