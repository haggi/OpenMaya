#ifndef LUX_SHADER_BASE_H
#define LUX_SHADER_BASE_H

#include "Lux.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MColor.h>
#include <maya/MVector.h>

#include "utilities/attrTools.h"
#include "utilities/tools.h"

#include "utilities/logging.h"
static Logging logger;

class LuxShadingNode
{
public:
	AttrParams params;
	MObject nodeObject;
	MString nodeName;
	ParamSet shaderParams;
	Instance lux;
	virtual void defineParams() = 0;
	virtual void defineShader() = 0;

	LuxShadingNode(MObject mObject, Instance l)
	{
		lux = l;
		shaderParams =  CreateParamSet();
		nodeObject = mObject;
		nodeName = getObjectName(mObject);
	};

	void createParamsFromMayaNode()
	{
		MFnDependencyNode depFn(nodeObject);
		for( size_t pId = 0; pId < params.size(); pId++)
		{
			AttrParam p = params[pId];
			logger.debug(MString("trying to get attribute: ") + p.paramName + " from node " + depFn.name());

			if( p.ptype == AttrParam::BOOL )
			{
				bool b = false;
				// in Lux we have no texture mapping for boolean parameters, so no connected node check here.
				if(!getBool(p.paramName, depFn, b))
					logger.debug(MString("Unable to get boolean attribute."));
				else
					shaderParams->AddBool(p.paramName.asChar(), &b, 1);
			}
			if( p.ptype == AttrParam::FLOAT )
			{
				float f = 0.0;
				MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				
				if( objectName != "")
				{
					// if we have a texture connection, the name should always be extended with the appropriate connection type
					// e.g. brick1_outColor
					MString texName = objectName + "_outFloat";
					shaderParams->AddTexture(p.paramName.asChar(), texName.asChar());
				}else{
					if(!getFloat(p.paramName, depFn, f))
						logger.debug(MString("Unable to get float attribute."));
					else
						shaderParams->AddFloat(p.paramName.asChar(), &f, 1);
				}
			}
			if( p.ptype == AttrParam::FLOATARRAY )
			{
				//float f = 0.0;
				//MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				//
				//if( objectName != "")
				//{
				//	// if we have a texture connection, the name should always be extended with the appropriate connection type
				//	// e.g. brick1_outColor
				//	MString texName = objectName + "_outFloat";
				//	shaderParams->AddTexture(p.paramName.asChar(), texName.asChar());
				//}else{
				//	if(!getFloat(p.paramName, depFn, f))
				//		logger.debug(MString("Unable to get float attribute."));
				//	else
				//		shaderParams->AddFloat(p.paramName.asChar(), &f, 1);
				//}
			}
			if( p.ptype == AttrParam::INT )
			{
				int i = 0;
				MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				if( objectName != "")
					shaderParams->AddTexture(p.paramName.asChar(), objectName.asChar());
				else{
					if(!getInt(p.paramName, depFn, i))
						logger.debug(MString("Unable to get int attribute."));
					else
						shaderParams->AddInt(p.paramName.asChar(), &i, 1);
				}
			}
			if( p.ptype == AttrParam::STRING )
			{
				MString s;
				if(!getString(p.paramName, depFn, s))
					logger.debug(MString("Unable to get string attribute."));
				else{
					const char *c = s.asChar();
					shaderParams->AddString(p.paramName.asChar(), &c, 1);
				}
			}
			if( p.ptype == AttrParam::COLOR)
			{
				MColor c;
				MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				if( objectName != "")
				{
					// if we have a texture connection, the name should always be extended with the appropriate connection type
					// e.g. brick1_outColor
					MString texName = objectName + "_outColor";
					shaderParams->AddTexture(p.paramName.asChar(), texName.asChar());
				}else{
					if(!getColor(p.paramName, depFn, c))
						logger.debug(MString("Unable to get color attribute."));
					else{
						float col[3];
						col[0] = c.r;
						col[1] = c.g;
						col[2] = c.b;
						shaderParams->AddRGBColor(p.paramName.asChar(), col, 1);
					}
				}
			}
			if( p.ptype == AttrParam::COLORARRAY)
			{
				//MColor c;
				//MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				//if( objectName != "")
				//{
				//	// if we have a texture connection, the name should always be extended with the appropriate connection type
				//	// e.g. brick1_outColor
				//	MString texName = objectName + "_outColor";
				//	shaderParams->AddTexture(p.paramName.asChar(), texName.asChar());
				//}else{
				//	if(!getColor(p.paramName, depFn, c))
				//		logger.debug(MString("Unable to get color attribute."));
				//	else{
				//		float col[3];
				//		col[0] = c.r;
				//		col[1] = c.g;
				//		col[2] = c.b;
				//		shaderParams->AddRGBColor(p.paramName.asChar(), col, 1);
				//	}
				//}
			}
			if( p.ptype == AttrParam::VECTOR)
			{
				MVector v;
				MString objectName = getObjectName(getOtherSideNode( p.paramName, nodeObject));
				if( objectName != "")
					shaderParams->AddTexture(p.paramName.asChar(), objectName.asChar());
				else{
					if(!getVector(p.paramName, depFn, v))
						logger.debug(MString("Unable to get color attribute."));
					else{
						float vec[3];
						vec[0] = v.x;
						vec[1] = v.y;
						vec[2] = v.z;
						shaderParams->AddVector(p.paramName.asChar(), vec, 1);
					}
				}
			}
			if( p.ptype == AttrParam::ENUMINT)
			{}
			if( p.ptype == AttrParam::ENUMSTRING)
			{
				MString s;
				int id;
				if(!getEnum(p.paramName, depFn, id, s))
					logger.debug(MString("Unable to get enum string attribute."));
				else{
					const char *c = s.asChar();
					shaderParams->AddString(p.paramName.asChar(), &c, 1);
				}
			}
		}
	}

	~LuxShadingNode(){};
};

#endif