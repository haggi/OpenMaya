#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

class CoronaSkyShader : public MPxNode
{
public:
                    CoronaSkyShader();
    virtual         ~CoronaSkyShader();

    static  void *  creator();
    virtual MStatus compute( const MPlug&, MDataBlock& );
    static  MStatus initialize();

    virtual void    postConstructor();

    static  MTypeId   id;

	static    MObject pSkyModel;
	static    MObject pSkyMultiplier;
	static    MObject pSkyHorizBlur;
	static    MObject pSkyGroundColor;
	static    MObject pSkyAffectGround;
	static    MObject pSkyPreethamTurb;
	static    MObject pSkySunFalloff;
	static    MObject pSkyZenith;
	static    MObject pSkyHorizon;
	static    MObject pSkySunGlow;
	static    MObject pSkySunSideGlow;
	static    MObject pSkySunBleed;
	static    MObject sunSizeMulti;

	static MObject  outColor;

};
