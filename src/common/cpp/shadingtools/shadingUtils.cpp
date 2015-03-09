#include "shadingUtils.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>

#include "utilities/logging.h"

static Logging logger;

bool getObjectShadingGroups(MObject& geoObject, MObject& sGroup, int instId)
{

	MPlugArray	connections;
	MFnDependencyNode dependNode(geoObject);
	MPlug plug(geoObject, dependNode.attribute("instObjGroups"));
	int numConnections = plug.numConnectedElements();
	
	plug.elementByLogicalIndex(instId).connectedTo(connections, false, true);
	
	if( connections.length() > 0)
	{
		MObject shadingGroup(connections[0].node());
		if (shadingGroup.apiType() == MFn::kShadingEngine )
		{
			sGroup = shadingGroup; 
			return true;
		}
	}else{
		Logging::debug(MString("Object-instObjGroups has no connection to shading group."));
	}
	return false;
}

bool getObjectShadingGroups(MDagPath& shapeObjectDP, MObject& shadingGroup)
{
	// if obj is a light, simply return the mobject
	if(shapeObjectDP.hasFn(MFn::kLight))
		shadingGroup = shapeObjectDP.node();
	
    if(shapeObjectDP.hasFn(MFn::kMesh))
    {
        // Find the Shading Engines Connected to the SourceNode 
        MFnMesh fnMesh(shapeObjectDP.node());

        // A ShadingGroup will have a MFnSet 
        MObjectArray sets, comps;
        fnMesh.getConnectedSetsAndMembers(shapeObjectDP.instanceNumber(), sets, comps, true);

        // Each set is a Shading Group. Loop through them
		//Logging::debug(MString("Found ") + sets.length()  + " shading groups for mesh object " + fnMesh.name());
        for(unsigned int i = 0; i < sets.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(sets[i]);
			//Logging::debug(MString("SG: ") + fnDepSGNode.name());
			shadingGroup = sets[i];
			return true;
            //cout << fnDepSGNode.name() << endl;
        }
    }

    if(shapeObjectDP.hasFn(MFn::kNurbsSurface)||shapeObjectDP.hasFn(MFn::kParticle)||shapeObjectDP.hasFn(MFn::kNParticle))
    {

        MObject instObjGroupsAttr;
		if( shapeObjectDP.hasFn(MFn::kNurbsSurface))
		{
	        MFnNurbsSurface fnNurbs(shapeObjectDP.node());
			instObjGroupsAttr = fnNurbs.attribute("instObjGroups");
		}
		if( shapeObjectDP.hasFn(MFn::kParticle)||shapeObjectDP.hasFn(MFn::kNParticle))
		{
	        MFnParticleSystem fnPart(shapeObjectDP.node());
			instObjGroupsAttr = fnPart.attribute("instObjGroups");
		}
        MPlug instPlug(shapeObjectDP.node(), instObjGroupsAttr);

        // Get the instance that our node is referring to;
        // In other words get the Plug for instObjGroups[intanceNumber];
        MPlug instPlugElem = instPlug.elementByLogicalIndex(shapeObjectDP.instanceNumber());
        
        // Find the ShadingGroup plugs that we are connected to as Source 
        MPlugArray SGPlugArray;
        instPlugElem.connectedTo(SGPlugArray, false, true);

        // Loop through each ShadingGroup Plug
        for(unsigned int i=0; i < SGPlugArray.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(SGPlugArray[i].node());
            //cout << fnDepSGNode.name() << endl;
			shadingGroup = SGPlugArray[i].node();
			return true;
        }
    }
    return false;
}


//
//	This is the most intersting method because it recognizes per face assignments as well.
//	The per face intArray list will contain a shading group index for every face.
//	Input: DagPath of the shape node
//	Output: perFaceInt array
//			all connected shading groups

bool getObjectShadingGroups(MDagPath& shapeObjectDP, MIntArray& perFaceAssignments, MObjectArray& shadingGroups, bool needsPerFaceInfo=true)
{

	//Logging::debug(MString("getObjectShadingGroups:: obj: ") +  shapeObjectDP.partialPathName() + " type: " + shapeObjectDP.node().apiTypeStr());

	// if obj is a light, simply return the mobject
	if(shapeObjectDP.node().hasFn(MFn::kLight))
	{
		perFaceAssignments.clear();
		shadingGroups.clear();
		shadingGroups.append(shapeObjectDP.node());
		return true;
	}

    if(shapeObjectDP.node().hasFn(MFn::kMesh))
    {
        // Find the Shading Engines Connected to the SourceNode 
        MFnMesh meshFn(shapeObjectDP.node());

		perFaceAssignments.clear();
		shadingGroups.clear();
		MObjectArray comps;

		// this one seems to be extremly much faster if we need no per face informations.
		// e.g. for a sphere with 90000 faces, the non per face method needs 0.05 sec. whereas the 
		// method with per face info needs about 20 sec.
		if (!needsPerFaceInfo)
		{
			meshFn.getConnectedSetsAndMembers(shapeObjectDP.instanceNumber(), shadingGroups, comps, true);
			return true;
		}

		meshFn.getConnectedShaders(shapeObjectDP.instanceNumber(), shadingGroups, perFaceAssignments);

		if (!meshFn.findPlug("displaySmoothMesh").asBool())
			return true;

		MIntArray indices;
		indices = perFaceAssignments;

		int subdivs = 0;
		int multiplier = 0;

		if (meshFn.findPlug("displaySmoothMesh").asBool())
		{
			MMeshSmoothOptions options;
			MStatus status = meshFn.getSmoothMeshDisplayOptions(options);
			if (status)
			{
				if (!meshFn.findPlug("useSmoothPreviewForRender", false, &status).asBool())
				{
					int smoothLevel = meshFn.findPlug("renderSmoothLevel", false, &status).asInt();
					options.setDivisions(smoothLevel);
				}

				subdivs = options.divisions();
				if (subdivs > 0)
					multiplier = static_cast<int> (pow(4.0f, (subdivs - 1)));
			}
		}

		if (multiplier > 0)
			perFaceAssignments.clear();

		for (unsigned int i = 0; i < indices.length(); i++)
		{
			int subdivisions = multiplier * meshFn.polygonVertexCount(i);
			int index = 0 > indices[i] ? 0 : indices[i]; // non assigned has -1, but we want 0
			perFaceAssignments.append(index);

			// simply replicate the index for all subdiv faces
			for (int k = 0; k < subdivisions - 1; k++)
			{
				perFaceAssignments.append(index);
			}
		}
		return true;
    }

    if(shapeObjectDP.node().hasFn(MFn::kNurbsSurface)||shapeObjectDP.hasFn(MFn::kParticle)||shapeObjectDP.hasFn(MFn::kNParticle))
    {

        MObject instObjGroupsAttr;
		if( shapeObjectDP.hasFn(MFn::kNurbsSurface))
		{
	        MFnNurbsSurface fnNurbs(shapeObjectDP.node());
			instObjGroupsAttr = fnNurbs.attribute("instObjGroups");
		}
		if( shapeObjectDP.hasFn(MFn::kParticle)||shapeObjectDP.hasFn(MFn::kNParticle))
		{
	        MFnParticleSystem fnPart(shapeObjectDP.node());
			instObjGroupsAttr = fnPart.attribute("instObjGroups");
		}
        MPlug instPlug(shapeObjectDP.node(), instObjGroupsAttr);

        // Get the instance that our node is referring to;
        // In other words get the Plug for instObjGroups[intanceNumber];
        MPlug instPlugElem = instPlug.elementByLogicalIndex(shapeObjectDP.instanceNumber());
        
        // Find the ShadingGroup plugs that we are connected to as Source 
        MPlugArray SGPlugArray;
		instPlugElem.connectedTo(SGPlugArray, false, true);

		perFaceAssignments.clear();
		shadingGroups.clear();

        // Loop through each ShadingGroup Plug
        for(unsigned int i=0; i < SGPlugArray.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(SGPlugArray[i].node());
			shadingGroups.append(SGPlugArray[i].node());
			return true;
        }
    }
    return false;
}
