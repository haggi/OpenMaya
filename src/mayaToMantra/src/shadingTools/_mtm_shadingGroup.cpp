#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsSurface.h>
#include "shadingTools/mtm_shadingGroup.h"
#include "shadingTools/mtm_readShaderDefs.h"
#include "utilities/tools.h"
#include "utilities/logging.h"
#include "mtm_globals.h"
#include <fstream>
#include "utilities/pystring.h"

static Logging logger;

static std::vector<MString> donePlugNames;

bool getShadingGroup(MObject& shadingGroupObject, ShadingGroup& sg)
{
	MString findName = getObjectName(shadingGroupObject);
	//logger.debug(MString("-->getShadingGroup:: checking ") + mtm_ShadingGroups.size() + " shadinggroups");
	for( int i = 0; i < mtm_ShadingGroups.size(); i++)
	{
		//logger.debug(MString("-->getShadingGroup:: Compare shading group ") + findName + " with " + mtm_ShadingGroups[i].name);
		if( mtm_ShadingGroups[i].name == findName)
		{
			//logger.debug(MString("-->getShadingGroup:: Equal found shading group."));
			sg = mtm_ShadingGroups[i];
			return true;
		}
	}
	return false;
}


bool exportShadingGroups()
{
	mtm_ShadingGroups.clear();
	for (unsigned int i=0; i < mtm_MayaSGList.size(); i++)
	{
		mtm_ShadingGroups.push_back(ShadingGroup(mtm_MayaSGList[i]));
	}
	for (unsigned int i=0; i < mtm_ShadingGroups.size(); i++)
	{
		logger.debug(MString("exportShadingGroups: ") + mtm_ShadingGroups[i].name);
		mtm_ShadingGroups[i].exportVfl();
		mtm_ShadingGroups[i].compileShader();
	}
	return true;
}

void clearObjectShadingGroups()
{
	mtm_MayaSGList.clear();
}

void clearShaderNodes()
{
	mtm_ShadingGroups.clear();
}
// This function checks the Shading groups connected to a geometry shape
// and puts them into a list. This list will be later exported and compiled.
// This way only the really connected shaders will be exported
void getObjectShadingGroups(MObject& geoObject)
{
	logger.debug(MString("getConnectedSgroups for obj ") + getObjectName(geoObject));
	MPlugArray	connections;
	MFnDependencyNode dependNode(geoObject);

	MPlug plug(geoObject, dependNode.attribute("instObjGroups"));
	int numConnections = plug.numConnectedElements();
	
	for( int connId = 0; connId < numConnections; connId++)
	{
		plug.elementByLogicalIndex(connId).connectedTo(connections, false, true);
	
		for (unsigned int k=0; k<connections.length(); ++k)
		{
			MObject shadingGroup(connections[k].node());
			if (shadingGroup.apiType() == MFn::kShadingEngine )
			{
				bool found = false;
				for (unsigned int i=0; i < mtm_MayaSGList.size(); i++)
				{
					if( shadingGroup == mtm_MayaSGList[i] )
						found = true;
				}
				if(!found)
					mtm_MayaSGList.push_back(shadingGroup);
			}
		}
	}
}

void getObjectShadingGroups(MDagPath& shapeObjectDP)
{
    if(shapeObjectDP.hasFn(MFn::kMesh))
    {
        // Find the Shading Engines Connected to the SourceNode 
        MFnMesh fnMesh(shapeObjectDP.node());
		logger.debug(MString("getConnectedSgroups for mesh ") + fnMesh.name());

        // A ShadingGroup will have a MFnSet 
        MObjectArray sets, comps;
        fnMesh.getConnectedSetsAndMembers(shapeObjectDP.instanceNumber(), sets, comps, true);

        // Each set is a Shading Group. Loop through them
        for(unsigned int i = 0; i < sets.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(sets[i]);
			logger.debug(MString("Found Shading group: ") + fnDepSGNode.name());
			bool found = false;
			for (unsigned int k=0; k < mtm_MayaSGList.size(); k++)
			{
				if( sets[i] == mtm_MayaSGList[k] )
					found = true;
			}
			if(!found)
				mtm_MayaSGList.push_back(sets[i]);
			break;
        }
    }

    if(shapeObjectDP.hasFn(MFn::kNurbsSurface))
    {
        MFnNurbsSurface fnNurbs(shapeObjectDP.node());

        MObject instObjGroupsAttr = fnNurbs.attribute("instObjGroups");

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
			logger.debug(MString("Found Shading group: ") + fnDepSGNode.name());
			bool found = false;
			for (unsigned int k=0; k < mtm_MayaSGList.size(); k++)
			{
				if( SGPlugArray[i].node() == mtm_MayaSGList[k] )
					found = true;
			}
			if(!found)
				mtm_MayaSGList.push_back(SGPlugArray[i].node());
			break;
        }
    }

}

bool getObjectShadingGroups(MObject& geoObject, MObject& sGroup, int instId)
{
	logger.debug(MString("getObjectShadingGroups for obj: ") + getObjectName(geoObject) + " instance id " + instId);
	MPlugArray	connections;
	MFnDependencyNode dependNode(geoObject);
	logger.debug(MString(" shading object ") + dependNode.name());
	MPlug plug(geoObject, dependNode.attribute("instObjGroups"));
	int numConnections = plug.numConnectedElements();
	
	plug.elementByLogicalIndex(instId).connectedTo(connections, false, true);
	
	if( connections.length() > 0)
	{
		MObject shadingGroup(connections[0].node());
		if (shadingGroup.apiType() == MFn::kShadingEngine )
		{
			logger.debug(MString("Found shading group for object ")  + dependNode.name() + " == " + getObjectName(shadingGroup));
			sGroup = shadingGroup; 
			return true;
		}
	}else{
		logger.debug(MString("Object-instObjGroups has no connection to shading group."));
	}
	return false;
}

bool getObjectShadingGroups(MDagPath& shapeObjectDP, MObject& shadingGroup)
{
    if(shapeObjectDP.hasFn(MFn::kMesh))
    {
        // Find the Shading Engines Connected to the SourceNode 
        MFnMesh fnMesh(shapeObjectDP.node());

        // A ShadingGroup will have a MFnSet 
        MObjectArray sets, comps;
        fnMesh.getConnectedSetsAndMembers(shapeObjectDP.instanceNumber(), sets, comps, true);

        // Each set is a Shading Group. Loop through them
		logger.debug(MString("Found ") + sets.length()  + " shading groups for mesh object " + fnMesh.name());
        for(unsigned int i = 0; i < sets.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(sets[i]);
			logger.debug(MString("SG: ") + fnDepSGNode.name());
			shadingGroup = sets[i];
			return true;
            //cout << fnDepSGNode.name() << endl;
        }
    }

	if(shapeObjectDP.hasFn(MFn::kParticle)||shapeObjectDP.hasFn(MFn::kNParticle))
    {
        // Find the Shading Engines Connected to the SourceNode 
        MFnParticle fnParticle(shapeObjectDP.node());

        // A ShadingGroup will have a MFnSet 
        MObjectArray sets, comps;
        fnMesh.getConnectedSetsAndMembers(shapeObjectDP.instanceNumber(), sets, comps, true);

        // Each set is a Shading Group. Loop through them
		logger.debug(MString("Found ") + sets.length()  + " shading groups for mesh object " + fnParticle.name());
        for(unsigned int i = 0; i < sets.length(); ++i)
        {
            MFnDependencyNode fnDepSGNode(sets[i]);
			logger.debug(MString("SG: ") + fnDepSGNode.name());
			shadingGroup = sets[i];
			return true;
            //cout << fnDepSGNode.name() << endl;
        }
    }

    if(shapeObjectDP.hasFn(MFn::kNurbsSurface))
    {
        MFnNurbsSurface fnNurbs(shapeObjectDP.node());

        MObject instObjGroupsAttr = fnNurbs.attribute("instObjGroups");

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

ShadingGroup::ShadingGroup(MObject& sGroup)
{
	this->name = getObjectName(sGroup);
	logger.debug(MString("Creating shading group for sGroup ") + this->name );

	this->displacementShader.shaderObject = MObject::kNullObj;
	this->volumeShader.shaderObject = MObject::kNullObj;
	this->surfaceShader.shaderObject = MObject::kNullObj;

	this->mobject = sGroup;
	// first get the base connections
	MObject surface = getOtherSideNode(MString("surfaceShader"), this->mobject);
	MObject volume = getOtherSideNode(MString("volumeShader"), this->mobject);
	MObject displace = getOtherSideNode(MString("displacementShader"), this->mobject);

	MString surfaceType = surface.apiTypeStr();
	if(isNodeSupported(surfaceType, this->surfaceShader, surface))
	{
		donePlugNames.clear();
		logger.debug(MString("ShadingGroup::ShadingGroup surface type ") + surfaceType + " supported doing recursiveRead");
		this->recursiveReadConnections(this->surfaceShader, this->surfaceNodeList, this->surfaceExternalPlugs);
	}else{
		logger.debug(MString("ShadingGroup::ShadingGroup surface type ") + surfaceType + " is NOT supported. Skipping.");
	}
	
	MString displacementType = displace.apiTypeStr();
	if(isNodeSupported(displacementType, this->displacementShader, displace))
	{
		donePlugNames.clear();
		logger.debug(MString("\n---> ShadingGroup::ShadingGroup displace type ") + displacementType + " supported doing recursiveRead");
		this->recursiveReadConnections(this->displacementShader, this->displacementNodeList, this->displacementExternalPlugs);
		logger.debug(MString("\n---> ShadingGroup::ShadingGroup displace type recusiveRead done.\n\n"));
	}else{
		logger.debug(MString("ShadingGroup::ShadingGroup displace type ") + displacementType + " is NOT supported. Skipping.");
	}

	MString volumeType = volume.apiTypeStr();
	if(isNodeSupported(volumeType, this->volumeShader, volume))
	{
		donePlugNames.clear();
		logger.debug(MString("ShadingGroup::ShadingGroup volume type ") + volumeType + " supported doing recursiveRead");
		this->recursiveReadConnections(this->volumeShader, this->volumeNodeList, this->volumeExternalPlugs);
	}else{
		logger.debug(MString("ShadingGroup::ShadingGroup volume type ") + volumeType + " is NOT supported. Skipping.");
	}
}

ShadingGroup::ShadingGroup()
{}

ShadingGroup::~ShadingGroup(){}

// reading the network is done the following way:
//		- the current shading node is evaluated and checked for input connections
//		- the connection is set in the current node and the connected node is now checked for its connections
//		- if the connected node is not supported, the plug is added to the external plugs, e.g. if an animcurve is
//		  connected. This is not supported, but the connection can be evaluated every frame so that the shader call is correct

//bool ShadingGroup::recursiveReadConnections(ShaderNode& sn)
bool ShadingGroup::recursiveReadConnections(ShaderNode& sn, std::vector<ShaderNode>& nodeList, std::vector<ShaderPlug>& externalPlugs)
{
//	bool result = true;
	MString info;
	MFnDependencyNode depFn(sn.shaderObject);
	logger.debug(MString("recursiveReadConnections::Check hypershade node ") + depFn.name());

	// shader node is filled with current plug values. Now I have to find out
	// if the node is connected to other nodes. First I need the connected plugs
	MPlugArray connectedPlugs;
	depFn.getConnections(connectedPlugs);
	int numConnections = connectedPlugs.length();
	logger.debug(MString("Node has ") + numConnections + " in or out connections.");
	
	MString dna = depFn.name();

	for( int connId = 0; connId < numConnections; connId++)
	{
		// check for incoming connections only. Outgoing connections are not relevant
		MPlug plug = connectedPlugs[connId];
		logger.debug(MString("Check connection: ") + plug.name());
		// an plug can be source AND destination at the same time, like the displacement attribute of a displacementShader
		if( plug.isSource() && !plug.isDestination())
		{
			logger.debug(MString("Connection: ") + plug.name() + " is source and NO destination, need destination --> skipping");
			continue;
		}
		logger.debug(MString("connection ") + plug.name() + " is destination plug");
		MString shortPlugName = plug.partialName(false, false, false, false, false, true);

		// check if the plug short name is supported in the current shading input nodes
		int plugId = -1, subId = -1;
		ShaderPlug thisPlug;
		if( isInPlugSupported(shortPlugName, sn, thisPlug))
		{
			logger.debug(MString("InPlug: ") + shortPlugName + " is supported.");
		}else{
			logger.debug(MString("InPlug: ") + shortPlugName + " is NOT supported.");
			continue;
		}

		MString otherSidePlugName;
		MObject plugObject = getOtherSideNode(shortPlugName, sn.shaderObject, otherSidePlugName);
		if( plugObject == MObject::kNullObj )
		{
			logger.debug(MString("Unable to get other side node.") + otherSidePlugName);
			continue;
		}

		// check if the node on the other side is supported, if not then save the connction in the external plug list
		// this list will be used to define and call the shading group shader
		ShaderNode otherSideSn;
		if( !isNodeSupported(plugObject.apiTypeStr(), otherSideSn, plugObject))
		{
			logger.info(MString("-->OtherSideNode: ") + getObjectName(plugObject) + " is not supported by plug " + shortPlugName);
			ShaderPlug exPlug = thisPlug;

			if( thisPlug.parentPlugName.length() > 0)
			{
				ShaderPlug parentPlug;
				if(!mtm_findShaderPlugByName( thisPlug.parentPlugName, sn, exPlug ))
				{
					logger.info(MString("-->Plug ") + thisPlug.plugName + " is subplug but parent plug not found");
				}else{
					logger.info(MString("-->Adding plug from subplug ") + thisPlug.plugName + " parentPlug " + parentPlug.plugName);
				}
			}

			bool found = false;
			for( int epId = 0; epId < externalPlugs.size(); epId++)
			{
				//logger.info(MString("-->Comparing external plug ") + this->externalPlugs[epId].plugName + " with " + exPlug.plugName);
				if( externalPlugs[epId].plugName == exPlug.plugName )
				{
					found = true;
					break;
				}
			}
			if( !found )
			{
				//logger.info(MString("-->Adding Plug ") + exPlug.plugName);
				externalPlugs.push_back(exPlug);
				//if( thisPlug.parentPlugName.length() > 0)
				//{
				//	ShaderPlug parentPlug;
				//	if(!mtm_findShaderPlugByName( thisPlug.parentPlugName, sn, parentPlug ))
				//	{
				//		logger.info(MString("-->Plug ") + thisPlug.plugName + " is subplug but parent plug not found");
				//	}else{
				//		logger.info(MString("-->Adding plug from subplug ") + thisPlug.plugName + " parentPlug " + parentPlug.plugName);
				//		this->externalPlugs.push_back(parentPlug);
				//	}
				//}else{
				//	logger.info(MString("-->Adding plug ") + thisPlug.plugName);
				//	this->externalPlugs.push_back(thisPlug);
				//}
			}
			continue;
		}
		
		// Okay, other side shader node is supported. Lets see if the outPlug of the other side node is supported too
		ShaderPlug otherSidePlug;
		if(isOutPlugSupported(otherSidePlugName, otherSideSn, otherSidePlug))
		{
			// save the connection into the current plug
			logger.info(MString("recread: otherside: ") + otherSideSn.mayaName + " with plug " + otherSidePlug.plugName + " is supported and connected with this plug " + thisPlug.plugName);
			sn.setConnectedPlugs(otherSideSn, thisPlug, otherSidePlug);
			for( uint tsti = 0; tsti < sn.inPlugs.size(); tsti++)
			{
				logger.debug(MString("test shaderplug ") + sn.inPlugs[tsti].plugName);
				if(  sn.inPlugs[tsti].connected )
					logger.debug(MString("test shaderplug ") + sn.inPlugs[tsti].plugName + " connected with " + sn.inPlugs[tsti].connection);

			}
		}else{
			//MGlobal::displayInfo("OtherSideNode: " + getObjectName(plugObject) + " is supported but outplug " + otherSidePlugName + " is not. Ignoring...");
		}

		bool alreadyDone = false;
		for( int dpId = 0; dpId < donePlugNames.size(); dpId++)
			if( donePlugNames[dpId] == otherSideSn.mayaName)
				alreadyDone = true;
		if(!alreadyDone)
		{
			this->recursiveReadConnections(otherSideSn, nodeList, externalPlugs);
			donePlugNames.push_back(sn.mayaName);
		}else{
			logger.info(MString("recread: plug: ") + sn.mayaName + " already done");
		}
	}
	
	bool alreadyDone = false;
	for( int nId = 0; nId < nodeList.size(); nId++)
	{
		if( nodeList[nId].mayaName == sn.mayaName )
		{
			alreadyDone = true;
			break;
		}
	}
	if( !alreadyDone )
		nodeList.push_back(sn);
	else
		logger.debug(MString("recread: shader node: ") + sn.mayaName + " already done - skipping");
//	MGlobal::displayInfo("pushback " + sn.mayaName);
//	//MGlobal::displayInfo(this->makeStringFromPlugList());
	return true;
}

void ShadingGroup::parseTree()
{
	//if( this->surfaceShader.m
}

void ShadingGroup::writeNodeExternalPlugs( std::ofstream& outFile, std::vector<ShaderPlug>& externalPlugs)
{
	for( int plugId = 0; plugId < externalPlugs.size(); plugId++)
	{
		ShaderPlug plug = externalPlugs[plugId];
		logger.debug(MString("Writing external plug: ") + makeGoodString(plug.plugName) + " type " + plug.vexType());

		if( plugId > 0)
			outFile << "; ";
		outFile << plug.valueVarString(0);
	}
}

void writeIncludeList( std::vector<ShaderNode>& nodeList, std::ofstream& outFile)
{
	outFile << "\n";
	std::vector<ShaderNode> doneList;
	for( int nId = 0; nId < nodeList.size(); nId++)
	{
		ShaderNode node = nodeList[nId];
		bool alreadyIncluded = false;
		for( int ndId = 0; ndId < doneList.size(); ndId++)
		{
			ShaderNode dnode = doneList[ndId];
			if( node.internalTypeName == dnode.internalTypeName)
				alreadyIncluded = true;
		}		
		if(!alreadyIncluded)
		{
			outFile << "#include \"" << node.internalTypeName << ".h\"\n";
			doneList.push_back(node);
		}
	}	
	outFile << "\n";
}

// TODO check for multiple shaders in one vex file 1 sg --> 1 vfl file
void ShadingGroup::exportVfl()
{
	logger.debug(MString("ShadingGroup::exportVfl"));
	MString vflPath = getBasePath() + "/shaders/" + makeGoodString(this->name);

	if( this->surfaceShader.shaderObject != MObject::kNullObj )
	{
		this->surfaceFileName = vflPath + "_surface.vfl";
		logger.progress(MString("Exporting surface shader ") + this->surfaceShader.mayaName + " to " + this->surfaceFileName);

		std::ofstream shaderFile(this->surfaceFileName.asChar());
		if( !shaderFile.good())
		{
			shaderFile.close();
			return;
		}
		shaderFile << "// Surface shader file for shading group " << makeGoodString(this->name) << "\n";
		writeIncludeList(this->surfaceNodeList, shaderFile);

		shaderFile << "surface " << makeGoodString(this->name) << "_surface( vector uv = {0,0,0}; ";
		this->writeNodeExternalPlugs(shaderFile, this->surfaceExternalPlugs);
		shaderFile << ")\n{";
		
		std::vector<ShaderPlug> writtenInPlugs;
		for( int nodeId = 0; nodeId < this->surfaceNodeList.size(); nodeId++)
		{
			ShaderNode sn = this->surfaceNodeList[nodeId];
			for( int p = 0; p < sn.inPlugs.size(); p++)
			{
				//MGlobal::displayInfo("check node " + sn.mayaName); 
				//if( sn.inPlugs[p].connected )
				//	MGlobal::displayInfo("inPlug " + sn.inPlugs[p].plugName + " is connected");
				if( sn.inPlugs[p].subConnected )
				{
					//MGlobal::displayInfo("sub plug of " + sn.inPlugs[p].plugName + " is connected");
					for( int sp = 0; sp < sn.inPlugs[p].subPlugs.size(); sp++)
					{
						//MGlobal::displayInfo("check sub plug " + sn.inPlugs[p].subPlugs[sp].plugName);
						//if(sn.inPlugs[p].subPlugs[sp].connected)
						//	MGlobal::displayInfo("-->is connected");
						//else
						//	MGlobal::displayInfo("-->is NOT connected");
					}
				}
			}
			shaderFile << "\n// code created by " << makeGoodString(sn.mayaName) << "\n";
			sn.writeNodeInPlugs(shaderFile, this->surfaceExternalPlugs, writtenInPlugs);
			sn.writeNodeOutPlugs(shaderFile);
			sn.writeNodeShader(shaderFile);
		}
		
		// write out variables
		// get the surface shader connection plugname
		MString otherSidePlugName;
		if(getOtherSidePlugName(MString("surfaceShader"), this->mobject, otherSidePlugName))
		{
			shaderFile << "Cf = " << pointToUnderscore(makeGoodString(otherSidePlugName)) << ";\n";
		}else{
			shaderFile << "Cf = set(0,0,1);\n";
		}
		shaderFile << "\n}\n";
		shaderFile.close();
	}
	if( this->displacementShader.shaderObject != MObject::kNullObj )
	{
		this->displacementFileName = vflPath + "_displacement.vfl";
		logger.debug(MString("Exporting displacement shader ") + this->displacementShader.mayaName + " to " + this->displacementFileName);

		std::ofstream shaderFile(this->displacementFileName.asChar());
		if( !shaderFile.good())
		{
			shaderFile.close();
			return;
		}
		shaderFile << "// displacement shader file for shading group " << this->name << "\n";
		shaderFile << "\t\t#define VOP_SHADING\n"; // I don't know what this is good for...
		shaderFile << "\t\t#define VOP_DISPLACE\n";

		writeIncludeList(this->displacementNodeList, shaderFile);

		shaderFile << "displacement " << this->name << "_displacement( vector uv = {0,0,0}; ";
		this->writeNodeExternalPlugs(shaderFile, this->displacementExternalPlugs);
		shaderFile << ")\n{";
		
		std::vector<ShaderPlug> writtenInPlugs;
		for( int nodeId = 0; nodeId < this->displacementNodeList.size(); nodeId++)
		{
			ShaderNode sn = this->displacementNodeList[nodeId];
			for( int p = 0; p < sn.inPlugs.size(); p++)
			{
				//MGlobal::displayInfo("check node " + sn.mayaName); 
				//if( sn.inPlugs[p].connected )
				//	MGlobal::displayInfo("inPlug " + sn.inPlugs[p].plugName + " is connected");
				if( sn.inPlugs[p].subConnected )
				{
					//MGlobal::displayInfo("sub plug of " + sn.inPlugs[p].plugName + " is connected");
					for( int sp = 0; sp < sn.inPlugs[p].subPlugs.size(); sp++)
					{
						//MGlobal::displayInfo("check sub plug " + sn.inPlugs[p].subPlugs[sp].plugName);
						//if(sn.inPlugs[p].subPlugs[sp].connected)
						//	MGlobal::displayInfo("-->is connected");
						//else
						//	MGlobal::displayInfo("-->is NOT connected");
					}
				}
			}
			shaderFile << "\n// code created by " << sn.mayaName << "\n";
			sn.writeNodeInPlugs(shaderFile, this->displacementExternalPlugs, writtenInPlugs);
			sn.writeNodeOutPlugs(shaderFile);
			sn.writeNodeShader(shaderFile);
		}
		
		// write out variables
		// get the surface shader connection plugname
		MString otherSidePlugName;
		if(getOtherSidePlugName(MString("displacementShader"), this->mobject, otherSidePlugName))
		{
			//shaderFile << "P = " << pointToUnderscore(otherSidePlugName) << ";\n";
			//shaderFile << "P = displacementShader1_outPoint;\n";
			//shaderFile << "N = displacementShader1_outNormal;\n";
		}else{
			//shaderFile << "P = N;\n";
		}
		shaderFile << "\n}\n";
		shaderFile.close();
	}
}

bool ShadingGroup::compileShader()
{
	MStatus stat = MS::kSuccess;
	MString outLib = getBasePath() + "/shaders/shaders.otl";

	if( this->surfaceFileName.length() > 0)
	{
		logger.debug(MString("compiling surface shader: ") + this->surfaceShader.mayaName);
		//vcc -m vops.otl shader.vfl
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + this->surfaceFileName + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return false;
		}
	}
	if( this->displacementFileName.length() > 0)
	{
		logger.debug(MString("compiling displacement shader: ") + this->displacementShader.mayaName);
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + this->displacementFileName + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return false;
		}
	}
	if( this->volumeFileName.length() > 0)
	{
		logger.debug(MString("compiling volume shader: ") + this->volumeShader.mayaName);
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + this->volumeFileName + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return false;
		}
	}
	return true;
}

//#include <maya/MFnDependencyNode.h>
//#include <maya/MGlobal.h>
//
//#include "mtm_renderOptions.h"
//#include "mtm_shaderNode.h"
//#include "mtm_shadingTree.h"
//#include "tools.h"
//#include "attrTools.h"

//ShadingTree::ShadingTree()
//{
//	this->isValid = false;
//	renderGlobals = NULL;
//}
//
//ShadingTree::ShadingTree(RenderOptions *ro)
//{
//	this->isValid = false;
//	renderGlobals = ro;
//}
//
//ShadingTree::~ShadingTree()
//{}
//
//ShadingTree::ShadingTree(RenderOptions *ro, MObject& object, ShaderType type)
//{
//	this->isValid = false;
//	this->renderGlobals = ro;
//	this->shaderType = type;
//	if(this->readShadingTree(object))
//			this->isValid = true;
//}
//
//MObject ShadingTree::getShadingGroup(MObject& geoNode)
//{
//	MObject returnObject;
//
//	MPlugArray        connections;
//	MFnDependencyNode dependNode(geoNode);
//
//	MPlug plug(geoNode, dependNode.attribute("instObjGroups"));
//
//	int instanceNum = 0;
//	plug.elementByLogicalIndex(instanceNum).connectedTo(connections, false, true);
//
//	for (unsigned int k=0; k<connections.length(); ++k)
//	{
//	  MObject shadingGroup(connections[k].node());
//	  if (shadingGroup.apiType() == MFn::kShadingEngine )
//	  {
//		 return shadingGroup;
//	  }
//	}
//	return MObject::kNullObj;
//}
//
//MString getSubPlugName(int index, ShaderPlug& plug)
//{
//	//MString info = MString("getSubPlugName::checkin plug ") + plug.plugName + " type " + plug.type;
//	//MGlobal::displayInfo(info);
//	MStringArray col;
//	col.append("R");
//	col.append("G");
//	col.append("B");
//	MStringArray vec;
//	vec.append("X");
//	vec.append("Y");
//	vec.append("Z");
//	MStringArray uv;
//	uv.append("U");
//	uv.append("V");
//	uv.append("W");
//	if( plug.type == mtmColor )
//		return plug.plugName + col[index];
//	if( plug.type == mtmVector )
//		return plug.plugName + vec[index];
//	if( plug.type == mtmFloat2 )
//	{
//		if( plug.plugName == "outUV" )
//		{
//			//MGlobal::displayInfo("Found uvw");
//			MString cutString = plug.plugName.substring(0,2);
//			return cutString + uv[index];
//		}
//		return plug.plugName + uv[index];
//	}
//	return MString("");
//}
//
//MString getSubPlugHName(int index, ShaderPlug& plug)
//{
//	MStringArray col;
//	col.append(".r");
//	col.append(".g");
//	col.append(".b");
//	MStringArray vec;
//	vec.append(".x");
//	vec.append(".y");
//	vec.append(".z");
//	if( plug.type == mtmColor )
//		return plug.plugName + col[index];
//	if( plug.type == mtmVector )
//		return plug.plugName + vec[index];
//	if( plug.type == mtmFloat2 )
//		return plug.plugName + vec[index];
//	return MString("");
//}
//
//MString getSubPlugHNameA(int index, MString name, PlugType type)
//{
//	MStringArray col;
//	col.append(".r");
//	col.append(".g");
//	col.append(".b");
//	MStringArray vec;
//	vec.append(".x");
//	vec.append(".y");
//	vec.append(".z");
//	if( type == mtmColor )
//		return name + col[index];
//	if( type == mtmVector )
//		return name + vec[index];
//	if( type == mtmFloat2 )
//		return name + vec[index];
//	return MString("");
//}
//
//bool ShadingTree::isOutPlugSupported( ShaderNode& sn, MString plugName, int *inPlugId, int *inSubId)
//{
//	int numPlugs = (int)sn.outPlugs.size();
//	MString shortPlugName = getPlugName(plugName); // if the plug contains a . for lambert1.outColor only outColor is returned
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		*inPlugId = plugId;
//		ShaderPlug sp = sn.outPlugs[plugId];
//		//MGlobal::displayInfo("isOutPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.plugname " + sn.outPlugs[plugId].plugName);
//		if( sp.plugName == shortPlugName )
//			return true;
//		if( sp.plugLen > 1)
//		{
//			for( int subId = 0; subId < sp.plugLen; subId++)
//			{
//				*inSubId = subId;
//				MString subName = getSubPlugName(subId, sp);
//				//MGlobal::displayInfo("isOutPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.subPlugname " + subName);
//				if( subName == shortPlugName)
//					return true;
//			}
//		}
//	}
//	return false;
//
//}
//
//
//bool ShadingTree::isOutPlugSupported( ShaderNode& sn, MString plugName)
//{
//	int numPlugs = (int)sn.outPlugs.size();
//	MString shortPlugName = getPlugName(plugName); // if the plug contains a . for lambert1.outColor only outColor is returned
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		ShaderPlug sp = sn.outPlugs[plugId];
//		//MGlobal::displayInfo("isOutPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.plugname " + sn.outPlugs[plugId].plugName);
//		if( sp.plugName == shortPlugName )
//			return true;
//		if( sp.plugLen > 1)
//		{
//			for( int subId = 0; subId < sp.plugLen; subId++)
//			{
//				MString subName = getSubPlugName(subId, sp);
//				//MGlobal::displayInfo("isOutPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.subPlugname " + subName);
//				if( subName == shortPlugName)
//					return true;
//			}
//		}
//	}
//	return false;
//
//}
//
//bool ShadingTree::isInPlugSupported( ShaderNode& sn, MString plugName)
//{
//	int numPlugs = (int)sn.inPlugs.size();
//	MString shortPlugName = getPlugName(plugName); // if the plug contains a . for lambert1.outColor only outColor is returned
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		ShaderPlug sp = sn.inPlugs[plugId];
//		//MGlobal::displayInfo("isInPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.plugname " + sn.inPlugs[plugId].plugName);
//		if( sp.plugName == shortPlugName )
//			return true;
//		if( sp.plugLen > 1)
//		{
//			for( int subId = 0; subId < sp.plugLen; subId++)
//			{
//				MString subName = getSubPlugName(subId, sp);
//				//MGlobal::displayInfo("isInPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.subPlugname " + subName);
//				if( subName == shortPlugName)
//					return true;
//			}
//		}
//	}
//	return false;
//}
//
//
//bool ShadingTree::isInPlugSupported( ShaderNode& sn, MString plugName, int *inPlugId, int *inSubId)
//{
//	int numPlugs = (int)sn.inPlugs.size();
//	MString shortPlugName = getPlugName(plugName); // if the plug contains a . for lambert1.outColor only outColor is returned
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		*inPlugId = plugId;
//		ShaderPlug sp = sn.inPlugs[plugId];
//		//MGlobal::displayInfo("isInPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.plugname " + sn.inPlugs[plugId].plugName);
//		if( sp.plugName == shortPlugName )
//			return true;
//		if( sp.plugLen > 1)
//		{
//			for( int subId = 0; subId < sp.plugLen; subId++)
//			{
//				*inSubId = subId;
//				MString subName = getSubPlugName(subId, sp);
//				//MGlobal::displayInfo("isInPlugSupported:: compare plugShortName " + shortPlugName + " mit sn.subPlugname " + subName);
//				if( subName == shortPlugName)
//					return true;
//			}
//		}
//	}
//	return false;
//}
//
//bool ShadingTree::isNodeSupported( MObject& object, ShaderNode& sn)
//{
//	MFnDependencyNode depFn(object);
//	for( int shId = 0; shId < this->renderGlobals->supportedShaders.size(); shId++)
//	{
//		sn = this->renderGlobals->supportedShaders[shId];
//		MString sType = object.apiTypeStr();
//		
//		if( sType == sn.mayaType )
//		{
//			//MGlobal::displayInfo("isNodeSupported: Found and added supported node:" + depFn.name());
//			sn.mayaName = depFn.name();
//			sn.shaderObject = object;
//			this->getPlugAttrs(sn);
//			return true;
//		}
//	}
//	//MGlobal::displayInfo("isNodeSupported: Not supported node:" + depFn.name() + " type " + object.apiTypeStr());
//	sn.mayaName = depFn.name();
//	sn.shaderObject = object;
//
//	return false;
//}
//
//bool ShadingTree::isShadingNode(MObject& object)
//{
//	MFnDependencyNode depFn(object);
//	MString classification = depFn.classification(depFn.typeName());
//	if(classification.indexW("shader") > -1)
//		return true;
//	return false;
//}
//
//bool ShadingTree::recursiveReadConnections(MObject& object)
//{
//	bool result = true;
//
//	ShaderNode sn;
//	MFnDependencyNode depFn(object);
//
//	//MGlobal::displayInfo("Check node " + depFn.name());
//
//	// Wenn dieser Node supported ist, dann mache weiter.
//	// Sn wird automatisch mit den Werten des objects gefüllt
//	if( this->isNodeSupported(object, sn))
//	{
//		//// lese die aktuellen Werte aus, werden evtl. irrelevant wenn der plug connected ist
//		//// ist aber egal.
//		//int numInPlugs = sn.inPlugs.size();
//		//for( int plugId = 0; plugId < numInPlugs; plugId++)
//		//{
//		//	int plugLen = sn.inPlugs[plugId].plugLen;
//		//	for( int plugCId = 0; plugCId < plugLen; plugCId++)
//		//	{
//		//		// stelle fest ob der plug connected ist
//		//		sn.inPlugs[plugId].connected = false;
//		//		MStringArray otherSidePlugNames;
//		//		MGlobal::displayInfo("recursiveReadConnections::Check input connection of " + sn.inPlugs[plugId].plugName);
//		//		MObject plugObject = getOtherSideNode(sn.inPlugs[plugId].plugName, object, otherSidePlugNames);
//		//		if( plugObject == MObject::kNullObj )
//		//		{
//		//			MGlobal::displayInfo("recursiveReadConnections::plugObject == nullobj"); 
//		//			continue;
//		//		}
//		//		MGlobal::displayInfo("recursiveReadConnections::Connections found " + otherSidePlugNames[0]);
//		//		// okay ist connected, feststellen ob der Node auf der anderen Seite supported ist
//		//		// dummySn bekommt automatisch die entsprechenden Werte
//		//		ShaderNode dummySn;
//		//		if( this->isNodeSupported(plugObject, dummySn))
//		//		{
//		//			// Hier kann z.B. auch message in colorR connected sein, message ist aber nicht supported,
//		//			// also checken
//
//		//			if( this->isOutPlugSupported(dummySn, otherSidePlugName)
//		//			{
//		//				sn.inPlugs[plugId].connected = true;
//		//				sn.inPlugs[plugId].otherSidePlugName = otherSidePlugName;
//		//			}else{
//		//				MGlobal::displayInfo("recursiveReadConnections::connection between " + otherSidePlugName + " and " + sn.inPlugs[plugId].plugName + " are not (yet) supported"); 
//		//			}
//		//		}else{
//		//			//node ist nicht supported, aber eventuell kein Shading node, kann also animcurve input haben,
//		//			//expression oder auch andere plugs. Falls das der Fall ist, wird die Connection nach aussen getragen
//		//			//für den fall dass es eine Animation ist
//		//			MGlobal::displayInfo("recursiveReadConnections::OtherSideNode not supported ");
//		//			if( !this->isShadingNode(plugObject) )
//		//			{
//		//				MGlobal::displayInfo("recursiveReadConnections::But it is not a shading node so I make it visible in shader arguments");
//		//				this->shaderCmdPlugs.push_back(&sn.inPlugs[plugId]);
//		//			}
//		//		}
//		//		//MGlobal::displayInfo("recursiveReadConnections: Check connected plug " + sn.inPlugs[plugId].plugName);
//		//		this->recursiveReadConnections(plugObject);
//		//	}
//		//}
//		//this->treeNodes.push_back(sn);
//	}
//	return result;
//}
//
//
//bool ShadingTree::recursiveReadConnections(MObject& object, ShaderNode& sn)
//{
//	bool result = true;
//	MString info;
//	MFnDependencyNode depFn(object);
//
//	MGlobal::displayInfo("Check node " + depFn.name());
//
//	// shader node is filled with current plug values. Now I have to find out
//	// if the node is connected to other nodes. First I need the connected plugs
//	MPlugArray connectedPlugs;
//	depFn.getConnections(connectedPlugs);
//	int numConnections = connectedPlugs.length();
//	//info = MString("Node has ") + numConnections + " in or out connections.";
//	//MGlobal::displayInfo(info);
//	std::vector<ShaderNode *> donePlugs;
//
//	for( int connId = 0; connId < numConnections; connId++)
//	{
//		// check for incoming connections only. Outgoing connections are not relevant
//		MPlug plug = connectedPlugs[connId];
//		if( plug.isSource() )
//			continue;
//
//		//info = MString("connection " + plug.name() + " is destination plug");MGlobal::displayInfo(info);
//		MString shortPlugName = plug.partialName(false, false, false, false, false, true);
//		//info = MString("plug short name " + shortPlugName);MGlobal::displayInfo(info);
//
//		// check if the plug short name is supported in the current shading input nodes
//		int plugId = -1, subId = -1;
//		if( !this->isInPlugSupported(sn, shortPlugName, &plugId, &subId) )
//		{
//			MString("plug type " + sn.mayaType + " is not supported");MGlobal::displayInfo(info);
//			continue;
//		}
//
//		MString otherSidePlugName;
//		MObject plugObject = getOtherSideNode(shortPlugName, object, otherSidePlugName);
//		if( plugObject == MObject::kNullObj )
//		{
//			MString("Uable to get other side node." + otherSidePlugName);MGlobal::displayInfo(info);
//			continue;
//		}
//		// check if the node on the other side is supported, if not ignore this connection
//		ShaderNode newSn;
//		if( !this->isNodeSupported(plugObject, newSn))
//		{
//			MString("other side node is not supported.");MGlobal::displayInfo(info);
//			// the node is not supported, so check if it is a shadeing node or something else
//			// if it is something else, e.g. a anim curve, the attributes have to appear in the shader declaration
//			//MGlobal::displayInfo("recursiveReadConnections::OtherSideNode called "  + newSn.mayaName + " is not supported ");
//			if( !this->isShadingNode(plugObject) )
//			{
//				//MGlobal::displayInfo("recursiveReadConnections::But it is not a shading node so I make it visible in shader arguments");
//				//MGlobal::displayInfo("recursiveReadConnections::shaderCmdPlugs - nodeName: " + sn.inPlugs[plugId].nodeName + " plugName: " + sn.inPlugs[plugId].plugName);
//				ShaderPlug spp = sn.inPlugs[plugId];
//				bool found = false;
//				for( int spid = 0; spid < this->shaderCmdPlugs.size(); spid++)
//					if( this->shaderCmdPlugs[spid].nodeName == spp.nodeName)
//						found = true;
//				if( !found)
//					this->shaderCmdPlugs.push_back(sn.inPlugs[plugId]);
//			}
//			continue;
//		}
//
//		// well, the other side node is supported, but is the outPlug on the other side allowed as well?
//		int outPlugId = -1, outSubId = -1;
//		if( !this->isOutPlugSupported(newSn, otherSidePlugName, &outPlugId, &outSubId) )
//		{
//			info = MString("other side plug is not supported.");MGlobal::displayInfo(info);
//			continue;
//		}		
//		if( outSubId > -1)
//			otherSidePlugName = otherSidePlugName.substring(0, otherSidePlugName.length() - 2); // remove RGB/XYZ
//
//		if( plugId != -1)
//		{
//			sn.inPlugs[plugId].connected = true;
//			if( subId != -1)
//			{
//				sn.inPlugs[plugId].subConnected = true;
//				//MString noSubIdentifier = otherSidePlugName.substring(0, otherSidePlugName.length() - 1);
//				sn.inPlugs[plugId].connectedSubPlugs[subId] = otherSidePlugName; 
//			}else{
//				sn.inPlugs[plugId].otherSidePlugName = otherSidePlugName;
//			}
//		}
//		bool alreadyDone = false;
//		for( int dpId = 0; dpId < donePlugs.size(); dpId++)
//			if( donePlugs[dpId] == &sn)
//				alreadyDone = true;
//		if(!alreadyDone)
//		{
//			MGlobal::displayInfo("call rec " + newSn.mayaName);
//			this->recursiveReadConnections(plugObject, newSn);
//			donePlugs.push_back(&sn);
//		}
//	}
//	this->treeNodes.push_back(sn);
//	MGlobal::displayInfo("pushback " + sn.mayaName);
//	//MGlobal::displayInfo(this->makeStringFromPlugList());
//	return true;
//}
//
//bool ShadingTree::readShadingTree(MObject& geoNode)
//{
//	bool result = true;
//	this->shadingGroup = this->getShadingGroup(geoNode);
//	if( this->shadingGroup == MObject::kNullObj)
//		return false;
//	
//	//read displacement shader, volume und surface
//	MObject shader;
//	if( this->shaderType == mtmSurface)
//		shader = getOtherSideNode(MString("surfaceShader"), this->shadingGroup);
//	if( this->shaderType == mtmVolume)
//		shader = getOtherSideNode(MString("volumeShader"), this->shadingGroup);
//	if( this->shaderType == mtmDisplacement)
//		shader = getOtherSideNode(MString("displacementShader"), this->shadingGroup);
//
//	if( shader == MObject::kNullObj)
//	{
//		//MGlobal::displayInfo("Could not find surfaceShader");
//		return false;
//	}
//	
//	this->shaderCmdPlugs.clear();
//
//	MFnDependencyNode depFn(shader);
//	//MGlobal::displayInfo("read shading tree for object " + depFn.name() );
//	ShaderNode sn;
//	if( this->isNodeSupported(shader, sn))
//	{
//		this->name = depFn.name();
//		this->recursiveReadConnections(shader, sn);
//	}else{
//		return false;
//	}
//	return true;
//}
//
//void ShadingTree::defineOutputAttrs( ShaderNode& sn, std::ofstream& shaderFile)
//{
//	int numOutPlugs = sn.outPlugs.size();
//	for( int plugId = 0; plugId < numOutPlugs; plugId++)
//	{
//		shaderFile << this->houdiniType(sn.outPlugs[plugId].type).asChar() << " " << sn.mayaName.asChar() << "_" << sn.outPlugs[plugId].plugName.asChar() << ";\n"; 
//	}
//}
//
//void ShadingTree::getPlugAttrs( ShaderNode& sn)
//{
//	int numInPlugs = sn.inPlugs.size();
//	MFnDependencyNode depNode(sn.shaderObject);
//	
//	for( int plugId = 0; plugId < numInPlugs; plugId++)
//	{
//		ShaderPlug *pl = &sn.inPlugs[plugId];
//		pl->node = sn.shaderObject;
//		pl->nodeName = sn.mayaName;
//		if(	pl->type == mtmFloat )
//		{
//			getFloat(pl->plugName, depNode, pl->floatValue);
//			pl->plugValueAsString = MString("") + pl->floatValue;
//			pl->plugValueCmdString = MString("") + pl->floatValue;
//		}
//		if(	pl->type == mtmColor )
//		{
//			getColor(pl->plugName, depNode, pl->colorValue);
//			pl->plugValueAsString = MString("set(") + pl->colorValue.r + "," + pl->colorValue.g + "," + pl->colorValue.b + ")";
//			pl->plugValueCmdString = MString("{") + pl->colorValue.r + "," + pl->colorValue.g + "," + pl->colorValue.b + "}";
//		}
//		if(	pl->type == mtmVector )
//		{
//			getVector(pl->plugName, depNode, pl->vectorValue);
//			pl->plugValueAsString = MString("set(") + pl->vectorValue.x + "," + pl->vectorValue.y + "," + pl->vectorValue.z + ")";
//			pl->plugValueCmdString = MString("{") + pl->vectorValue.x + "," + pl->vectorValue.y + "," + pl->vectorValue.z + "}";
//		}
//		if(	pl->type == mtmFloat2 )
//		{
//			getFloat2(pl->plugName, depNode, pl->vectorValue);
//			pl->plugValueAsString = MString("set(") + pl->vectorValue.x + "," + pl->vectorValue.y + "," + pl->vectorValue.z + ")";
//			pl->plugValueCmdString = MString("{") + pl->vectorValue.x + "," + pl->vectorValue.y + "," + pl->vectorValue.z + "}";
//		}
//		if(	pl->type == mtmString )
//		{
//			getString(pl->plugName, depNode, pl->stringValue);
//			pl->plugValueAsString = pl->stringValue;
//			pl->plugValueCmdString = pl->stringValue;
//		}
//		if(	pl->type == mtmInt )
//		{
//			getInt(pl->plugName, depNode, pl->intValue);
//			pl->plugValueAsString = MString("") + pl->intValue;
//			pl->plugValueCmdString = MString("") + pl->intValue;
//		}
//		if(	pl->type == mtmBool )
//		{
//			getBool(pl->plugName, depNode, pl->boolValue);
//			pl->plugValueAsString = MString("") + pl->boolValue;
//			pl->plugValueCmdString = MString("") + pl->boolValue;
//		}
//	}
//}
//
//MString ShadingTree::getPlugAttrAsString( ShaderPlug pl)
//{
//	MFnDependencyNode depNode(pl.node);
//	MString val;
//
//	if(	pl.type == mtmFloat )
//	{
//		getFloat(pl.plugName, depNode, pl.floatValue);
//		val = MString("") + pl.floatValue;
//	}
//	if(	pl.type == mtmColor )
//	{
//		getColor(pl.plugName, depNode, pl.colorValue);
//		val = MString("") + pl.colorValue.r + " " + pl.colorValue.g + " " + pl.colorValue.b;
//	}
//	if(	pl.type == mtmVector )
//	{
//		getVector(pl.plugName, depNode, pl.vectorValue);
//		val = MString("") + pl.vectorValue.x + " " + pl.vectorValue.y + " " + pl.vectorValue.z;
//	}
//	if(	pl.type == mtmFloat2 )
//	{
//		getFloat2(pl.plugName, depNode, pl.vectorValue);
//		val = MString("") + pl.vectorValue.x + " " + pl.vectorValue.y + " " + pl.vectorValue.z;
//	}
//	if(	pl.type == mtmString )
//	{
//		getString(pl.plugName, depNode, pl.stringValue);
//		val = pl.stringValue;
//	}
//	if(	pl.type == mtmInt )
//	{
//		getInt(pl.plugName, depNode, pl.intValue);
//		val = MString("") + pl.intValue;
//	}
//	if(	pl.type == mtmBool )
//	{
//		getBool(pl.plugName, depNode, pl.boolValue);
//		val = MString("") + pl.boolValue;
//	}
//	return val;
//}
//
//MString ShadingTree::houdiniType(PlugType type )
//{
//	switch(type)
//	{
//	case mtmVector:
//		return MString("vector");
//	case mtmColor:
//		return MString("vector");
//	case mtmFloat:
//		return MString("float");
//	case mtmInt:
//		return MString("int");
//	case mtmBool:
//		return MString("int");
//	case mtmFloat2:
//		return MString("vector");
//	default:
//		return MString("");
//	}
//}
//
//
//MString ShadingTree::makeCmdStringFromPlugList()
//{
//	MString cmd = this->typeAndName;
//
//	int numPlugs = this->shaderCmdPlugs.size();
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		if( plugId > 0)
//			cmd += " ";
//		ShaderPlug splug = this->shaderCmdPlugs[plugId];
//		cmd += " " + splug.nodeName + "_";
//		cmd += splug.plugName;
//		cmd += " " + this->getPlugAttrAsString(splug);
//		//MString info = MString("Get cmd value from node " + splug.nodeName + " att name " + splug.plugName);
//		//MGlobal::displayInfo(info);
//		//get value from splug.node, attrname, 
//	}
//	return cmd;
//}
//
//MString ShadingTree::makeStringFromPlugList()
//{
//	int numPlugs = this->shaderCmdPlugs.size();
//	MString plugString("");
//	for( int plugId = 0; plugId < numPlugs; plugId++)
//	{
//		if( plugId > 0)
//			plugString += ";";
//		ShaderPlug splug = this->shaderCmdPlugs[plugId];
//		plugString += houdiniType(splug.type);
//		plugString += " " + splug.nodeName + "_";
//		plugString += splug.plugName;
//		plugString += " = " + splug.plugValueCmdString;
//	}
//	return plugString;
//}
//
//MString getSubAsString(int index, ShaderPlug& splug)
//{
//	if( splug.plugLen == 1)
//	{
//		if( splug.type == mtmFloat)
//			return MString("") + splug.floatValue;
//		if( splug.type == mtmInt)
//			return MString("") + splug.intValue;
//		if( splug.type == mtmBool)
//			return MString("") + splug.boolValue;
//		if( splug.type == mtmString)
//			return MString("") + splug.stringValue;
//	}
//	if( splug.plugLen == 3)
//	{
//		if( splug.type == mtmColor)
//		{
//			return MString("") + splug.colorValue[index];
//		}
//		if( splug.type == mtmVector)
//		{
//			return MString("") + splug.vectorValue[index];
//		}
//		if( splug.type == mtmFloat2)
//		{
//			return MString("") + splug.vectorValue[index];
//		}
//	}
//}
//
//bool ShadingTree::exportShadingTree()
//{
//	//MFn::Type::kPlace2dTexture
//	bool result = false;
//	if( this->treeNodes.size() == 0)
//		return result;
//
//	MFnDependencyNode sgNode(this->shadingGroup);
//	this->outFile = this->renderGlobals->basePath + "/shaders/" + sgNode.name() + ".vfl";
//	std::ofstream shaderFile( this->outFile.asChar());
//	if( !shaderFile.good())
//	{
//		shaderFile.close();
//		return false;
//	}
//
//	//MString shaderIncludeFileName = "C:/Users/haggi/coding/mayaTo/VS/shaderIncludes.h";
//	//shaderFile << "#include \"" << shaderIncludeFileName.asChar() << "\"\n\n";
//
//	int numShaders = this->treeNodes.size();
//	
//	MStringArray alreadyIncluded;
//
//	for( int shId = 0; shId < numShaders; shId++)
//	{
//		bool found = false;
//		for( int inId = 0; inId < alreadyIncluded.length(); inId++)
//		{
//			if( alreadyIncluded[inId] == this->treeNodes[shId].internalName)
//			{
//				found = true;
//				break;
//			}
//		}
//		if(!found)
//		{
//			shaderFile << "#include \"" + this->treeNodes[shId].internalName + ".h\"\n";
//			alreadyIncluded.append(this->treeNodes[shId].internalName);
//		}
//	}
//	shaderFile << "\n\n";
//
//	//int numCmdPlugs = (int)this->shaderCmdPlugs.size();
//	MString cmdPlugString("");
//	//for( int shCId = 0; shCId < numCmdPlugs; shCId++)
//	//{
//	//	ShaderPlug sp = *(shaderCmdPlugs[shCId]);
//	//	if( shCId > 0)
//	//		cmdPlugString += ";";
//	//	cmdPlugString += sp.houdiniTypeName + " " + sp.nodeName + "_" + sp.plugName + " = " + sp.plugValueAsString;
//	//}
//	cmdPlugString = makeStringFromPlugList();
//	
//	MString shaderCmdString;
//	if( this->shaderType == mtmSurface)
//	{
//		shaderFile << "surface " << sgNode.name().asChar() << "_surface(" << cmdPlugString << ")\n{\n";
//		shaderCmdString += sgNode.name() + "_surface";
//		this->typeAndName = MString("surface ") + sgNode.name().asChar() + "_surface";
//	}
//	if( this->shaderType == mtmVolume)
//	{
//		shaderFile << "volume " << sgNode.name().asChar() << "_volume(" << cmdPlugString << ")\n{\n";
//		shaderCmdString += sgNode.name() + "_volume";
//		this->typeAndName = MString("volume ") + sgNode.name().asChar() + "_volume";
//	}
//	if( this->shaderType == mtmDisplacement)
//	{	
//		shaderFile << "displacement " << sgNode.name().asChar() << "_displacement(" << cmdPlugString << ")\n{\n";
//		shaderCmdString += sgNode.name() + "_displacement";
//		this->typeAndName = MString("displacement ") + sgNode.name().asChar() + "_displacement";
//	}
//	
//	
//	//this->shaderCmd = makeCallFromCmdString(shaderCmdString);
//
//	//MGlobal::displayInfo("-------->>ShadingTree::exportShadingTree: shader command string: " + shaderCmdString + " for tree named " + this->name);
//
//
//	MString shaderArgString;
//
//	for( int shId = 0; shId < numShaders; shId++)
//	{
//		ShaderNode sn = this->treeNodes[shId];
//		//MGlobal::displayInfo("exportShadingTree::exporting node called: " + sn.mayaName + " " + sn.mayaType);
//		//MGlobal::displayInfo("exportShadingTree::exporting #include " + sn.internalName + ".h");
//		//this->getPlugAttrs(sn);
//		this->defineOutputAttrs(sn, shaderFile);
//		//// getConnectedInputNames
//		int numInPlugs = sn.inPlugs.size();
//		int numOutPlugs = sn.outPlugs.size();
//		MString plugArgString;
//		for( int plugId = 0; plugId < numInPlugs; plugId++)
//		{
//			ShaderPlug splug = sn.inPlugs[plugId];
//			int numCmdPlugs = (int)this->shaderCmdPlugs.size();
//			bool isCmdPlug = false;
//			for( int cmdPlugId = 0; cmdPlugId < numCmdPlugs; cmdPlugId++)
//			{
//				if(this->shaderCmdPlugs[cmdPlugId].nodeName == splug.nodeName && this->shaderCmdPlugs[cmdPlugId].plugName == splug.plugName)
//				{
//					isCmdPlug = true;
//					break;
//				}
//			}	
//			if( plugId > 0)
//				plugArgString += ", ";
//			plugArgString += sn.mayaName + "_" + splug.plugName;
//			
//			if( isCmdPlug )
//				continue;
//
//			//MGlobal::displayInfo("exportShadingTree::check plug: " + splug.plugName);
//			shaderFile << splug.houdiniTypeName << " " << sn.mayaName.asChar() << "_" << splug.plugName.asChar() << " = ";
//
//			// plug is connected, so check if elements are connected or not
//			if( splug.connected )
//			{
//				MString otherSideConnection = makeGoodString(splug.otherSidePlugName);
//				//MGlobal::displayInfo("plug is connected to: " + otherSideConnection);
//				if( splug.subConnected )
//				{
//					//MGlobal::displayInfo("plug is sub connected");
//					MString valueString("set(");
//					for( int subCId = 0; subCId < splug.plugLen; subCId++)
//					{
//						if( subCId > 0)
//							valueString += ",";
//
//						MString subVal = makeGoodString(splug.connectedSubPlugs[subCId]);
//						if( subVal.length() == 0)
//						{
//							MString subValStr = getSubAsString(subCId, splug);
//							valueString += subValStr;
//						}else{
//							MString subName = getSubPlugHNameA(subCId, subVal, splug.type);
//							//MString subName = getSubPlugHName(subCId, splug);
//							valueString += subName;
//						}
//					}
//					valueString += ")";
//					shaderFile << valueString << ";\n";
//				}else{
//					//MGlobal::displayInfo("plug is not subconnected to");
//					shaderFile << otherSideConnection.asChar() << ";\n";
//				}	
//			}else{
//				//MGlobal::displayInfo("plug not connected: " + splug.plugValueAsString);
//				if( splug.type == mtmString )
//					shaderFile << "\"" << splug.plugValueAsString << "\";\n";
//				else
//					shaderFile << splug.plugValueAsString << ";\n";
//			}	
//		}
//
//		for( int plugId = 0; plugId < numOutPlugs; plugId++)
//		{
//			ShaderPlug splug = sn.outPlugs[plugId];
//			if( plugArgString.length() > 0)
//				plugArgString += ", ";
//			plugArgString += sn.mayaName + "_" + splug.plugName;
//		}
//		
//		shaderFile << "\n" << sn.internalName.asChar() << "(" << plugArgString.asChar() << ");\n\n";
//	}
//
//	if( this->shaderType == mtmSurface)
//	{
//		MPlug inPlug, outPlug;
//		if(getConnectedPlugs(MString("surfaceShader"), this->shadingGroup, inPlug, outPlug))
//		{
//			shaderFile << "Cf = " <<  makeGoodString(outPlug.name()).asChar() << ";\n";
//		}
//	}	
//	shaderFile << "\n}";
//	shaderFile.close();
//	this->isValid = true;
//	return result;
//}
//
//bool ShadingTree::compileShader()
//{
//	if(!this->isValid )
//		return false;
//	bool result = false;
//	//vcc -m vops.otl shader.vfl
//	cout << "\n\n=================== compiling shader " << this->name << " ======================\n";
//	MString outLib = this->renderGlobals->basePath + "/shaders/shaders.otl";
//	MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + this->outFile + "\")");
//	MGlobal::displayInfo(cmd);
//	MGlobal::executePythonCommand(cmd);
//	return result;
//}
//
//HyperShade::HyperShade()
//{}
//
//HyperShade::HyperShade(RenderOptions *opt)
//{
//	this->renderGlobals = opt;
//}
//
//HyperShade::~HyperShade()
//{}
//
//// 
//std::vector<Material *> HyperShade::appendNode(MObject object)
//{
//	Material *mat;
//	std::vector<Material *> matList;
//	ShadingTree st(this->renderGlobals);
//	MObject shadingGroup = st.getShadingGroup(object);
//	MFnDependencyNode fn(object);
//
//	// No shading group found
//	if( shadingGroup == MObject::kNullObj)
//	{
//		MGlobal::displayInfo("no shading group for object " + fn.name());
//		return matList;
//	}else{
//		//MGlobal::displayInfo("get shading group for object " + fn.name());
//	}
//	int numMaterials = (int)this->materialList.size();
//	for( int matId = 0; matId < numMaterials; matId++)
//	{
//		if( shadingGroup == this->materialList[matId]->surfaceShader.shadingGroup ||
//			shadingGroup == this->materialList[matId]->volumeShader.shadingGroup ||
//			shadingGroup == this->materialList[matId]->displacementShader.shadingGroup)
//			{
//				mat = this->materialList[matId];
//				this->objectList.push_back(object);
//				this->materialList.push_back(mat);
//				matList.push_back(mat);
//				return matList;
//			}
//	}	
//	mat = new Material();
//
//	//MGlobal::displayInfo("=========== Surface Shader ================");
//	mat->surfaceShader = ShadingTree(this->renderGlobals, object, mtmSurface);
//	//MGlobal::displayInfo("=========== Volume Shader ================");
//	mat->volumeShader = ShadingTree(this->renderGlobals, object, mtmVolume);
//	//MGlobal::displayInfo("=========== Displacement Shader ================");
//	mat->displacementShader = ShadingTree(this->renderGlobals, object, mtmDisplacement);
//
//	
//	if( mat->surfaceShader.isValid || mat->volumeShader.isValid || mat->displacementShader.isValid)
//	{	
//		this->objectList.push_back(object);
//		this->materialList.push_back(mat);
//		matList.push_back(mat);
//	}else{
//		MGlobal::displayInfo("None of the shaders is valid --> no material.");
//	}
//	return(matList);
//}
//
//void HyperShade::exportShader()
//{
//	int numMat = this->materialList.size();
//	for( int shId = 0; shId < numMat; shId++)
//	{
//		this->materialList[shId]->surfaceShader.exportShadingTree();
//		this->materialList[shId]->surfaceShader.compileShader();
//		this->materialList[shId]->displacementShader.exportShadingTree();
//		this->materialList[shId]->displacementShader.compileShader();
//		this->materialList[shId]->volumeShader.exportShadingTree();
//		this->materialList[shId]->volumeShader.compileShader();
//	}
//
//}


//		if( !this->isNodeSupported(plugObject, newSn))
//		{
//			MString("other side node is not supported.");MGlobal::displayInfo(info);
//			// the node is not supported, so check if it is a shadeing node or something else
//			// if it is something else, e.g. a anim curve, the attributes have to appear in the shader declaration
//			//MGlobal::displayInfo("recursiveReadConnections::OtherSideNode called "  + newSn.mayaName + " is not supported ");
//			if( !this->isShadingNode(plugObject) )
//			{
//				//MGlobal::displayInfo("recursiveReadConnections::But it is not a shading node so I make it visible in shader arguments");
//				//MGlobal::displayInfo("recursiveReadConnections::shaderCmdPlugs - nodeName: " + sn.inPlugs[plugId].nodeName + " plugName: " + sn.inPlugs[plugId].plugName);
//				ShaderPlug spp = sn.inPlugs[plugId];
//				bool found = false;
//				for( int spid = 0; spid < this->shaderCmdPlugs.size(); spid++)
//					if( this->shaderCmdPlugs[spid].nodeName == spp.nodeName)
//						found = true;
//				if( !found)
//					this->shaderCmdPlugs.push_back(sn.inPlugs[plugId]);
//			}
//			continue;
//		}
//
//		// well, the other side node is supported, but is the outPlug on the other side allowed as well?
//		int outPlugId = -1, outSubId = -1;
//		if( !this->isOutPlugSupported(newSn, otherSidePlugName, &outPlugId, &outSubId) )
//		{
//			info = MString("other side plug is not supported.");MGlobal::displayInfo(info);
//			continue;
//		}		
//		if( outSubId > -1)
//			otherSidePlugName = otherSidePlugName.substring(0, otherSidePlugName.length() - 2); // remove RGB/XYZ
//
//		if( plugId != -1)
//		{
//			sn.inPlugs[plugId].connected = true;
//			if( subId != -1)
//			{
//				sn.inPlugs[plugId].subConnected = true;
//				//MString noSubIdentifier = otherSidePlugName.substring(0, otherSidePlugName.length() - 1);
//				sn.inPlugs[plugId].connectedSubPlugs[subId] = otherSidePlugName; 
//			}else{
//				sn.inPlugs[plugId].otherSidePlugName = otherSidePlugName;
//			}
//		}
