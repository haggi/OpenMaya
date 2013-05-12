#ifndef LUX_SHADER_ATT_H
#define LUX_SHADER_ATT_H

#include "LuxShaderBase.h"

class LuxMaterial : public LuxShadingNode
{
public:
	LuxMaterial(MObject mObject, Instance l): LuxShadingNode(mObject, l)
	{}
};

class	LambertMaterial : public LuxMaterial
{
public:
	LambertMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
	}
	virtual void defineParams()
	{
		MFnDependencyNode dn(this->nodeObject);
		MColor color;
		getColor(MString("color"), dn, color);
		float col[3];
		col[0] = color.r;
		col[1] = color.g;
		col[2] = color.b;
		this->shaderParams->AddRGBColor("Kd", col);
	}

	virtual void defineShader()
	{
		const char *type = "matte";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};


//---------------------------- automatically created attributes start ------------------------------------
class	MixMaterial : public LuxMaterial
{
public:
	MixMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "amount";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "namedmaterial2";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "namedmaterial1";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "mix";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	MattetranslucentMaterial : public LuxMaterial
{
public:
	MattetranslucentMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "sigma";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "energyconserving";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "Kt";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "mattetranslucent";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	GlassMaterial : public LuxMaterial
{
public:
	GlassMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "index";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "architectural";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "Kt";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "cauchyb";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "film";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "filmindex";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "glass";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	Glass2Material : public LuxMaterial
{
public:
	Glass2Material(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "glasss2_dispersion";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "architectural";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "glass2";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	LayeredMaterial : public LuxMaterial
{
public:
	LayeredMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "opacity4";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "opacity1";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "opacity3";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "opacity2";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "namedmaterial4";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "namedmaterial2";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "namedmaterial3";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "namedmaterial1";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "layered";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	RoughglassMaterial : public LuxMaterial
{
public:
	RoughglassMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "index";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "dispersion";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "Kt";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "cauchyb";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "roughglass";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	MetalMaterial : public LuxMaterial
{
public:
	MetalMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "name";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "metal";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	Metal2Material : public LuxMaterial
{
public:
	Metal2Material(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "fresnel";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "metal2";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	ShinymetalMaterial : public LuxMaterial
{
public:
	ShinymetalMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "film";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "filmindex";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "shinymetal";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	ClothMaterial : public LuxMaterial
{
public:
	ClothMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "weft_Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "warp_Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "repeat_u";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "weft_Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "warp_Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "presetname";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "cloth";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	MatteMaterial : public LuxMaterial
{
public:
	MatteMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "sigma";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "matte";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	GlossycoatingMaterial : public LuxMaterial
{
public:
	GlossycoatingMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "index";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ka";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "multibounce";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "d";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "basematerial";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "glossycoating";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	GlossyMaterial : public LuxMaterial
{
public:
	GlossyMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "separable";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		float index = 0.0;
		if( getFloat(MString("index"), dn, index))
		{
			if( index > 0.0f)
			{
				p.paramName = "index";
				p.ptype = AttrParam::FLOAT;
				this->params.push_back(p);
			}
		}
		p.paramName = "Ka";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "multibounce";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "d";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "sigma";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "glossy";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	MirrorMaterial : public LuxMaterial
{
public:
	MirrorMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "Kr";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "film";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "filmindex";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "mirror";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	VelvetMaterial : public LuxMaterial
{
public:
	VelvetMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "p2";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "p3";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "thickness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "p1";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "velvet";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	ScatterMaterial : public LuxMaterial
{
public:
	ScatterMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "g";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "scatter";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	GlossytranslucentMaterial : public LuxMaterial
{
public:
	GlossytranslucentMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "index";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ka";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "onesided";
		p.ptype = AttrParam::BOOL;
		this->params.push_back(p);
		p.paramName = "d";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "backface_d";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "backface_Ka";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "backface_uroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "backface_index";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kt";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "backface_Ks";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "backface_vroughness";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "glossytranslucent";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
class	CarpaintMaterial : public LuxMaterial
{
public:
	CarpaintMaterial(MObject mObject, Instance l) : LuxMaterial(mObject, l)
	{
		MFnDependencyNode dn(mObject);
		AttrParam p;
		p.paramName = "Ks1";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "R1";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Ks3";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "Ks2";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "R3";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "Kd";
		p.ptype = AttrParam::COLOR;
		this->params.push_back(p);
		p.paramName = "R2";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "M1";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "M3";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "M2";
		p.ptype = AttrParam::FLOAT;
		this->params.push_back(p);
		p.paramName = "name";
		p.ptype = AttrParam::ENUMSTRING;
		this->params.push_back(p);
	}
	virtual void defineParams()
	{
		createParamsFromMayaNode();
	}

	virtual void defineShader()
	{
		const char *type = "carpaint";
		this->shaderParams->AddString("type", &type);
		this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
	}

};
//---------------------------- automatically created attributes end ------------------------------------

#endif