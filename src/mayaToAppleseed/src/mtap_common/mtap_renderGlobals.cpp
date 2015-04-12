#include "renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"

static Logging logger;

void RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void RenderGlobals::setRendererAxis()
{
	this->rendererAxis = YUp;
}



//#include "mtap_renderGlobals.h"
//#include "maya/MSelectionList.h"
//#include "maya/MFnDependencyNode.h"
//#include "utilities/logging.h"
//#include "utilities/attrTools.h"
//
//static Logging logger;
//
//mtap_RenderGlobals::mtap_RenderGlobals()
//{
//	this->setRendererAxis();
//	this->setRendererUnit();
//
//	this->sceneScaleMatrix.setToIdentity();
//	MFnDependencyNode depFn(this->getRenderGlobalsNode());
//	float scaleFactor = getFloatAttr("sceneScale", depFn, 1.0f);
//
//	this->sceneScaleMatrix.matrix[0][0] = scaleFactor;
//	this->sceneScaleMatrix.matrix[1][1] = scaleFactor;
//	this->sceneScaleMatrix.matrix[2][2] = scaleFactor;
//
//	this->defineGlobalConversionMatrix();
//}
//
//void mtap_RenderGlobals::setRendererUnit()
//{
//	this->rendererUnit = MDistance::kMeters;
//}
//
//void mtap_RenderGlobals::setRendererAxis()
//{
//	this->rendererAxis = YUp;
//}
//
//
//MObject mtap_RenderGlobals::getRenderGlobalsNode()
//{
//	MSelectionList appleseedGlobalsList;
//	appleseedGlobalsList.add("appleseedGlobals");
//	MObject node;
//	appleseedGlobalsList.getDependNode(0, node);
//	return node;
//}
