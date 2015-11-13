#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMeshData.h>

#include "mayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "shadingtools/shadingUtils.h"
#include "world.h"
#include "renderGlobals.h"

static Logging logger;

static std::vector<int> lightIdentifier; // plugids for detecting new lighttypes
static std::vector<int> objectIdentifier; // plugids for detecting new objTypes

void addLightIdentifier(int id)
{
	lightIdentifier.push_back(id);
}

void addObjectIdentifier(int id)
{
	objectIdentifier.push_back(id);
}

bool MayaObject::isInstanced()
{
	return this->dagPath.isInstanced() || (this->instanceNumber > 0) || ((this->attributes != nullptr) && this->attributes->hasInstancerConnection);
}

bool MayaObject::isLight()
{
	if( this->mobject.hasFn(MFn::kLight))
		return true;
	MFnDependencyNode depFn(this->mobject);
	uint nodeId = depFn.typeId().id();
	for( uint lId = 0; lId < lightIdentifier.size(); lId++)
	{
		if( nodeId == lightIdentifier[lId])
		{
			Logging::debug(MString("Found external lighttype: ") + depFn.name());
			return true;
		}
	}
	return false;
}

bool MayaObject::isCamera()
{
	if( this->mobject.hasFn(MFn::kCamera))
	{
		this->motionBlurred = true;
		return true;
	}
	return false;
}

bool MayaObject::isGeo()
{

	if( this->mobject.hasFn(MFn::kMesh))
		return true;
	if( this->mobject.hasFn(MFn::kNurbsSurface))
		return true;
	if( this->mobject.hasFn(MFn::kParticle))
		return true;
	if( this->mobject.hasFn(MFn::kSubSurface))
		return true;
	if( this->mobject.hasFn(MFn::kNurbsCurve))
		return true;
	if( this->mobject.hasFn(MFn::kHairSystem))
		return true;

	MFnDependencyNode depFn(this->mobject);
	uint nodeId = depFn.typeId().id();
	for( uint lId = 0; lId < objectIdentifier.size(); lId++)
	{
		if( nodeId == objectIdentifier[lId])
		{
			Logging::debug(MString("Found external geotype: ") + depFn.name());
			return true;
		}
	}
	return false;
}

bool MayaObject::isTransform()
{
	if( this->mobject.hasFn(MFn::kTransform))
		return true;
	return false;
}


void MayaObject::getShadingGroups()
{
	// get shading groups only for allowed shapes

	if( this->geometryShapeSupported() )
	{
		Logging::debug(MString("getShadingGroups::Supported geo ") + this->shortName);
		// only makes sense if we have a geometry shape.
		if( this->mobject.hasFn(MFn::kMesh) || this->mobject.hasFn(MFn::kNurbsSurface) || this->mobject.hasFn(MFn::kParticle) || this->mobject.hasFn(MFn::kNParticle))
		{
			getObjectShadingGroups(this->dagPath, this->perFaceAssignments, this->shadingGroups, true);
		}
	}
}

// check if the node or any of its parents has a animated visibility
bool  MayaObject::isVisiblityAnimated()
{
	MStatus stat = MStatus::kSuccess;
	bool visibility = true;
	MDagPath dp = this->dagPath;
	while (stat == MStatus::kSuccess)
	{
		MFnDependencyNode depFn(dp.node());
		MPlug vplug = depFn.findPlug("visibility");
		if(vplug.isConnected())
		{
			Logging::debug(MString("Object: ") + vplug.name() + " has animated visibility");
			return true;
		}
		stat = dp.pop();
	}

	return false;	
}

bool  MayaObject::isObjVisible()
{
	MFnDagNode dagNode(this->mobject);
	if (!IsVisible(dagNode) || IsTemplated(dagNode) || !IsInRenderLayer(this->dagPath) || !IsPathVisible(this->dagPath) || !IsLayerVisible(this->dagPath))
		return false;
	return true;
}


bool MayaObject::geometryShapeSupported()
{
	return true;
}

bool MayaObject::shadowMapCastingLight()
{
	if(!this->mobject.hasFn(MFn::kLight))
		return false;
	
	MFnDependencyNode lightFn(this->mobject);

	bool useDepthMapShadows = false;
	if(!getBool(MString("useDepthMapShadows"), lightFn,  useDepthMapShadows))
		return false;
	
	if(!useDepthMapShadows)
		return false;

	return true;
}

void MayaObject::updateObject()
{
	this->visible = isObjVisible();
}

void MayaObject::initialize()
{
	this->isInstancerObject = false;
	this->instancerParticleId = -1;
	this->instanceNumber = 0;
	this->attributes = nullptr;
	this->origObject = nullptr;
	this->shortName = getObjectName(mobject);
	this->fullName = this->dagPath.fullPathName();
	this->fullNiceName = makeGoodString(this->fullName);
	this->transformMatrices.push_back(this->dagPath.inclusiveMatrix());
	this->instanceNumber = dagPath.instanceNumber();
	MFnDependencyNode depFn(mobject);
	this->motionBlurred = true;
	this->geometryMotionblur = false;
	bool mb = true;
	if( getBool(MString("motionBlur"), depFn, mb) )
		this->motionBlurred = mb;
	// cameras have motionBlur attribute but it is set to false by default and it is not accessible via UI
	// but we want to have a blurred camera by default. 
	if( this->mobject.hasFn(MFn::kCamera))
		this->motionBlurred = true;
	this->perObjectTransformSteps = 1;
	this->perObjectDeformSteps = 1;
	this->index = -1;
	this->shapeConnected = false;
	this->lightExcludeList = true; // In most cases only a few lights are ignored, so the list is shorter with excluded lights
	this->shadowExcludeList = true; // in most cases only a few objects ignore shadows, so the list is shorter with ignoring objects
	this->animated = this->isObjAnimated();
	this->shapeConnected = this->isShapeConnected();
	this->parent = nullptr;
	this->visible = true;
	this->hasInstancerConnection = false;
	MDagPath dp;
	dp = dagPath;
	MFnDagNode dagNode(this->mobject);
	if (!IsVisible(dagNode) || IsTemplated(dagNode) || !IsInRenderLayer(dp) || !IsPathVisible(dp) || !IsLayerVisible(this->dagPath))
		this->visible = false;

	// get instancer connection
	MStatus stat;
	MPlug matrixPlug = depFn.findPlug(MString("matrix"), &stat);
	if( !stat)
		Logging::debug(MString("Could not find matrix plug"));
	else{
		MPlugArray outputs;
		if( matrixPlug.isConnected())
		{
			matrixPlug.connectedTo(outputs, false, true);
			for( uint i = 0; i < outputs.length(); i++)
			{
				MObject otherSide = outputs[i].node();
				Logging::debug(MString("matrix is connected to ") + getObjectName(otherSide));
				if( otherSide.hasFn(MFn::kInstancer))
				{
					Logging::debug(MString("other side is instancer"));
					this->hasInstancerConnection = true;
				}
			}
		}	
	}

	if( this->mobject.hasFn(MFn::kWorld))
	{
		this->shortName = this->fullName = this->fullNiceName = "world";
	}
}

MayaObject::MayaObject(MObject& mobject)
{
	this->mobject = mobject;
	MFnDagNode dagNode(mobject);
	dagNode.getPath(this->dagPath);
	this->initialize();
}

MayaObject::MayaObject(MDagPath& objPath)
{
	this->mobject = objPath.node();
	this->dagPath = objPath;
	this->initialize();
}

// to check if an object is animated, we need to check e.g. its transform inputs
// if the object is from an instancer node, always return true
bool MayaObject::isObjAnimated()
{
	MStatus stat;
	bool returnValue = false;
	
	if( this->instancerParticleId > -1)
		return true;

	if( this->mobject.hasFn(MFn::kTransform))
	{
		MFnDependencyNode depFn(this->mobject, &stat);
		if(stat)
		{
			MPlugArray connections;
			depFn.getConnections(connections);
			if( connections.length() == 0)
				returnValue = false;
			else{
				for( uint cId = 0; cId < connections.length(); cId++)
				{
					if( connections[cId].isDestination())
					{
						returnValue = true;
					}
				}
			}
			
		}
	}
	return returnValue;
}

// to check if shape is connected, simply test for the common shape inputs, nurbs: create mesh: inMesh
bool MayaObject::isShapeConnected()
{
	MStatus stat;
	bool returnValue = false;
	MPlug inPlug;
	MFnDependencyNode depFn(this->mobject, &stat);
	if(stat)
	{
		MFn::Type type = this->mobject.apiType();
		switch(type)
		{
		case MFn::kMesh:
			inPlug = depFn.findPlug(MString("inMesh"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		case MFn::kNurbsSurface:
			inPlug = depFn.findPlug(MString("create"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		case MFn::kNurbsCurve:
			inPlug = depFn.findPlug(MString("create"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		}
	}

	return returnValue;
}

// if we have a mesh and it is a bifrost mesh it may contain velocity vertex data
bool MayaObject::hasBifrostVelocityChannel()
{
	MFnMesh meshFn(this->mobject);
	int numColorSets = meshFn.numColorSets();
	MStringArray colorSetNames;
	meshFn.getColorSetNames(colorSetNames);
	for (uint i = 0; i < colorSetNames.length(); i++)
	{
		if (colorSetNames[i] == "bifrostVelocity")
			return true;
	}
	return false;
}

// get the mesh informations from the current time and save it in the meshDataList
void MayaObject::addMeshData()
{
	MeshData mdata;

	if (this->hasBifrostVelocityChannel())
	{
		bool doMb = this->motionBlurred;
		std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;		
		doMb = doMb && renderGlobals->doMb;

		Logging::debug(MString("Found bifrost velocity data for object: ") + this->shortName);
		if ((this->meshDataList.size() == 2) || !doMb)
		{
			Logging::debug("Bifrost mesh already has two motion steps or mb is turned off for it -> skipping");
			return;
		}

		this->getMeshData(mdata.points, mdata.normals);

		// if we have a bifrost velocity channel I suppose this is a bifost mesh and there is no need to 
		// save the mesh motion steps because it has changing topology what does not work in most renderers
		// so we save only the very first motion step and derive the other steps from velocity channel.
		// and we simply produce two steps only because everything else would not make sense.

		MFnMesh meshFn(this->mobject);
		if (meshFn.hasColorChannels("bifrostVelocity"))
		{
			MColorArray colors;
			MString colorSetName = "bifrostVelocity";
			meshFn.getVertexColors(colors, &colorSetName);

			if (colors.length() == mdata.points.length())
			{
				for (uint ptId = 0; ptId < mdata.points.length(); ptId++)
				{
					MColor c = colors[ptId];
					MVector v = MVector(c.r, c.g, c.b);
					mdata.points[ptId] -= v * 0.5 / 24.0;
				}
				this->meshDataList.push_back(mdata);
				for (uint ptId = 0; ptId < mdata.points.length(); ptId++)
				{
					MColor c = colors[ptId];
					MVector v = MVector(c.r, c.g, c.b);
					mdata.points[ptId] += v * 0.5 / 24.0;
				}
				this->meshDataList.push_back(mdata);
			}
		}else{
			Logging::debug("Bifrost mesh has no velocity data, no motionblur.");
			if (this->meshDataList.size() == 0)
				this->meshDataList.push_back(mdata);
		}
	}
	else{
		this->getMeshData(mdata.points, mdata.normals);
		int np = mdata.points.length();
		this->meshDataList.push_back(mdata);

		for (auto md : this->meshDataList)
		{
			np = md.points.length();
		}
	}

}

void MayaObject::getMeshData(MPointArray& points, MFloatVectorArray& normals)
{
	MStatus stat;
	MObject meshObject = this->mobject;
	MMeshSmoothOptions options;
	MFnMesh tmpMesh(this->mobject);
	MFnMeshData meshData;
	MObject dataObject;
	MObject smoothedObj;

	// create smooth mesh if needed
	if (tmpMesh.findPlug("displaySmoothMesh").asBool())
	{
		stat = tmpMesh.getSmoothMeshDisplayOptions(options);
		if (stat)
		{
			if (!tmpMesh.findPlug("useSmoothPreviewForRender", false, &stat).asBool())
			{
				//Logging::debug(MString("useSmoothPreviewForRender turned off"));
				int smoothLevel = tmpMesh.findPlug("renderSmoothLevel", false, &stat).asInt();
				options.setDivisions(smoothLevel);
			}
			if (options.divisions() > 0)
			{
				dataObject = meshData.create();
				smoothedObj = tmpMesh.generateSmoothMesh(dataObject, &options, &stat);
				if (stat)
				{
					meshObject = smoothedObj;
				}
			}
		}
	}
	MFnMesh meshFn(meshObject, &stat);
	if (!stat)
	{
		MString error = stat.errorString();
		Logging::error(error);
	}
	meshFn.getPoints(points);
	meshFn.getNormals(normals, MSpace::kObject);
}

void MayaObject::getMeshData(MPointArray& points, MFloatVectorArray& normals, MFloatArray& uArray, MFloatArray& vArray, MIntArray& triPointIndices, MIntArray& triNormalIndices, MIntArray& triUvIndices, MIntArray& triMatIndices)
{

	MStatus stat;
	MObject meshObject = this->mobject;
	MMeshSmoothOptions options;
	MFnMesh tmpMesh(this->mobject, &stat);

	MFnMeshData meshData;
	MObject dataObject;
	MObject smoothedObj;

	// create smooth mesh if needed
	if (tmpMesh.findPlug("displaySmoothMesh").asBool())
	{
		stat = tmpMesh.getSmoothMeshDisplayOptions(options);
		if (stat)
		{
			if (!tmpMesh.findPlug("useSmoothPreviewForRender", false, &stat).asBool())
			{
				//Logging::debug(MString("useSmoothPreviewForRender turned off"));
				int smoothLevel = tmpMesh.findPlug("renderSmoothLevel", false, &stat).asInt();
				options.setDivisions(smoothLevel);
			}
			if (options.divisions() > 0)
			{
				dataObject = meshData.create();
				smoothedObj = tmpMesh.generateSmoothMesh(dataObject, &options, &stat);
				if (stat)
				{
					meshObject = smoothedObj;
				}
			}
		}
	}

	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);
	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);

	meshFn.getPoints(points);
	meshFn.getNormals(normals, MSpace::kObject);
	meshFn.getUVs(uArray, vArray);

	uint numVertices = points.length();
	uint numNormals = normals.length();
	uint numUvs = uArray.length();

	//Logging::debug(MString("numVertices ") + numVertices);
	//Logging::debug(MString("numNormals ") + numNormals);
	//Logging::debug(MString("numUvs ") + numUvs);

	// some meshes may have no uv's
	// to avoid problems I add a default uv coordinate
	if (numUvs == 0)
	{
		Logging::warning(MString("Object has no uv's: ") + this->shortName);
		uArray.append(0.0);
		vArray.append(0.0);
	}
	for (uint nid = 0; nid < numNormals; nid++)
	{
		if (normals[nid].length() < 0.1f)
			Logging::warning(MString("Malformed normal in ") + this->shortName);
	}
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;

	for (faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		int faceId = faceIt.index();
		int numTris;
		faceIt.numTriangles(numTris);
		faceIt.getVertices(faceVtxIds);

		int perFaceShadingGroup = 0;
		if (this->perFaceAssignments.length() > 0)
			perFaceShadingGroup = this->perFaceAssignments[faceId];
		
		MIntArray faceUVIndices;
		
		faceNormalIds.clear();
		for (uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
		{
			faceNormalIds.append(faceIt.normalIndex(vtxId));
			int uvIndex;
			if (numUvs == 0)
			{
				faceUVIndices.append(0);
			}
			else{
				faceIt.getUVIndex(vtxId, uvIndex);
				//if (uvIndex > uArray.length())
				//	Logging::info(MString("-----------------> UV Problem!!! uvIndex ") + uvIndex + " > uvArray in object " + this->shortName);
				faceUVIndices.append(uvIndex);
			}
		}

		for (int triId = 0; triId < numTris; triId++)
		{
			int faceRelIds[3];
			faceIt.getTriangle(triId, triPoints, triVtxIds);

			for (uint triVtxId = 0; triVtxId < 3; triVtxId++)
			{
				for (uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
				{
					if (faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
					{
						faceRelIds[triVtxId] = faceVtxId;
					}
				}
			}

			uint vtxId0 = faceVtxIds[faceRelIds[0]];
			uint vtxId1 = faceVtxIds[faceRelIds[1]];
			uint vtxId2 = faceVtxIds[faceRelIds[2]];
			uint normalId0 = faceNormalIds[faceRelIds[0]];
			uint normalId1 = faceNormalIds[faceRelIds[1]];
			uint normalId2 = faceNormalIds[faceRelIds[2]];
			uint uvId0 = faceUVIndices[faceRelIds[0]];
			uint uvId1 = faceUVIndices[faceRelIds[1]];
			uint uvId2 = faceUVIndices[faceRelIds[2]];

			triPointIndices.append(vtxId0);
			triPointIndices.append(vtxId1);
			triPointIndices.append(vtxId2);

			triNormalIndices.append(normalId0);
			triNormalIndices.append(normalId1);
			triNormalIndices.append(normalId2);

			triUvIndices.append(uvId0);
			triUvIndices.append(uvId1);
			triUvIndices.append(uvId2);

			triMatIndices.append(perFaceShadingGroup);

			//Logging::debug(MString("vtxIds ") + vtxId0 + " " + vtxId1 + " " + vtxId2);
			//Logging::debug(MString("nIds ") + normalId0 + " " + normalId1 + " " + normalId2);
			//Logging::debug(MString("uvIds ") + uvId0 + " " + uvId1 + " " + uvId2);
		}
	}

}

MayaObject::~MayaObject()
{}
