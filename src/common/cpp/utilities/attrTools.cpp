#include <maya/MStatus.h>
#include <maya/MDGContext.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPlugArray.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MAngle.h>

#include "attrTools.h"

float getDegree(const char* plugName, MFnDependencyNode& dn)
{
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	return plug.asMAngle().asDegrees();
}

float getRadians(const char* plugName, MFnDependencyNode& dn)
{
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	return plug.asMAngle().asRadians();
}


float getFloatAttr(const char* plugName, MFnDependencyNode& dn, float defaultValue)
{
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return defaultValue;
	return plug.asFloat();
}

bool getFloat(MString& plugName, MFnDependencyNode& dn, float& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asFloat(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getFloat(const char* plugName, MFnDependencyNode& dn, float& value)
{
	return getFloat(MString(plugName), dn, value);
}

bool getFloat2(MString& plugName, MFnDependencyNode& dn, float2& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MString pName0 = plugName + "0";
	MString pName1 = plugName + "1";
	if( plugName == "repeatUV")
	{
		pName0 = "repeatU";
		pName0 = "repeatV";
	}
	MPlug plug = dn.findPlug(pName0, &stat);
	if( !stat )
		return false;
	value[0] = plug.asFloat(ctx, &stat);
	plug = dn.findPlug(pName0, &stat);
	if( !stat )
		return false;
	value[1] = plug.asFloat(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getFloat2(MString& plugName, MFnDependencyNode& dn, MVector& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName + "0", &stat);
	if( !stat )
		return false;
	value.x = plug.asFloat(ctx, &stat);
	plug = dn.findPlug(plugName + "1", &stat);
	if( !stat )
		return false;
	value.y = plug.asFloat(ctx, &stat);
	if(stat)
		return true;
	return result;
}


bool getDouble(MString& plugName, MFnDependencyNode& dn, double& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asDouble(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getString(MString& plugName, MFnDependencyNode& dn, MString& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asString(ctx, &stat);
	if(stat)
		return true;
	return result;
}

MString getString(const char *plugName, MFnDependencyNode& dn)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return "";
	return plug.asString(ctx, &stat);
}

MString getStringAttr(MString plugName, MFnDependencyNode& dn, MString default)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	MPlug plug = dn.findPlug(plugName, &stat);
	if (!stat)
		return default;
	return plug.asString(ctx, &stat);
}

bool getLong(MString& plugName, MFnDependencyNode& dn, long& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asInt(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getInt(MString& plugName, MFnDependencyNode& dn, int& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asInt(ctx, &stat);
	if(stat)
		return true;
	return result;
}
bool getInt(const char *plugName, MFnDependencyNode& dn, int& value)
{
	return getInt(MString(plugName), dn, value);
}

int getIntAttr(const char *plugName, MFnDependencyNode& dn, int defaultValue)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return defaultValue;
	return plug.asInt(ctx, &stat);
}

bool getUInt(const char *plugName, MFnDependencyNode& dn, uint& value)
{
	int val = value;
	bool ret = getInt(MString(plugName), dn, val);
	value = val;
	return ret;
}

bool getBool(MString& plugName, MFnDependencyNode& dn, bool& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asBool(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getBoolAttr(const char *plugName, MFnDependencyNode& dn, bool defaultValue)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	return plug.asBool(ctx, &stat);
}

bool getBool(const char *plugName, MFnDependencyNode& dn, bool& value)
{
	return getBool(MString(plugName), dn, value);
}

bool getEnum(MString& plugName, MFnDependencyNode& dn, int& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	value = plug.asShort(ctx, &stat);
	if(stat)
		return true;
	return result;
}

int getEnumInt(MString plugName, MFnDependencyNode& dn)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	MPlug plug = dn.findPlug(plugName, &stat);
	if (!stat)
		return -1;
	int value = plug.asShort(ctx, &stat);
	if (!stat)
		return -1;
	return value;
}

int getEnumInt(MPlug plug)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	int value = plug.asShort(ctx, &stat);
	if (!stat)
		return -1;
	return value;
}

MString getEnumString(MString plugName, MFnDependencyNode& dn)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if (!stat)
		return "";
	int id = plug.asShort(ctx, &stat);
	if (!stat)
		return "";
	MFnEnumAttribute eAttr(plug.attribute(&stat));
	if (!stat)
		return "";
	MString value = eAttr.fieldName(id, &stat);
	if (!stat)
		return "";
	return value;
}
bool getEnum(MString& plugName, MFnDependencyNode& dn, int& id, MString& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	id = plug.asShort(ctx, &stat);
	if(!stat)
		return false;
	MFnEnumAttribute eAttr(plug.attribute(&stat));
	if(!stat)
		return false;
	value = eAttr.fieldName(id, &stat);
	if(!stat)
		return false;
	return true;
}

bool getEnum(const char *plugName, MFnDependencyNode& dn, int& id, MString& value)
{
	return getEnum(MString(plugName), dn, id, value);
}

bool getInt2(MString& plugName, MFnDependencyNode& dn, int2& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName + "0", &stat);
	if( !stat )
		return false;
	value[0] = plug.asInt(ctx, &stat);
	plug = dn.findPlug(plugName + "1", &stat);
	if( !stat )
		return false;
	value[1] = plug.asInt(ctx, &stat);
	if(stat)
		return true;
	return result;
}

bool getColor(MString& plugName, MFnDependencyNode& dn, MColor& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	float r, g, b;

	std::string pn = plugName.asChar();
	int index = -1;
	if (pn[pn.size() - 1] == ']')
	{
		size_t found = pn.rfind('[');
		if (found != std::string::npos)
		{
			MString indexString = pn.substr(found + 1, pn.size() - 3).c_str();
			pn = pn.substr(0, found);
			index = indexString.asInt();
		}
	}
	
	// I suppose the attribute is a color and has 3 children
	MPlug plug = dn.findPlug(pn.c_str(), stat);
	if (index >= 0)
		plug = plug[index];
	if (!stat)
		return result;
	if (plug.numChildren() < 3)
		return result;
	r = plug.child(0).asFloat(ctx, &stat);
	if (!stat)
		return result;
	g = plug.child(1).asFloat(ctx, &stat);
	if (!stat)
		return result;
	b = plug.child(2).asFloat(ctx, &stat);
	if (!stat)
		return result;
	value.r = r;
	value.g = g;
	value.b = b;
	return true;
}

bool getColor(const char *plugName, MFnDependencyNode& dn, MColor& value)
{
	return getColor(MString(plugName), dn, value);
}

bool getColor(MString& plugName, MFnDependencyNode& dn, MString& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	float r, g, b;
	MPlug plug = dn.findPlug(plugName + "R", &stat);
	if( !stat ) return false;
	r = plug.asFloat(ctx, &stat);
	plug = dn.findPlug(plugName + "G", &stat);
	if( !stat ) return false;
	g = plug.asFloat(ctx, &stat);
	plug = dn.findPlug(plugName + "B", &stat);
	if( !stat ) return false;
	b = plug.asFloat(ctx, &stat);
	value = MString("") + r + " " + g + " " + b;
	result = true;
	return result;
}

bool getColor(const char *plugName, MFnDependencyNode& dn, float *value)
{
	MColor c(1,0,1);
	bool result = getColor(MString(plugName), dn, c);
	if( result )
	{
		value[0] = c.r;
		value[1] = c.g;
		value[2] = c.b;
	}
	return result;
}

MColor getColorAttr(MPlug plug)
{
	MColor c;
	if (!plug.isCompound())
		return c;
	if (plug.numChildren() != 3)
		return c;
	c.r = plug.child(0).asFloat();
	c.g = plug.child(1).asFloat();
	c.b = plug.child(2).asFloat();
	return c;
}

MColor getColorAttr(const char *plugName, MFnDependencyNode& dn)
{
	MColor c(1,0,1);
	getColor(MString(plugName), dn, c);
	return c;
}

MVector getVectorAttr(const char *plugName, MFnDependencyNode& dn)
{
	MVector c(0, 0, 0);
	getVector(MString(plugName), dn, c);
	return c;
}

MVector getVectorAttr(MPlug plug)
{
	MVector c;
	if (!plug.isCompound())
		return c;
	if (plug.numChildren() != 3)
		return c;
	c.x = plug.child(0).asFloat();
	c.y = plug.child(1).asFloat();
	c.z = plug.child(2).asFloat();
	return c;
}

bool getColor(const char *plugName, MFnDependencyNode& dn, MString& value)
{
	return getColor(MString(plugName), dn, value);
}

bool getVector(MString& plugName, MFnDependencyNode& dn, MVector& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	MPlug Xplug = dn.findPlug(plugName + "X", &stat);
	MPlug Yplug = dn.findPlug(plugName + "Y", &stat);
	MPlug Zplug = dn.findPlug(plugName + "Z", &stat);
	if( !stat )
		return false;
	value.x = Xplug.asDouble(ctx, &stat);
	value.y = Yplug.asDouble(ctx, &stat);
	value.z = Zplug.asDouble(ctx, &stat);
	if( !stat )
		return false;
	return true;
}

bool getPoint(MString& plugName, MFnDependencyNode& dn, MPoint& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	MPlug Xplug = dn.findPlug(plugName + "X", &stat);
	MPlug Yplug = dn.findPlug(plugName + "Y", &stat);
	MPlug Zplug = dn.findPlug(plugName + "Z", &stat);
	if( !stat )
		return false;
	value.x = Xplug.asDouble(ctx, &stat);
	value.y = Yplug.asDouble(ctx, &stat);
	value.z = Zplug.asDouble(ctx, &stat);
	if( !stat )
		return false;
	return true;
}

bool getPoint(MString& plugName, MFnDependencyNode& dn, MVector& value)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	MPlug Xplug = dn.findPlug(plugName + "X", &stat);
	MPlug Yplug = dn.findPlug(plugName + "Y", &stat);
	MPlug Zplug = dn.findPlug(plugName + "Z", &stat);
	if( !stat )
		return false;
	value.x = Xplug.asDouble(ctx, &stat);
	value.y = Yplug.asDouble(ctx, &stat);
	value.z = Zplug.asDouble(ctx, &stat);
	if( !stat )
		return false;
	return true;
}

bool getMsgObj(const char *plugName, MFnDependencyNode& dn, MObject& value)
{
	value = MObject::kNullObj;
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;
	bool result = false;
	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	if(!plug.isConnected())
		return false;
	MPlugArray inConnections;
	plug.connectedTo(inConnections, true, false, &stat);
	if( !stat )
		return false;
	value = inConnections[0].node();
	return true;
}

MMatrix getMatrix(const char *plugName, MFnDependencyNode& dn)
{
	MMatrix m;
	m.setToIdentity();
	MStatus stat;
	MPlug p = dn.findPlug(plugName, &stat);
	if (stat)
	{
		MObject matrixObject;
		p.getValue(matrixObject);
		MFnMatrixData fnMat(matrixObject);
		MMatrix mat = fnMat.matrix();
		m = mat;
	}
	return m;
}

ATTR_TYPE getPlugAttrType(MPlug plug)
{
	MStatus stat = MS::kSuccess;

	MPlug p = plug;
	while (p.isChild())
		p = p.parent();
	plug = p;
	MObject attObj = plug.attribute(&stat);
	MFnAttribute att(attObj);

	if (!stat)
		return ATTR_TYPE::ATTR_TYPE_NONE;
	MString attName = att.name();

	// all 3 child attributes are vectors for osl
	if (p.numChildren() == 3)
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "input1")
	{
		if (plug.node().hasFn(MFn::kMultiplyDivide))
			return ATTR_TYPE::ATTR_TYPE_VECTOR;
	}
	if (attName == "input2")
	{
		if (plug.node().hasFn(MFn::kMultiplyDivide))
			return ATTR_TYPE::ATTR_TYPE_VECTOR;
	}

	if (attName == "output")
	{
		if (plug.node().hasFn(MFn::kMultiplyDivide))
			return ATTR_TYPE::ATTR_TYPE_VECTOR;
	}

	if (attName == "outValue")
	{
		if (plug.node().hasFn(MFn::kGammaCorrect))
			return ATTR_TYPE::ATTR_TYPE_COLOR;
	}
	if (attName == "outDPdu")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "outDPdv")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "outColor")
		return ATTR_TYPE::ATTR_TYPE_COLOR;

	if (attName == "uvCoord")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "outUV")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "color")
		return ATTR_TYPE::ATTR_TYPE_COLOR;

	if (attName == "normalCamera")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "pointWorld")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (attName == "rayDirection")
		return ATTR_TYPE::ATTR_TYPE_VECTOR;

	if (att.isUsedAsColor())
		return ATTR_TYPE::ATTR_TYPE_COLOR;
	if (attObj.apiType() == MFn::kNumericAttribute)
	{
		MFnNumericAttribute na(attObj, &stat);
		if (!stat)
			return ATTR_TYPE::ATTR_TYPE_NONE;
		if (na.unitType() == MFnNumericData::Type::kFloat)
			return ATTR_TYPE::ATTR_TYPE_FLOAT;
	}
	if (attObj.apiType() == MFn::kAttribute3Float)
	{
		return ATTR_TYPE::ATTR_TYPE_VECTOR;
	}
	return ATTR_TYPE::ATTR_TYPE_NONE;
}

ATTR_TYPE getPlugAttrType(const char *plugName, MFnDependencyNode& dn)
{
	MDGContext ctx = MDGContext::fsNormal;
	MStatus stat = MS::kSuccess;

	std::string pn = plugName;
	if (pn[pn.length() - 1] == ']')
	{
		size_t found = pn.rfind('[');
		if (found != std::string::npos)
		{
			pn = pn.substr(0, found);
		}
	}
	plugName = pn.c_str();

	MPlug plug = dn.findPlug(plugName, &stat);
	if( !stat )
		return ATTR_TYPE::ATTR_TYPE_NONE;

	MObject attObj = plug.attribute(&stat);
	MFnAttribute att(attObj);
	if( !stat )
		return ATTR_TYPE::ATTR_TYPE_NONE;

	if(att.isUsedAsColor())
		return ATTR_TYPE::ATTR_TYPE_COLOR;
	if( attObj.apiType() == MFn::kNumericAttribute)
	{
		MFnNumericAttribute na(attObj, &stat);
		if( !stat )
			return ATTR_TYPE::ATTR_TYPE_NONE;
		if( na.unitType() == MFnNumericData::Type::kFloat )
			return ATTR_TYPE::ATTR_TYPE_FLOAT;
	}
	return ATTR_TYPE::ATTR_TYPE_NONE;
}

int getChildId(MPlug& plug)
{
	if (!plug.isChild())
		return -1;

	MPlug parentPlug = plug.parent();
	for (uint chId = 0; chId < parentPlug.numChildren(); chId++)
	{
		if (parentPlug.child(chId) == plug)
			return chId;
	}
	return -1;
}

