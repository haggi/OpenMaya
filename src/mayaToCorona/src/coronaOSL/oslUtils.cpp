#include "oslUtils.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
static Logging logger;

namespace OSL{

	void listProjectionHistory(MObject& mobject, ProjectionUtil& util)
	{
		MFnDependencyNode depFn(mobject);
		MStatus stat;
		MPlugArray pa;
		MObjectArray inputNodes;
		MString dn = depFn.name();
		depFn.getConnections(pa);
		for (uint i = 0; i < pa.length(); i++)
		{
			if (pa[i].isSource())
				continue;
			getConnectedInNodes(pa[i], inputNodes);
		}

		if (mobject.hasFn(MFn::kProjection))
		{
			util.projectionNodes.append(mobject);
			// found a projection node, clear all input plugs
			// and follow only the plugs connected to the "image" plug
			MPlug imagePlug = depFn.findPlug("image");
			MPlugArray imgPlugs;
			imagePlug.connectedTo(imgPlugs, true, false);
			if (imgPlugs.length() == 0)
				return;
			inputNodes.clear();
			for (uint i = 0; i < imgPlugs.length(); i++)
				inputNodes.append(imgPlugs[i].node());
		}

		// no more nodes, this is my leaf node
		if (inputNodes.length() == 0)
		{
			logger.debug(MString("node ") + depFn.name() + " has no incoming connections this is my leaf node.");
			util.leafNodes.append(depFn.object());
			return;
		}
		
		uniqueMObjectArray(inputNodes);

		for (uint i = 0; i < inputNodes.length(); i++)
		{
			MString nodeName = getObjectName(inputNodes[i]);
			logger.debug(MString("Checking node ") + nodeName);
			listProjectionHistory(inputNodes[i], util);
		}
	}

}