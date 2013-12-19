#include "Corona.h"
#include <maya/MObjectArray.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"

static Logging logger;

void CoronaRenderer::defineLights()
{
	// first get the globals node and serach for a directional light connection
	MObject coronaGlobals = objectFromName(MString("coronaGlobals"));
	MObjectArray nodeList;
	getConnectedInNodes(MString("sunLightConnection"), coronaGlobals, nodeList);
	if( nodeList.length() > 0)
	{
		MObject sunObj = nodeList[0];
		if(sunObj.hasFn(MFn::kTransform))
		{
			// we suppose what's connected here is a dir light transform
			MVector lightDir(0,0,1); // default dir light dir
			MFnDagNode sunDagNode(sunObj);
			lightDir *= sunDagNode.transformationMatrix();
			//sunDagNode.dagPath().extendToShapeDirectlyBelow(0);
		
			Corona::Sun sun;
			sun.active = true;
			sun.dirTo = Corona::Dir(lightDir.x, lightDir.y, lightDir.z).getNormalized();
			sun.color = Corona::Rgb(40000,40000,40000);
			sun.visibleDirect = true;
			sun.visibleReflect = true;
			sun.visibleRefract = true;
			sun.sizeMultiplier = 2.0f;
			this->context.scene->getSun() = sun;
		}
	}

}
