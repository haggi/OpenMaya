#ifndef MTM_TOOLS
#define MTM_TOOLS

#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MObjectArray.h>
#include <direct.h>
#include <io.h>
#include <fstream>
#include <math.h>

#include <vector>

//degrees * ( M_PI/ 180.0 );
//radians * (180.0/M_PI);
#define DegToRad(x) (M_PI/ 180.0 * x)
#define RadToDeg(x) (180.0/M_PI * x)

bool checkDirectory( MString& path);

MString makeGoodString(MString& oldString);

MString makeGoodHierarchyString(MString& oldString);

MString makeHierarchyString(MString& oldString);

MString getPlugName(MString& longPlugname);

bool IsVisible(MFnDagNode& node);

bool IsTemplated(MFnDagNode& node);

bool IsInRenderLayer(MDagPath& dagPath);

bool IsPathVisible( MDagPath& dagPath );

bool CheckVisibility( MDagPath& dagPath );

MString matrixToString(MMatrix& matrix);

MObject getOtherSideNode(MString& plugName, MObject& thisObject);

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MString& otherSidePlugName);

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MStringArray& otherSidePlugNames);

MObject getOtherSideNode(MPlug& plug);

bool getOtherSidePlugName(MString& plugName, MObject& thisObject, MString& otherSidePlugName);

bool getConnectedPlugs(MString& plugName, MObject& thisObject, MPlug& inPlug, MPlug& outPlug);

void getConnectedNodes(MObject& thisObject, MObjectArray& nodeList);

MString getObjectName(MObject& mobject);

MString pointToUnderscore(MString& string);

void writeTMatrixList( std::ofstream& outFile, std::vector<MMatrix>& transformMatrices, bool inverse = false, float scaleFactor = 1.0);

void writeTMatrixList( std::ofstream *outFile, std::vector<MMatrix>& transformMatrices, bool inverse = false, float scaleFactor = 1.0);

MString lightColorAsString(MFnDependencyNode& depFn);

float shadowColorIntensity(MFnDependencyNode& depFn);

void rowToColumn(MMatrix& from, MMatrix& to);

MObject objectFromName(MString& name);

void posFromMatrix(MMatrix& matrix, MVector& pos);

void posRotFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot);

void posRotSclFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot, MVector& scl);

bool getConnectedFileTexturePath(MString& plugName, MString& nodeName, MString& value);

#endif

