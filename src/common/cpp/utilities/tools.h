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

static MString RendererName;
static MString RendererShortCut;
static MString RendererHome;

//degrees * ( M_PI/ 180.0 );
//radians * (180.0/M_PI);
#define DegToRad(x) (M_PI/ 180.0 * x)
#define RadToDeg(x) (180.0/M_PI * x)

void setRendererName(MString rname);

void setRendererShortCutName(MString rname);

void setRendererHome(MString home);

MString getRendererName();

MString getRendererShortCutName();

MString getRendererHome();

bool checkDirectory( MString& path);

MString makeGoodString(MString& oldString);

MString makeGoodHierarchyString(MString& oldString);

MString makeHierarchyString(MString& oldString);

MString getPlugName(MString& longPlugname);

bool IsVisible(MFnDagNode& node);

bool IsVisible(MDagPath& node);

bool IsTemplated(MFnDagNode& node);

bool IsInRenderLayer(MDagPath& dagPath);

bool IsLayerVisible(MDagPath& dagPath);

bool IsPathVisible( MDagPath& dagPath );

bool CheckVisibility( MDagPath& dagPath );

MString matrixToString(MMatrix& matrix);

MObject getOtherSideNode(MString& plugName, MObject& thisObject);

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MString& otherSidePlugName);

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MStringArray& otherSidePlugNames);

MObject getOtherSideNode(MPlug& plug);

bool getOtherSidePlugName(MString& plugName, MObject& thisObject, MString& otherSidePlugName);

bool getConnectedPlugs(MString& plugName, MObject& thisObject, MPlug& inPlug, MPlug& outPlug);

bool getConnectedInPlugs(MObject& thisObject, MPlugArray& inPlugs);

bool getConnectedOutPlugs(MObject& thisObject, MPlugArray& outPlugs);

void getConnectedNodes(MObject& thisObject, MObjectArray& nodeList);

void getConnectedInNodes(MPlug& plug, MObjectArray& nodeList);

void getConnectedInNodes(MString attribute, MObject& thisObject, MObjectArray& nodeList);

bool hasPlug(MObject& thisObject, MString& plugName);

MString getObjectName(MObject& mobject);

MString getObjectName(const MObject& mobject);

//MString getDepNodeTypeName(const MObject& mobject);

MString getDepNodeTypeName(MObject mobject);

MString pointToUnderscore(MString& string);

void writeTMatrixList( std::ofstream& outFile, std::vector<MMatrix>& transformMatrices, bool inverse = false, float scaleFactor = 1.0);

void writeTMatrixList( std::ofstream *outFile, std::vector<MMatrix>& transformMatrices, bool inverse = false, float scaleFactor = 1.0);

MString lightColorAsString(MFnDependencyNode& depFn);

float shadowColorIntensity(MFnDependencyNode& depFn);

void rowToColumn(MMatrix& from, MMatrix& to, bool transRev = false);

MObject objectFromName(MString& name);

void posFromMatrix(MMatrix& matrix, MVector& pos);

void posRotFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot);

void posRotSclFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot, MVector& scl);

bool getConnectedFileTexturePath(MString& plugName, MString& nodeName, MString& value);

MString getConnectedFileTexturePath(MString& plugName, MFnDependencyNode& node);

MObject getConnectedFileTextureObject(MString& plugName, MFnDependencyNode& depFn);

bool findCamera(MDagPath& dagPath);

bool isLightTransform(MDagPath& dagPath);

bool isCameraTransform(MDagPath& dagPath);

void makeUniqueArray( MObjectArray& );

float rnd();

float srnd();

bool isObjectInList(MObject obj, MObjectArray& objectArray);
bool isPlugInList(MObject obj, MPlugArray& plugArray);

void findConnectedNodeTypes(uint nodeId, MObject thisObject, MObjectArray& connecedElements, bool upstream);
void findConnectedNodeTypes(uint nodeId, MObject thisObject, MObjectArray& connecedElements, MPlugArray& completeList, bool upstream);

MString getAttributeNameFromPlug(MPlug& plug);

MObject getConnectedShadingEngine(MObject node);

void getMatrixComponents(MMatrix& matrix, MPoint& pos, MPoint& rot, MPoint& scale);


#endif

