#ifndef LUX_TEXTURE_H
#define LUX_TEXTURE_H

#include "LuxShaderBase.h"
#include <maya/MPlugArray.h>
#include <maya/MPlug.h>
#include <vector>
#include "utilities/pystring.h"

class LuxTexture : public LuxShadingNode
{
public:
	enum OutTypes{
		FLOAT=0,
		FRESNEL,
		COLOR
	};
	
	std::vector<OutTypes> outTypeArray;
	MString textureName;

	void getOutType()
	{
		//MFnDependencyNode dn(this->nodeObject);
		MPlugArray outPlugs;
		getConnectedOutPlugs(this->nodeObject, outPlugs);
		//if( outPlugs.length() > 1)
		//{
		for( uint plugId = 0; plugId < outPlugs.length(); plugId++)
		{
			logger.debug(MString("Plug connected : ") + outPlugs[plugId].name());
			std::vector<std::string> strArray;
			pystring::split(outPlugs[plugId].name().asChar(), strArray, ".");
			MString lastElement = strArray[strArray.size() - 1].c_str();

			// convenience procedure. If we simply connect outColor and the texture has an
			// luxOutColor - define it as COLOR
			if( pystring::startswith(lastElement.asChar(), "outColor"))
				if( hasPlug(this->nodeObject, MString("luxOutColor")) || this->nodeObject.hasFn(MFn::kFileTexture))
						outTypeArray.push_back(COLOR);

			if( pystring::startswith(lastElement.asChar(), "lux"))
			{
				if( pystring::endswith(lastElement.asChar(), "Float"))
					outTypeArray.push_back(FLOAT);
				if( pystring::endswith(lastElement.asChar(), "Fresnel"))
					outTypeArray.push_back(FRESNEL);
				if( pystring::endswith(lastElement.asChar(), "Color"))
					outTypeArray.push_back(COLOR);
			}
		}
		//}
	}

	LuxTexture(MObject mObject, Instance l): LuxShadingNode(mObject, l)
	{
		getOutType();
	}

	virtual void defineShader()
	{
		for( size_t outId = 0; outId < this->outTypeArray.size(); outId++)
		{
			MString texName = this->nodeName;
			MString outType = "color";
			if(this->outTypeArray[outId] == FLOAT)
			{
				texName = texName + "_outFloat"; 
				outType = "float";
			}
			if(this->outTypeArray[outId] == COLOR)
			{	
				texName = texName + "_outColor"; 
				outType = "color";
			}

			if(this->outTypeArray[outId] == FRESNEL)
			{	
				texName = texName + "_outFresnel"; 
				outType = "fresnel";
			}

			logger.debug(MString("Defining texture ") + this->textureName.asChar() + " as name " + texName);
			this->lux->texture(texName.asChar(), outType.asChar(), this->textureName.asChar(), boost::get_pointer(this->shaderParams));
		}
	}
};

class	MayaFileTexture : public LuxTexture
{
public:
	MayaFileTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
	}
	virtual void defineParams()
	{
		MFnDependencyNode dn(this->nodeObject);
		MString fileName("nofile");
		getString(MString("fileTextureName"), dn, fileName);
		const char *fn = fileName.asChar();
		//logger.debug(MString("tex file name: ") + fileName);
		this->shaderParams->AddString("filename", &fn);
		MColor gain;
		getColor(MString("colorGain"), dn, gain);
		float g = (gain.r + gain.g + gain.b) / 3.0f; 
		this->shaderParams->AddFloat("gain", &g);
	}

	virtual void defineShader()
	{
		MString texName = this->nodeName;
		texName = texName + "_outColor"; 
		logger.debug(MString("Creating maya file texture connection for: ") + this->nodeName);
		this->lux->texture(texName.asChar(), "color", "imagemap", boost::get_pointer(this->shaderParams));
	}

};

//---------------------------- automatically created attributes start ------------------------------------
class	BlackbodyTexture : public LuxTexture
{
public:
	BlackbodyTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blackbody";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "temperature";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	ConstantTexture : public LuxTexture
{
public:
	ConstantTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "constant";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "color";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	Blender_distortednoiseTexture : public LuxTexture
{
public:
	Blender_distortednoiseTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blender_distortednoise";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "noisesize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisebasis";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "noisedepth";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "bright";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "contrast";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "type";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "distamount";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	FresnelTexture : public LuxTexture
{
public:
	FresnelTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "fresnel";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "filename";
		p.ptype = AttrParam::STRING;
		this->params.push_back(p);
		p.paramName = "name";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	GaussianTexture : public LuxTexture
{
public:
	GaussianTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "gaussian";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "wavelength";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "energy";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "width";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	Blender_marbleTexture : public LuxTexture
{
public:
	Blender_marbleTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blender_marble";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "noisesize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisebasis2";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "noisebasis";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "noisedepth";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "turbulance";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "bright";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "noisetype";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "type";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "contrast";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	WindyTexture : public LuxTexture
{
public:
	WindyTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "windy";
		MFnDependencyNode dn(mObject);
		AttrParam p;
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	ColordepthTexture : public LuxTexture
{
public:
	ColordepthTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "colordepth";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "depth";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kt";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	MediumclearTexture : public LuxTexture
{
public:
	MediumclearTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "mediumclear";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "absorption";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "fresnel";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	FresnelcolorTexture : public LuxTexture
{
public:
	FresnelcolorTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "fresnelcolor";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	MediumhomogenusTexture : public LuxTexture
{
public:
	MediumhomogenusTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "mediumhomogenus";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "g2";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "g1";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "sigma_s";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "g";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "sigma_a";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	HarlequinTexture : public LuxTexture
{
public:
	HarlequinTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "harlequin";
		MFnDependencyNode dn(mObject);
		AttrParam p;
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	AddTexture : public LuxTexture
{
public:
	AddTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "add";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "tex2";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "tex1";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	FbmTexture : public LuxTexture
{
public:
	FbmTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "fbm";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "octaves";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "roughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	BilerpTexture : public LuxTexture
{
public:
	BilerpTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "bilerp";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "v11";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "v01";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "v00";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "vdelta";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "udelta";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "mapping";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "v1";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "v2";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "uscale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "v10";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "vscale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	BrickTexture : public LuxTexture
{
public:
	BrickTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "brick";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "brickbevel";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "brickbond";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "mortarsize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "brickwidth";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "bricktex";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "brickdepth";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "brickmodtex";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "motartex";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "brickrun";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "brickheight";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	TabulateddataTexture : public LuxTexture
{
public:
	TabulateddataTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "tabulateddata";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "filename";
		p.ptype = AttrParam::STRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	Blender_musgraveTexture : public LuxTexture
{
public:
	Blender_musgraveTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blender_musgrave";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "noisesize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "outscale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "h";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "octs";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "bright";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "lacu";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "gain";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "offset";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisebasis";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "type";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "contrast";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	DotsTexture : public LuxTexture
{
public:
	DotsTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "dots";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "outside";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "vdelta";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "udelta";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "mapping";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "v1";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "v2";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "uscale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "inside";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "vscale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	BandTexture : public LuxTexture
{
public:
	BandTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "band";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "tex";
		p.ptype = AttrParam::COLORARRAY;
		this->params.push_back(p);
		p.paramName = "amount";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "offsets";
		p.ptype = AttrParam::FLOATARRAY;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	Blender_woodTexture : public LuxTexture
{
public:
	Blender_woodTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blender_wood";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "noisesize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisebasis2";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "noisebasis";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "bright";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "noisetype";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "turbulence";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "type";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "contrast";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	UvTexture : public LuxTexture
{
public:
	UvTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "uv";
		MFnDependencyNode dn(mObject);
		AttrParam p;
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	Blender_cloudsTexture : public LuxTexture
{
public:
	Blender_cloudsTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "blender_clouds";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "noisesize";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisebasis";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "noisedepth";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "bright";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "contrast";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "noisetype";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	WrinkledTexture : public LuxTexture
{
public:
	WrinkledTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "wrinkled";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "scale";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "octaves";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "roughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
class	MarbleTexture : public LuxTexture
{
public:
	MarbleTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		textureName = "marble";
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "scale";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "octaves";
		p.ptype = AttrParam::INT;
		this->params.push_back(p);
		p.paramName = "coordinates";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
		p.paramName = "roughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "rotate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
		p.paramName = "variation";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "translate";
		p.ptype = AttrParam::VECTOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}


};
//---------------------------- automatically created attributes end --------------------------------------

#endif