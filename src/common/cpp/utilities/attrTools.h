#ifndef MTM_ATTR_TOOLS_H
#define MTM_ATTR_TOOLS_H

#include <maya/MPlug.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MString.h>
#include <maya/MColor.h>
#include <maya/MVector.h>
#include <maya/Mpoint.h>

enum ATTR_TYPE
{
	ATTR_TYPE_NONE = 0,
	ATTR_TYPE_COLOR = 1,
	ATTR_TYPE_FLOAT = 2,
	ATTR_TYPE_VECTOR = 3
};

int getChildId(MPlug& plug);

float getDegree(const char* plugName, MFnDependencyNode& dn);

float getRadians(const char* plugName, MFnDependencyNode& dn);

float getFloatAttr(const char* plugName, MFnDependencyNode& dn, const float defaultValue);

bool getFloat(MString& plugName, MFnDependencyNode& dn, float& value);

bool getFloat(const char* plugName, MFnDependencyNode& dn, float& value);

bool getFloat2(MString& plugName, MFnDependencyNode& dn, float2& value);

bool getFloat2(MString& plugName, MFnDependencyNode& dn, MVector& value);

bool getDouble(MString& plugName, MFnDependencyNode& dn, double& value);

bool getString(MString& plugName, MFnDependencyNode& dn, MString& value);

MString getString(const char *plugName, MFnDependencyNode& dn);

MString getStringAttr(MString plugName, MFnDependencyNode& dn, MString default);

bool getInt(MString& plugName, MFnDependencyNode& dn, int& value);

bool getInt(const char *plugName, MFnDependencyNode& dn, int& value);

int getIntAttr(const char *plugName, MFnDependencyNode& dn, int defaultValue);

bool getUInt(const char *plugName, MFnDependencyNode& dn, uint& value);

bool getBool(MString& plugName, MFnDependencyNode& dn, bool& value);

bool getBool(const char *plugName, MFnDependencyNode& dn, bool& value);

bool getBoolAttr(const char *plugName, MFnDependencyNode& dn, bool defaultValue);

bool getEnum(MString& plugName, MFnDependencyNode& dn, int& value);

bool getEnum(MString& plugName, MFnDependencyNode& dn, int& id, MString& value);

bool getEnum(const char *plugName, MFnDependencyNode& dn, int& id, MString& value);

MString getEnumString(MString plugName, MFnDependencyNode& dn);

int getEnumInt(MString plugName, MFnDependencyNode& dn);

int getEnumInt(MPlug plug);

bool getInt2(MString& plugName, MFnDependencyNode& dn, int2& value);

bool getLong(MString& plugName, MFnDependencyNode& dn, long& value);

bool getColor(MString& plugName, MFnDependencyNode& dn, MColor& value);

bool getColor(MString& plugName, MFnDependencyNode& dn, MString& value);

bool getColor(const char *plugName, MFnDependencyNode& dn, MColor& value);

bool getColor(const char *plugName, MFnDependencyNode& dn, MString& value);

bool getColor(const char *plugName, MFnDependencyNode& dn, float *value);

MColor getColorAttr(const char *plugName, MFnDependencyNode& dn);

MColor getColorAttr(MPlug plug);

bool getVector(MString& plugName, MFnDependencyNode& dn, MVector& value);

MVector getVectorAttr(const char *plugName, MFnDependencyNode& dn);

MVector getVectorAttr(MPlug plug);

bool getPoint(MString& plugName, MFnDependencyNode& dn, MPoint& value);

bool getPoint(MString& plugName, MFnDependencyNode& dn, MVector& value);

bool getMsgObj(const char *plugName, MFnDependencyNode& dn, MObject& value);

MMatrix getMatrix(const char *plugName, MFnDependencyNode& dn);

ATTR_TYPE getPlugAttrType(const char *plugName, MFnDependencyNode& dn);

ATTR_TYPE getPlugAttrType(MPlug plug);

#endif
