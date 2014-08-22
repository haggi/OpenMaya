#include "CoronaCore/api/Api.h"
#include "OSL/oslexec.h"

class CoronaRenderer;
// basic map class as interface for OSL calls
class OSLMap : public Corona::Abstract::Map 
{
public:
	CoronaRenderer *coronaRenderer;
	enum BumpType { NONE = 0, BUMP, NORMALOBJECT, NORMALTANGENT, NORMALWORLD, GETU, GETV };
	BumpType bumpType;
	OSL::ShaderGroupRef shaderGroup;
	OSLMap();
	~OSLMap();

	void setShadingGlobals(const Corona::IShadeContext& context, OSL::ShaderGlobals &sg, int x, int y, OSL::Matrix44& Mshad, OSL::Matrix44& Mobj);
    virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
    virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha);
    virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*);
    virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output);
};
