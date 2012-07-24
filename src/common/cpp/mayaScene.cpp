#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnInstancer.h>
#include <maya/MMatrixArray.h>
#include <maya/MIntArray.h>
#include <maya/MLightLinks.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MRenderView.h>

#include "mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

static Logging logger;

MayaScene::MayaScene()
{
	this->good = true;
}

MayaScene::~MayaScene()
{}

// Here is checked if the linklist is complete, what means that we do not have to do any 
// complicated light linking, but default procedures. If excludedLights is > 0 then we found 
// any excluded Lights.
bool MayaScene::listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights)
{
	excludedLights.clear();	
	for( uint lId = 0; lId < this->lightList.size(); lId++)
	{
		MDagPath path = this->lightList[lId]->dagPath;
		bool found = false;
		for( uint liId = 0; liId < linkedLights.length(); liId++)
		{
			MDagPath linkedPath = linkedLights[liId];
			if( linkedPath == path )
			{
				found = true;
				break;
			}
		}		
		if( found )
			continue;
		else
			excludedLights.append(path);
	}
	
	if( excludedLights.length() > 0)
		return false;

	return true;
}

// there are two types of linking, light linking and shadow linking.
// Light linking will be done in the object attributes, whereas shadow linking is an attribute of a light
void MayaScene::getLightLinking()
{
	//logger.debug(MString("----------- MayaScene::getLightLinking ---------------"));
	MLightLinks lightLink;
    bool parseStatus;
    //parseStatus = lightLink.parseLinks(MObject::kNullObj, useIgnore = true);
    parseStatus = lightLink.parseLinks(MObject::kNullObj);
	MDagPathArray lightArray, excludeLights;
	MObject nullObj;
	std::vector<MayaObject *> castNoShadowObjects;

	for( int oId = 0; oId < this->objectList.size(); oId++)
	{
		MayaObject *mo = this->objectList[oId];
		MDagPath path = mo->dagPath;
		// the ignored list is more useful because if we have no ignored lights the list is empty
		// at the moment this does not work for lights with "illuminates by default" turned off, so
		// I use the linkedLights which works and search for excluded ones
		mo->linkedLights.clear();
		//lightLink.getIgnoredLights(path, nullObj, lightArray); 	
		//for( int ll = 0; ll < lightArray.length(); ll++)
		//{
		//	MDagPath lpath = lightArray[ll];
		//	logger.debug(lpath.partialPathName() + " is ignored by " + path.partialPathName());
		//	mo->linkedLights.push_back(lpath);
		//}

		lightLink.getLinkedLights(path, nullObj, lightArray); 	
		this->listContainsAllLights(lightArray, excludeLights);
		//logger.debug(MString("Exclude list contains: ") + excludeLights.length() + " lights");
		for( uint ll = 0; ll < excludeLights.length(); ll++)
		{
			MDagPath lpath = excludeLights[ll];
			//logger.debug(lpath.partialPathName() + " is ignored by " + path.partialPathName());
			mo->linkedLights.push_back(lpath);
		}

		bool castsShadows = true;
		MFnDependencyNode dn(mo->mobject);
		if( getBool(MString("castsShadows"), dn, castsShadows))
		{
			if( !castsShadows )
				castNoShadowObjects.push_back(mo);
		}
		// leave for later implementation: Compare include and exclude list and create the shorter one if necessary
		// not sure if this is useful at all...
		//if( !this->listContainsAllLights(lightArray, excludeLights) )
		//{
		//	for( int ll = 0; ll < lightArray.length(); ll++)
		//	{
		//		this->objectList[oId]->linkedLights.push_back(lightArray[ll]);
		//	}
		//}else{
		//	logger.debug(path.partialPathName() + " is connected to all lights.");
		//}
	}

	// shadow linking. Get objects which ignores shadows from a specific light
	MSelectionList selectionList;
	for( uint lId = 0; lId < this->lightList.size(); lId++)
	{
		if( castNoShadowObjects.size() > 0)
		{
			for( uint oId = 0; oId < castNoShadowObjects.size(); oId++)
			{
				this->lightList[lId]->castNoShadowObjects.push_back(castNoShadowObjects[oId]->dagPath);
			}
		}
		//MayaObject *mo = this->lightList[lId];
		//MDagPath path = mo->dagPath;
		//selectionList.clear();
		//lightLink.getShadowIgnoredObjects(path, selectionList);
		//int numObj = selectionList.length();
		//for( int oId = 0; oId < numObj; oId++)
		//{
		//	MDagPath obDag;
		//	selectionList.getDagPath(oId, obDag);
		//	logger.debug(MString("Object ") +  obDag.partialPathName() + " ignores shadows from " + path.partialPathName());
		//	mo->shadowObjects.push_back(obDag);
		//}
	}

	// search for shadow ignoring objects
	for( uint lId = 0; lId < this->lightList.size(); lId++)
	{
		MayaObject *mo = this->lightList[lId];
		MDagPath path = mo->dagPath;
		selectionList.clear();
		lightLink.getShadowIgnoredObjects(path, selectionList);
		int numObj = selectionList.length();
		for( int oId = 0; oId < numObj; oId++)
		{
			MDagPath obDag;
			selectionList.getDagPath(oId, obDag);
			//logger.debug(MString("Object ") +  obDag.partialPathName() + " ignores shadows from " + path.partialPathName());
			mo->shadowObjects.push_back(obDag);
		}
	}
}

//bool MayaScene::translateShaders()
//{
//	return true;
//}
//
//bool MayaScene::translateShapes()
//{
//	return true;
//}

bool MayaScene::parseScene()
{
	logger.debug(MString("parseScene"));
	
	instancerDagPathList.clear();

	MItDag   dagIterator(MItDag::kDepthFirst, MFn::kInvalid);
	MDagPath dagPath;
	
	for (dagIterator.reset(); (!dagIterator.isDone()); dagIterator.next())
	{
		if (!dagIterator.getPath(dagPath))
		{
			logger.error(MString("parseScene ERROR: Could not get path for DAG iterator."));
			return false;
		}
		logger.debug(MString("Parse Object: ") + getObjectName(dagPath.node()));		
		
		MFnDagNode node(dagPath.node());
		MObject obj = node.object();

		// here only base objects, instances will be exported later directly
		int instanceNumber = dagPath.instanceNumber();

		if( instanceNumber > 0 )
			continue;

		if (dagPath.apiType() == MFn::kWorld)
			continue;

		if (obj.hasFn(MFn::kCamera))
		{
			MFnCamera cam(obj);
			bool renderable = false;
			
			// ignore all cameras that are not renderable
			if(!getBool(MString("renderable"), cam, renderable))
				continue;

			if( renderable )
			{
				MayaObject *mo = this->mayaObjectCreator(obj);
				mo->visible = true;
				mo->scenePtr = this;
				mo->instanceNumber = 0;
				this->camList.push_back(mo);
				mo->index = (int)(this->camList.size() - 1);
				continue;
			}		
		}

		if (obj.hasFn(MFn::kLight))
		{

			if (!IsVisible(node) || IsTemplated(node) || !IsInRenderLayer(dagPath) || !IsPathVisible(dagPath))
				continue;
			else{
				MayaObject *mo = this->mayaObjectCreator(obj);
				mo->visible = true;
				mo->instanceNumber = 0;
				mo->scenePtr = this;
				//mo->findObject = &this->getObject;
				this->lightList.push_back(mo);
				mo->index = (int)(this->lightList.size() - 1);
				continue;
			}
		}

		if (dagPath.apiType() == MFn::kInstancer)
		{
			this->instancerDagPathList.push_back(dagPath);
		}
		
		MFnDependencyNode depFn(obj);
		uint nodeId = depFn.typeId().id();
		for( uint lId = 0; lId < this->lightIdentifier.size(); lId++)
		{
			bool nodeFound = false;
			if( nodeId == this->lightIdentifier[lId])
			{
				nodeFound = true;
				logger.debug("Found external light node.");
				if (!IsVisible(node) || IsTemplated(node) || !IsInRenderLayer(dagPath) || !IsPathVisible(dagPath))
				{	
					continue;
				}else{
					MayaObject *mo = this->mayaObjectCreator(obj);
					mo->visible = true;
					mo->instanceNumber = 0;
					mo->scenePtr = this;
					//mo->findObject = &this->getObject;
					this->lightList.push_back(mo);
					mo->index = (int)(this->lightList.size() - 1);
					continue;
				}
			}
			if( nodeFound )
				continue;
		}
		
		bool visible = true;
		if (!IsVisible(node) || IsTemplated(node) || !IsInRenderLayer(dagPath) || !IsPathVisible(dagPath))
		{			
			int numi = dagIterator.instanceCount(true);
			if( numi > 1 )
			{
				logger.info(MString("Object ") + node.name() + " has instances but is not visible. Set it to invisble but export geo");
				
				// okay I am an original object of an instance, now check if any of my instanced objects is visible				
				MDagPathArray instPathArray;
				dagIterator.getAllPaths(instPathArray);
				bool visibleInstFound = false;
				for( uint iId = 0; iId < instPathArray.length(); iId++)
				{
					MFnDagNode inode(instPathArray[iId].node()); 
					if (IsVisible(inode) || !IsTemplated(inode) || IsInRenderLayer(instPathArray[iId]) || IsPathVisible(instPathArray[iId]))
					{
						logger.info(MString("Found a visible instance for object: ") + inode.name());
						visibleInstFound = true;
						break;
					}
				}
				if( !visibleInstFound)
					continue;
				visible = false;				
			}else{
				continue;
			}
		}
		
		MayaObject *mo = this->mayaObjectCreator(node.object());
		mo->scenePtr = this;
		//mo->findObject = &this->getObject;
		mo->visible = visible;
		mo->instanceNumber = instanceNumber;
		this->objectList.push_back(mo);
		mo->index = (int)(this->objectList.size() - 1);
	}

	int numobjects = (int)this->objectList.size();
	if( numobjects == 0)
	{
		logger.error(MString("Scene parse: No renderable object found"));
		this->good = false;
		return false;
	}

	// get instances
	// the idea is, simply to copy the original object and set the necessary values only.
	// I have to access the original MayaObject to be able to get the HierarchyNameList for mb data.

	int numObjects = (int)this->objectList.size();
	for( int objId = 0; objId < numobjects; objId++)
	{
		MDagPathArray instArray;
		MayaObject *mo = this->objectList[objId];
		mo->dagPath.getAllPathsTo(mo->mobject, instArray);
		int numPaths = instArray.length();
		if( numPaths > 1)
		{
			logger.debug( MString("Found ") + (numPaths - 1) + " instances for " + mo->shortName);
			for( uint iId = 1; iId < instArray.length(); iId++)
			{
				MFnDagNode node(instArray[iId].node());
				MDagPath dagPath = instArray[iId];
				//logger.debug(MString("Inst node: ") + node.name());
				//logger.debug(MString("Full path name: ") + instArray[iId].fullPathName());
				MMatrix m0 = instArray[iId].inclusiveMatrix();

				if (!IsVisible(node) || IsTemplated(node) || !IsInRenderLayer(instArray[iId]) || !IsPathVisible(instArray[iId]))
					continue;
				MayaObject *newObj = this->mayaObjectCreator(node.object());
				*newObj = *mo;
				//logger.debug(MString("new Object Full path name: ") + newObj->dagPath.fullPathName());
				//logger.debug(MString("new instArray[iId] path name: ") + dagPath.fullPathName());
				newObj->dagPath = dagPath;
				newObj->visible = true;
				newObj->supported = true;
				newObj->origObject = mo;
				newObj->instanceNumber = iId;
				newObj->transformMatrices.clear();
				//MMatrix m = newObj->dagPath.inclusiveMatrix();
				newObj->transformMatrices.push_back(newObj->dagPath.inclusiveMatrix());
				//MObject parent = node.parent(0);
				//MFnDagNode pNode(parent);
				//logger.debug(MString("Full parent path name: ") + pNode.fullPathName());
				//newObj->transformMatrices.push_back(pNode.dagPath().inclusiveMatrix());
				//MMatrix m1 = pNode.dagPath().inclusiveMatrix();
				
				this->objectList.push_back(newObj);
				newObj->index = (int)(this->objectList.size() - 1);
			}
		}
	}
	
	this->parseInstancer(); 

	this->getLightLinking();

	if( MGlobal::mayaState() != MGlobal::kBatch )
	{
		// clear cameras and use only the active one
		this->clearObjList(this->camList);
		// if we are in UI rendering state, try to get the current camera
		M3dView curView = M3dView::active3dView();
		MDagPath camDagPath;
		curView.getCamera( camDagPath );
		MayaObject *mo = this->mayaObjectCreator(camDagPath.node());
		mo->visible = true;
		mo->scenePtr = this;
		mo->instanceNumber = 0;
		this->camList.push_back(mo);
		mo->index = (int)(this->camList.size() - 1);
	}

	this->good = true;
	return true;

}


bool MayaScene::updateScene()
{
	
	if(this->renderGlobals->currentMbElement.elementType == MbElement::XForm )
		logger.debug(MString("MayaScene::updateScene for XForm step"));
	if(this->renderGlobals->currentMbElement.elementType == MbElement::Geo )
		logger.debug(MString("MayaScene::updateScene for Deform step"));
	if(this->renderGlobals->currentMbElement.elementType == MbElement::Both )
		logger.debug(MString("MayaScene::updateScene for Deform and XForm step"));

	int numElements = (int)this->objectList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		MayaObject *obj = (MayaObject *)this->objectList[objId];

		// check if the geometry shape is supported
		if( !obj->geometryShapeSupported() )
			continue;

		// if this is not the very first time step of a rendering
		// we must be in a motionblur step
		// isMbStartStep will be always true in an animation without motionblur
		if( !this->renderGlobals->isMbStartStep )
		{
			if( !obj->motionBlurred )
			{
				continue;
			}
		}
				
		// clear transformation matrices if we are at very first mb step
		// we can have xform and deform steps for the same timeframe
		// currentMbStep is only 0 for the very first step
		if( this->renderGlobals->isMbStartStep && this->renderGlobals->isTransformStep())
		{
			obj->transformMatrices.clear();			
		}

		if( this->renderGlobals->isTransformStep() )
		{
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
			this->transformUpdateCallback(*obj);
			//continue;
		}

		// if we are here at the very first motionBlur step, then remove export filesNames, export filenames will be automatically filled by the exporter
		// at start frame, always clear
		// not at start frame only clear if shapedeformation is detected
		if( this->renderGlobals->isMbStartStep )
		{
			if( this->renderGlobals->currentFrameNumber != this->renderGlobals->startFrame )
			{
				if( obj->shapeConnected && this->renderGlobals->detectShapeDeform)
				{
					obj->exportFileNames.clear();
				}
			}else{
				obj->exportFileNames.clear();
			}
		}
		
		if( this->renderGlobals->isMbStartStep )
		{
			if(this->renderGlobals->isTransformStep())
			{
			}
			if(this->renderGlobals->isDeformStep())
			{
				if( (!obj->isInstancerObject) && 
					( obj->instanceNumber == 0))
				{
					if( obj->exportFileNames.size() == 0)
					{
						this->deformUpdateCallback(*obj);
					}
				}
			}
		}else{
			// only export if shape has deformation, of course only for original shapes
			if(obj->shapeConnected && this->renderGlobals->detectShapeDeform && (!obj->isInstancerObject) && (obj->instanceNumber == 0))
			{
				// during motion step
				if( this->renderGlobals->isDeformStep() )
				{
					// in case of geometryMotionblur, we need only the first export filename, no additional steps
					if(obj->geometryMotionblur )
						continue;
					logger.debug(MString("Deforming object not at start frame -> exporting ") + obj->shortName);
					this->deformUpdateCallback(*obj);
				}
			}
		}
	}

	numElements = (int)this->camList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		MayaObject *obj = (MayaObject *)this->camList[objId];
		if( this->renderGlobals->currentMbStep == 0)
			obj->transformMatrices.clear();

		if( this->renderGlobals->isTransformStep() )
		{
			logger.debug(MString("Adding cam transform at time ") + this->renderGlobals->currentFrameNumber);
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
			this->transformUpdateCallback(*obj);
		}
		if( obj->transformMatrices.size() > 1)
		{
			MMatrix m1 = obj->transformMatrices[0];
			MMatrix m2 = obj->transformMatrices[1];
			logger.debug(MString("CamMatrix1: t: ") + m1[3][0] + " " + m1[3][1] + " " + m1[3][2]);
			logger.debug(MString("CamMatrix2: t: ") + m2[3][0] + " " + m2[3][1] + " " + m2[3][2]);
		}
	}

	numElements = (int)this->lightList.size();
	for( int objId = 0; objId < numElements; objId++)
	{
		MayaObject *obj = (MayaObject *)this->lightList[objId];
		if( this->renderGlobals->currentMbStep == 0 )
			obj->transformMatrices.clear();

		if( this->renderGlobals->isTransformStep() )
		{
			obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
			this->transformUpdateCallback(*obj);
		}
	}

	// Recreate the instancer array for every frame. If we are in a motion blur step, update it.
	// With no motionblur, clear the array and re-parse the instancers. This is necessary because the contents of the instancer
	// can vary from frame to frame e.g. if instances disappear with the death of a particle
	if( !this->renderGlobals->doMb )
	{
		this->clearInstancerNodeList();
		this->parseInstancer();
	}else{
		this->updateInstancer();
	}

	return true;
}

void MayaScene::clearObjList(std::vector<MayaObject *>& objList)
{
	size_t numElements = objList.size();
	for( size_t i = 0; i < numElements; i++)
		this->mayaObjectDeleter(objList[i]);
	objList.clear();
}

void MayaScene::clearInstancerNodeList()
{
	size_t numElements = this->instancerNodeElements.size();
	for( size_t i = 0; i < numElements; i++)
		this->mayaObjectDeleter(this->instancerNodeElements[i]);

	this->instancerNodeElements.clear();
}

bool MayaScene::updateInstancer()
{
	size_t numElements = this->instancerNodeElements.size();
	for( size_t i = 0; i < numElements; i++)
	{
		MayaObject *obj =  this->instancerNodeElements[i];
		MFnInstancer instFn(obj->instancerMObj);
		MDagPathArray dagPathArray;
		MMatrix matrix;
		instFn.instancesForParticle(obj->instancerParticleId, dagPathArray, matrix); 
		// get matrix from current path?
		obj->transformMatrices.push_back(matrix);
	}
	return true;
}

std::vector<MayaObject *> InstDoneList;
// parsing all particle instancer nodes in the scene
// here I put all nodes created by an instancer node into a array of its own.
// The reason why I don't simply add it to the main list is that I have to recreate them
// during sceneUpdate() for every frame because the list can change with the birth or death 
// of a particle
bool MayaScene::parseInstancer()
{
	bool result = true;

	InstDoneList.clear();

	MDagPath dagPath;
	int numInstancers = (int)instancerDagPathList.size();
	int numobjects = (int)this->objectList.size();

	for(int iId = 0; iId < numInstancers; iId++)
	{
		MDagPath instPath = instancerDagPathList[iId];
		MObject instancerMObject = instPath.node();
		MString path = instPath.fullPathName();
		MFnInstancer instFn(instPath);
		int numParticles = instFn.particleCount();
		logger.debug(MString("Detected instancer. instPath: ") + path + " it has " + numParticles + " particle instances");
		MDagPathArray allPaths;
		MMatrixArray allMatrices;
		MIntArray pathIndices;
		MIntArray pathStartIndices;
		
		// give me all instances in this instancer
		instFn.allInstances( allPaths, allMatrices, pathStartIndices, pathIndices );

		for( int p = 0; p < numParticles; p++ )
		{
			MMatrix particleMatrix = allMatrices[p];

			//  the number of paths instanced under a particle is computed by
			//  taking the difference between the starting path index for this
			//  particle and that of the next particle.  The size of the start
			//  index array is always one larger than the number of particles.
			//
			int numPaths = pathStartIndices[p+1]-pathStartIndices[p];
	        
			//  the values pathIndices[pathStart...pathStart+numPaths] give the
			//  indices in the allPaths array of the paths instanced under this
			//  particular particle.  Remember, different paths can be instanced
			//  under each particle.
			//
			int pathStart = pathStartIndices[p];

			//  loop through the instanced paths for this particle
			//
			MayaObject *mo = NULL;
			MayaObject *origObj = NULL;
			for( int i = pathStart; i < pathStart+numPaths; i++ )
			{
				int curPathIndex = pathIndices[i];
				MDagPath curPath = allPaths[curPathIndex];
				
				// search for the correct orig MayaObject element
				// first search the already done list, this could be faster
				int numObj = (int)InstDoneList.size();
				bool found = false;
				MDagPath dp = curPath;
				MObject thisObject = curPath.node();
				MObject foundObject = MObject::kNullObj;
				for( int oId = 0; oId < numObj; oId++)
				{
					MObject doneObj = InstDoneList[oId]->mobject;
					if( doneObj == thisObject)
					{
						logger.debug("Found mobject in instancer done list");
						origObj = InstDoneList[oId];
						found = true;
						break;
					}
				}
				// well it is not in the done list, so search the whole obj list for instances
				if( !found)
				{
					int foundId = -1;
					for( int oId = 0; oId < numobjects; oId++)
					{
						mo = this->objectList[oId];
						MObject doneObj = mo->mobject;
						if( doneObj == thisObject)
						{
							logger.debug("Found mobject in all list");
							found = true;
							origObj = mo;
							InstDoneList.push_back(mo);
							break;
						}
					}
				}
				// TODO: visibiliy check - necessary?
				MMatrix instancedPathMatrix = curPath.inclusiveMatrix();
				MMatrix finalMatrixForPath = instancedPathMatrix * particleMatrix;
				MPoint finalPoint = MPoint::origin * finalMatrixForPath;
				MDagPath cp = curPath;
				MayaObject *newObj = this->mayaObjectCreator(thisObject);
				*newObj = *origObj;
				newObj->dagPath = cp;
				newObj->origObject = origObj;
				newObj->isInstancerObject = true;
				newObj->instancerMatrix = finalMatrixForPath;
				newObj->transformMatrices.clear();
				newObj->transformMatrices.push_back(finalMatrixForPath);
				newObj->supported = true;
				newObj->visible = true;
				newObj->instancerParticleId = p;
				newObj->instanceNumber = p;
				newObj->instancerMObj = instancerMObject;
				newObj->fullName = origObj->fullName + MString("_i_") + p;				
				newObj->shortName = origObj->shortName + MString("_i_") + p;
				this->instancerNodeElements.push_back(newObj);
				//this->objectList.push_back(newObj);
				newObj->index = (int)(this->instancerNodeElements.size() - 1);
	                
				MString instancedPathName = curPath.fullPathName();
				//logger.info(instancedPathName + " pos " + finalPoint.x + " "  + finalPoint.y + " "  + finalPoint.z);

			}
		}
	}
	return true;
}

bool MayaScene::getShadingGroups()
{
	size_t numobjects = this->objectList.size();
	for( size_t objId = 0; objId < numobjects; objId++)
	{
		MayaObject *mo = this->objectList[objId];
		MObject sg;
		if(getObjectShadingGroups(mo->dagPath, sg))
			mo->material = Material(sg);
	}
	numobjects = this->instancerNodeElements.size();
	for( uint objId = 0; objId < numobjects; objId++)
	{
		MayaObject *mo = this->instancerNodeElements[objId];
		MObject sg;
		if(getObjectShadingGroups(mo->dagPath, sg))
			mo->material = Material(sg);
	}
	numobjects = this->lightList.size();
	for( uint objId = 0; objId < numobjects; objId++)
	{
		MayaObject *mo = this->lightList[objId];
		MObject sg = mo->mobject;
		mo->material = Material(sg);
	}
	return true;
}

void MayaScene::getPasses()
{
	// clear
	for( size_t renderPassId = 0; renderPassId < this->renderGlobals->renderPasses.size(); renderPassId++)
		delete this->renderGlobals->renderPasses[renderPassId];
	this->renderGlobals->renderPasses.clear();

	// add pass for lights for shadow maps 

	// add pass for lights with photon maps

	// finally do the normal beauty passes
	RenderPass *rp = new RenderPass();
	rp->evalFrequency = RenderPass::OncePerFrame;
	rp->passType = RenderPass::Beauty;

	int numcams = (int)this->camList.size();
	for( int objId = 0; objId < numcams; objId++)
	{
		MayaObject *obj = this->camList[objId];
		rp->objectList.push_back(obj);
	}	
	
	this->renderGlobals->renderPasses.push_back(rp);
}


bool MayaScene::doFrameJobs()
{
	logger.debug("MayaScene::doFrameJobs()");
	logger.progress(MString("\n========== Start rendering of frame ") + this->currentFrame + " ==============\n");

	size_t numPasses = this->renderGlobals->renderPasses.size();
	for( size_t passId = 0; passId < numPasses; passId++)
	{
		// Get the objects from the currentPass object list. 
		// In case of a shadow map generation it is a light,
		// in case of a beauty its a camera.
		this->renderGlobals->currentRenderPass = this->renderGlobals->renderPasses[passId];
		size_t numPassElements = this->renderGlobals->currentRenderPass->objectList.size();
		for( size_t passElementId = 0; passElementId < numPassElements; passElementId++)
		{
		
			this->renderGlobals->currentRenderPassElementId = passElementId;
			if( this->renderGlobals->currentRenderPass->passType == RenderPass::Beauty)
			{
				MayaObject *camera = (MayaObject *)this->renderGlobals->currentRenderPass->objectList[passElementId];
				this->renderGlobals->getMbSteps(camera->mobject);
			}

			if(this->renderGlobals->mbElementList.size() == 0)
			{
				logger.error(MString("no mb steps, somethings wrong."));
				return false;
			}

			int numMbSteps = (int)this->renderGlobals->mbElementList.size();
			logger.debug(MString("mbSteps: ") + numMbSteps);
			this->renderGlobals->currentMbStep = 0;

			for( int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
			{
				this->renderGlobals->isMbStartStep = this->renderGlobals->mbElementList[mbStepId].time == this->renderGlobals->mbElementList[0].time;
				this->renderGlobals->currentMbElement = this->renderGlobals->mbElementList[mbStepId];
				this->renderGlobals->currentFrameNumber = (float)(this->renderGlobals->currentFrame + this->renderGlobals->mbElementList[mbStepId].time);
				bool needView = true;
				if( mbStepId > 0)
				{
					if( this->renderGlobals->mbElementList[mbStepId].time != this->renderGlobals->mbElementList[mbStepId-1].time)
					{
						needView = true;
					}else{
						needView = false;
					}
				}
				if( needView )
				{
					MayaObject *camera = (MayaObject *)this->renderGlobals->currentRenderPass->objectList[passElementId];
					logger.debug(MString("doFrameJobs() viewFrame: ") + this->renderGlobals->currentFrameNumber);
					if( MGlobal::mayaState() != MGlobal::kBatch )
					{
						if( MRenderView::setCurrentCamera( camera->dagPath ) != MS::kSuccess )
						{
							logger.error("renderViewRender: error occurred in setCurrentCamera." );
							return false;
						}
					}
					MGlobal::viewFrame(this->renderGlobals->currentFrameNumber);
				}

				// TODO: runup necessary?

				this->updateScene();
				logger.info(MString("update scene done"));
				this->renderGlobals->currentMbStep++;
			}
			this->renderImage();
		}		
	}

	// if we render from UI jump back to full frame after rendering
	if( MGlobal::mayaState() != MGlobal::kBatch )
		MGlobal::viewFrame(this->renderGlobals->currentFrame);

	return true;
}


bool MayaScene::renderScene()
{
	logger.info("MayaScene::renderScene()");
	this->getPasses();

	if(!this->doPreRenderJobs())
	{
		logger.error("doPreRenderJobs failed.");
		return false;
	}

	int numFrames = (int)this->renderGlobals->frameList.size();
	for( int frameNr = 0; frameNr < numFrames; frameNr++)
	{
		this->currentFrame = this->renderGlobals->frameList[frameNr];
		this->renderGlobals->currentFrame = this->currentFrame;
		if(!this->doPreFrameJobs())
		{
			logger.error("doPreFrameJobs failed.");
			return false;
		}
		if(!this->doFrameJobs())
		{
			logger.error("doFrameJobs failed.");
			return false;
		}

		if(!this->doPostFrameJobs())
		{
			logger.error("doPostFrameJobs failed.");
			return false;
		}
		logger.progress(MString("\n========== Rendering of frame ") + this->currentFrame + " done ==============\n");
	}

	if(!this->doPostRenderJobs())
	{
		logger.error("doPostRenderJobs failed.");
		return false;
	}


	return true;
}

MayaObject *MayaScene::getObject(MObject obj)
{
	MayaObject *mo = NULL;
	size_t numobjects = this->objectList.size();
	for( size_t objId = 0; objId < numobjects; objId++)
	{
		if( this->objectList[objId]->mobject == obj)
			return this->objectList[objId];
	}
	return mo;
}

MayaObject *MayaScene::getObject(MDagPath dp)
{
	MayaObject *mo = NULL;
	size_t numobjects = this->objectList.size();
	for( size_t objId = 0; objId < numobjects; objId++)
	{
		if( this->objectList[objId]->dagPath == dp)
			return this->objectList[objId];
	}
	return mo;
}