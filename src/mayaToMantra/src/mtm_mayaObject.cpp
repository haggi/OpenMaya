#include <maya/MPlug.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnCamera.h>
#include <maya/MLightLinks.h>
#include <maya/MSelectionList.h>

#include "mtm_mayaObject.h"
#include "mtm_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "exporter/mtm_meshExporter.h"
#include "exporter/mtm_nurbsExporter.h"
#include "exporter/mtm_nurbsCurveExporter.h"
#include "exporter/mtm_particleExporter.h"
#include "exporter/mtm_fluidExporter.h"
#include "exporter/mtm_standinExporter.h"
#include "shadingTools/mtm_material.h"


static Logging logger;

bool mtm_MayaObject::geometryShapeSupported()
{
	MFn::Type type = this->mobject.apiType();
	if( type == MFn::kMesh ||
		type == MFn::kNurbsCurve ||
		type == MFn::kNurbsSurface ||
		type == MFn::kParticle ||
		type == MFn::kNParticle ||
		type == MFn::kFluid)
	{
		return true;
	}

	MFnDependencyNode depFn(this->mobject);
	int type_id = depFn.typeId().id();
	if( type_id == 0x106EF2)
		return true;

	return false;
}

bool mtm_MayaObject::exportShape(int timeStep, bool binary, MString& basePath, bool useShortName)
{
	bool result = true;
	MString info;
	MStatus stat;

	MString ext = ".bgeo";
	// binary - True ==> bgeo, if false ==> geo
	if( !binary )
		ext = ".geo";
	// if useShortName is selected, then the file will be called with shortName + arrayindexnumber
	// something like ball85. The full name will use the complete path as filename.
	// e.g. |topnode|branch|leaf|ball ==> _topnode_branch_leaf_ball.geo this can lead to quite
	// long names if you have a deep hierarchy
	MString nodeName = this->fullName;
	if( useShortName )
		nodeName = MString(this->shortName) + this->index;

	logger.progress(MString("MayaObject::exportShape ") + nodeName + " at timestep " + timeStep);
	MString exportFilename = basePath + "/geo/" + nodeName + "." + timeStep + ext;
	logger.debug(MString("exporting to ") + exportFilename);

	MFnDependencyNode depFn(this->mobject);
	int type_id = depFn.typeId().id();

	MFn::Type type = this->mobject.apiType();
	switch(type)
	{
	case MFn::kMesh:
		{
			logger.info(MString("Mesh detected.") + this->shortName);
			MFnMesh meshFn(this->mobject);
			MPlug inMeshPlug = meshFn.findPlug(MString("inMesh"), &stat);
			if( stat != MS::kSuccess)
			{
				logger.error("Cannot find plug inMesh.");
				this->shapeConnected = true;
			}else{
				logger.debug("Found plug inMesh.");
				if( inMeshPlug.isConnected())
				{
					logger.debug("inMesh is connected.");
					this->shapeConnected = true;
				}else{
					logger.debug("inMesh is NOT connected.");
					this->shapeConnected = false;
				}
			}
			this->exportFileNames.push_back(exportFilename);
			MeshExporter mo(this->mobject, exportFilename);
			this->expand(mo.exportIt());
			this->supported = true;
			break;
		}
	case MFn::kNurbsCurve:
		{
			logger.info(MString("NurbsCurve detected.") + this->shortName);

			MFnNurbsCurve curveFn(this->mobject);
			MPlug inCurvePlug = curveFn.findPlug(MString("create"), &stat);
			if( stat != MS::kSuccess)
			{
				logger.error("Cannot find plug create.");
				this->shapeConnected = true;
			}else{
				logger.debug("Found plug create.");
				if( inCurvePlug.isConnected())
				{
					logger.debug("create is connected.");
					this->shapeConnected = true;
				}else{
					logger.debug("create is NOT connected.");
					this->shapeConnected = false;
				}
			}

			this->exportFileNames.push_back(exportFilename);
			NurbsCurveExporter no(this->mobject, exportFilename);
			this->expand(no.exportIt());
			this->supported = true;
			break;
		}
	case MFn::kNurbsSurface:
		{
			logger.debug( MString("Nurbs Surface Object detected: ") + this->shortName);
			MFnNurbsSurface nurbsFn(this->mobject);
			MPlug inSurfPlug = nurbsFn.findPlug(MString("create"), &stat);

			if( stat != MS::kSuccess)
			{
				logger.error("Cannot find plug create.");
				this->shapeConnected = true;
			}else{
				logger.debug("Found plug create.");
				if( inSurfPlug.isConnected())
				{
					logger.debug("create is connected.");
					this->shapeConnected = true;
				}else{
					logger.debug("create is NOT connected.");
					this->shapeConnected = false;
				}
			}
			this->exportFileNames.push_back(exportFilename);
			NurbsExporter no(this->mobject, exportFilename);
			this->expand(no.exportIt());
			this->supported = true;
			break;
		}
	case MFn::kParticle:
		{
			info = MString("Particle Object detected: ") + this->shortName;
			logger.debug(info);
			this->exportFileNames.push_back(exportFilename);
			//cloud.evaluateDynamics(timeSeconds,false);
			ParticleExporter po(this->mobject, exportFilename);
			this->expand(po.exportIt());
			this->supported = true;
			this->shapeConnected = true; // to trigger always scene upates
			break;
		}
	case MFn::kNParticle:
		{
			info = MString("NParticle Object detected: ") + this->shortName;
			logger.debug(info);
			this->exportFileNames.push_back(exportFilename);
			ParticleExporter po(this->mobject, exportFilename);
			this->expand(po.exportIt());
			this->supported = true;
			this->shapeConnected = true; // to trigger always scene upates
			break;
		}
	case MFn::kFluid:
		{
			info = MString("Fluid Object detected: ") + this->shortName;
			logger.debug(info);
			this->exportFileNames.push_back(exportFilename);
			FluidExporter fo(this->mobject, exportFilename);
			this->expand(fo.exportIt());
			this->supported = true;
			this->shapeConnected = true; // to trigger always scene upates
			break;
		}
	default:
		{
			// not supported
			//info = MString("Objects of type: ") + this->mobject.apiTypeStr() + " are not supported yet.";
			//MGlobal::displayInfo(info);
		}
	}

	switch(type_id)
	{
	case 0x106EF2:
		{
			info = MString("Mantra StandInObject detected: ") + this->shortName;
			logger.debug(info);
			MString geoFileName;
			getString(MString("geoFile"), depFn, geoFileName);
			this->exportFileNames.push_back(geoFileName);
			StandinExporter so(this->mobject, exportFilename);
			this->expand(so.exportIt());
			this->supported = true;
			break;
		}
	}

	return true;
}

bool isValidGeo(MObject& obj)
{
	if( obj.hasFn(MFn::kMesh))
		return true;
	if( obj.hasFn(MFn::kNurbsSurface))
		return true;
	if( obj.hasFn(MFn::kNurbsCurve))
		return true;
	if( obj.hasFn(MFn::kParticle))
		return true;
	if( obj.hasFn(MFn::kNParticle))
		return true;

	return false;
}

// here we write the geometry definition with the bounding box information
bool mtm_MayaObject::exportGeoDefinition(std::ofstream *outFile)
{
	// of course only objects with a valid shape are written to disk, so check if this is
	// such a valid object that can be loaded
	if( !isValidGeo(this->mobject))
		return false;

	// instances do not need to be defined this way 
	if( this->instanceNumber > 0 || this->isInstancerObject)
		return true;

	this->hierarchyNames.clear();

	// create a geometry definition for every exported mb step 
	for( int fileId = 0; fileId < this->exportFileNames.size(); fileId++)
	{
		//MGlobal::displayInfo(MString("MayaObject::writeObjDef - expfilename: ") + this->exportFileNames[fileId]);
		MString hname = makeHierarchyString(this->dagPath.fullPathName()) + "_" + fileId;
		this->hierarchyNames.push_back(hname);
		*outFile << "#start object definition of " << this->dagPath.fullPathName() << " mb step " << fileId << "\n";
		*outFile << "ray_start geo\n";
		*outFile << "\tray_detail ";
		*outFile << " -m " << this->min().x << " " << this->min().y << " " << this->min().z;
		*outFile << " -M " << this->max().x << " " << this->max().y << " " << this->max().z << " ";
		*outFile << hname << " " << this->exportFileNames[fileId] << "\n"; 
		*outFile << "ray_end\n\n";
	}

	//# Save geometry for /obj/sphere_object1/popnet1 at time 3.79167
	//ray_start geo	# {
	//	ray_detail -v 0.0416667 /obj/sphere_object1/popnet1-1 /obj/sphere_object1/popnet1
	//ray_end	# }

	// if object has geometryMotionblur turned on, we try to use the "v" point attribute as motionvectors
	// there sould be only one export file name above for this case
	// float velocityScale = 1.0;
	float velocityScale = 0.0416667f;
	if( this->geometryMotionblur )
	{
		MString geoMbName = makeHierarchyString(this->dagPath.fullPathName()) + "_1";
		this->hierarchyNames.push_back(geoMbName);
		*outFile << "#start object definition of geometryMotionblur object for" << this->dagPath.fullPathName() << "\n";
		*outFile << "ray_start geo\n";
		*outFile << "\tray_detail ";
		*outFile << " -m " << this->min().x << " " << this->min().y << " " << this->min().z;
		*outFile << " -M " << this->max().x << " " << this->max().y << " " << this->max().z << " ";
		*outFile << " -v " << velocityScale << " ";
		*outFile << geoMbName << " " << this->hierarchyNames[0] << "\n"; 
		*outFile << "ray_end\n\n";		
	}

	return true;
}

// here we write the instance definition for every object in the scene
bool mtm_MayaObject::exportInstanceDefinition(std::ofstream *outFile)
{
	return true;
}

void writeSpecDiffContribution( std::ofstream *outFile, MFnDependencyNode& depFn)
{
	bool emitDiffuse = true;
	getBool(MString("emitDiffuse"), depFn, emitDiffuse);
	bool emitSpecular = true;
	getBool(MString("emitSpecular"), depFn, emitSpecular);
	if( !emitDiffuse )
		*outFile << "\t\tray_property light __nondiffuse 1\n";
	if( !emitSpecular )
		*outFile << "\t\tray_property light __nonspecular 1\n";
}

bool mtm_MayaObject::writeLightInstance( std::ofstream *outFile)
{
	logger.debug(MString("exportInst: export light instance ") + this->shortName);

	MFnDependencyNode depFn(this->mobject);

	*outFile << "\n";
	*outFile << "\tray_start light\n";

	try{
		// if geoLight write obj instance matrix instead of light instance
		// not a very elegant way here, but I first have to find out if the object is a geo light
		// because if it is a geolight, then the matrix has to be used from this geometry instead from
		// normal light node.
		bool mtm_useLightGeometry = false;
		MObject lightGeoObject = MObject::kNullObj;
		bool failed = getBool(MString("mtm_useLightGeometry"), depFn, mtm_useLightGeometry);
		if( !failed )
			logger.debug(MString("----> failed getting attribute mtm_useLightGeometry on ") + this->shortName);
		else
			logger.debug(MString("----> mtm_useLightGeo ") + mtm_useLightGeometry);

		MayaObject *lightGeo = NULL;
		if(mtm_useLightGeometry)
		{
			lightGeoObject = getOtherSideNode(MString("mtm_lightGeometry"), this->mobject);
			logger.debug(MString("----> Use Light geometry: ") + getObjectName(lightGeoObject));
			if( lightGeoObject != MObject::kNullObj)
			{
				if( this->scenePtr != NULL )
				{
					lightGeo = this->scenePtr->getObject(lightGeoObject);
					// if the node was not found, it is not defined
					if( lightGeo != NULL )
					{
						writeTMatrixList(outFile, lightGeo->transformMatrices);
					}else{
						logger.debug("----> lightGeo is null.");
						mtm_useLightGeometry = false;
					}
				}else{
					logger.debug("---->scene pointer is null.");
					mtm_useLightGeometry = false;
				}
			}else{
				logger.debug("----> MObject::kNullObj for light geometry skip.");
				mtm_useLightGeometry = false;
			}
		}
	
		if( !mtm_useLightGeometry)
			writeTMatrixList(outFile, this->transformMatrices);

		if( (this->shadowObjects.size() > 0) || (this->castNoShadowObjects.size() > 0))
		{
			MString shadowObjects = "* ";
			for( uint oId = 0; oId < this->shadowObjects.size(); oId++)
			{			
				MString oname = " ";
				if( this->shadowExcludeList )
					oname += "^";
				oname += makeHierarchyString(this->shadowObjects[oId].fullPathName()) + "_0";
				shadowObjects += oname;
			}
			for( int oId = 0; oId < this->castNoShadowObjects.size(); oId++)
			{
				logger.debug(MString("-->Object ") +  this->castNoShadowObjects[oId].partialPathName() + " ignores shadows from " + this->dagPath.partialPathName());
				MString oname = " ^" + makeHierarchyString(this->castNoShadowObjects[oId].fullPathName()) + "_0";
				shadowObjects += oname;
			}
			//ray_property light shadowmask "/obj/grid_object1 /obj/torus_object2"
			*outFile << "\t\tray_property light shadowmask \"" << shadowObjects + "\"\n";
		}
		//ray_property object name "/obj/spotlight1"
		MString hname = makeHierarchyString(this->dagPath.fullPathName()) + "_0";
		*outFile << "\t\tray_property object name \"" << hname + "\"\n";
		bool mtm_surfaceShaders = false;
		getBool(MString("mtm_surfaceShaders"), depFn, mtm_surfaceShaders);

		bool castRTShadows = true;
		bool castSMShadows = true;
		getBool(MString("useRayTraceShadows"), depFn, castRTShadows);
		getBool(MString("useDepthMapShadows"), depFn, castSMShadows);
	
		// valid for all lights
		// ray_property light activeradius 55
		float mtm_activeRadius = 0.0;
		getFloat(MString("mtm_activeRadius"), depFn, mtm_activeRadius);
		if( mtm_activeRadius > 0.0)
		{
			*outFile << "\t\tray_property light activeradius " << mtm_activeRadius << "\n";
		}

		MString colorString = lightColorAsString(depFn);
		float shadowI = shadowColorIntensity(depFn);
		float mtm_shadowQuality = 1.0;
		getFloat(MString("mtm_shadowQuality"), depFn, mtm_shadowQuality);

		switch(this->mobject.apiType())
		{
	
		case MFn::kDirectionalLight:
			{
				logger.debug(MString("writeLightInstance: export kDirectionalLight light: ") + this->shortName);
				//ray_start light	# {
				//ray_transform 0.283283269448 5.96046053411e-008 -0.959036281509 0 -0.466797191313 0.873549299054 -0.137884024835 0 0.837765463261 0.486735679936 0.247461929284 0 35.8992500305 20.8571548462 10.6040563583 1
				//ray_property object name "/obj/distantlight1"
				//ray_property light surfaceshaders 0
				//ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1 distant 1 attenstart 0 doatten 0
				// TODO: attenuation implementieren
				
				mtm_Material *mat = (mtm_Material *)&this->material;
				if( mat->shadingEngineNode != MObject::kNullObj)
				{
					MString sgName = makeGoodString(getObjectName(mat->shadingEngineNode));
					if( mat->lightShaderNet.shaderList.size() > 0)
					{
						MString lightShaderName = "light_" + sgName;
						MString shadowShaderName = "shadow_" + sgName;
						*outFile << "\t\t# found shading group " << sgName << "\n";
						logger.debug(MString(" ObjectSG light shader: ") + sgName);
						*outFile << "\t\tray_property light shader " << lightShaderName << "\n";
						*outFile << "\t\tray_property light shadow " << shadowShaderName;
						*outFile << "\n";
					}
				}

				//*outFile << "\t\tray_property light shader opdef:/Shop/v_asadlight lightcolor " << colorString << " distant 1 attenstart 0 doatten 0\n";
				////ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality 1 shadowI 1
				//if(castRTShadows)
				//{
				//	*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
				//	writeSpecDiffContribution(outFile, depFn);
				//}
				//ray_property light projection "ortho"
				*outFile << "\t\tray_property light projection \"ortho\"\n";
				//ray_property light zoom 1 1
				*outFile << "\t\tray_property light zoom 1 1\n";
				//ray_property light orthowidth 25.1415729523 25.1415729523
				*outFile << "\t\tray_property light orthowidth 25.1415729523 25.1415729523\n";
				//ray_end	# }
				break;
			}
		case MFn::kSpotLight:
			{
				logger.debug(MString("exportInst: export kSpotLight light: ") + this->shortName);
	
				*outFile << "\t\tray_property light surfaceshaders " << (int)mtm_surfaceShaders << "\n";

				mtm_Material *mat = (mtm_Material *)&this->material;
				if( mat->shadingEngineNode != MObject::kNullObj)
				{
					MString sgName = makeGoodString(getObjectName(mat->shadingEngineNode));
					if( mat->lightShaderNet.shaderList.size() > 0)
					{
						MString lightShaderName = "light_" + sgName;
						MString shadowShaderName = "shadow_" + sgName;
						*outFile << "\t\t# found shading group " << sgName << "\n";
						logger.debug(MString(" ObjectSG light shader: ") + sgName);
						*outFile << "\t\tray_property light shader " << lightShaderName << "\n";
						*outFile << "\t\tray_property light shadow " << shadowShaderName;
						*outFile << "\n";
					}
				}

				//float coneAngle, penumbraAngle, dropoff;
				//getFloat(MString("coneAngle"), depFn, coneAngle);
				//getFloat(MString("penumbraAngle"), depFn, penumbraAngle);
				//getFloat(MString("dropoff"), depFn, dropoff);
				////ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1 docone 1 coneangle 45 conedelta 10 conerolloff 1 attenstart 0 doatten 0
				//MString spotString;
				//float rolloff = 0.0;
				//if( penumbraAngle > 0.0)
				//	rolloff = dropoff/penumbraAngle;
				//spotString += colorString + " docone 1  coneangle " + RadToDeg(coneAngle) + " conedelta " + RadToDeg(penumbraAngle) + " conerolloff " +  rolloff;
				//spotString += " attenstart 0 doatten 0";
				//*outFile << "\t\tray_property light shader opdef:/Shop/v_asadlight lightcolor " << spotString << "\n";
				//writeSpecDiffContribution(outFile, depFn);

				//if(castRTShadows)
				//{
				//	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality 1 shadowI 1
				//	*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
				//}
				if(castSMShadows)
				{
					if( this->shadowMapFiles.size() > 0 )
					{
						MString smapFilename = this->shadowMapFiles[0];
						bool deepMap = true;
						getBool(MString("mtm_deepShadowMap"), depFn, deepMap);
						float spread = 1.0f;
						getFloat(MString("mtm_spread"), depFn, spread);
						float bias = 0.05f;
						getFloat(MString("mtm_bias"), depFn, bias);
						float blur = 1.0f;
						getFloat(MString("mtm_blur"), depFn, blur);
						if( deepMap )
						{
							*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype deep map ";
							*outFile << "\"" << smapFilename << "\" spread " << spread << " blur " << blur << " bias " << bias << " quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
						}else{
							*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype zdepth map ";
							*outFile << "\"" << smapFilename << "\" spread " << spread << " blur " << blur << " bias " << bias << " quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
						}
					}
					//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype zdepth map "temp:spotlight2.rat" spread 0.977 blur 0.019 bias 0.06 quality 1.33 shadowI 1
				}
				//ray_property light projection "perspective"
				*outFile << "\t\tray_property light projection \"perspective\"\n";
				//ray_property light zoom 0.784842742499 0.784842742499
				*outFile << "\t\tray_property light zoom 1 1\n";
				break;
			}
		case MFn::kPointLight:
			{
				logger.debug(MString("exportInst: export kPointLight light: ") + this->shortName);
				*outFile << "\t\tray_property light surfaceshaders " << (int)mtm_surfaceShaders << "\n";
				//*outFile << "\t\tray_property light shader opdef:/Shop/v_asadlight lightcolor " << colorString << " attenstart 0 doatten 0\n";
				if(castRTShadows)
				{
					*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
				}
				writeSpecDiffContribution(outFile, depFn);
				//ray_property light projection "perspective"
				*outFile << "\t\tray_property light projection \"perspective\"\n";
				//ray_property light zoom 1 1
				*outFile << "\t\tray_property light zoom 1 1\n";
				//ray_property light orthowidth 25.1415729523 25.1415729523
				*outFile << "\t\tray_property light orthowidth 25.1415729523 25.1415729523\n";

				mtm_Material *mat = (mtm_Material *)&this->material;
				if( mat->shadingEngineNode != MObject::kNullObj)
				{
					MString sgName = makeGoodString(getObjectName(mat->shadingEngineNode));
					if( mat->lightShaderNet.shaderList.size() > 0)
					{
						MString lightShaderName = "light_" + sgName;
						MString shadowShaderName = "shadow_" + sgName;
						*outFile << "\t\t# found shading group " << sgName << "\n";
						logger.debug(MString(" ObjectSG light shader: ") + sgName);
						*outFile << "\t\tray_property light shader " << lightShaderName << "\n";
						*outFile << "\t\tray_property light shadow " << shadowShaderName;
						*outFile << "\n";
					}
				}

				break;
			}
		case MFn::kAreaLight:
			{
				logger.debug(MString("exportInst: export kAreaLight light: ") + this->shortName);
				*outFile << "\t\tray_property light surfaceshaders " << (int)mtm_surfaceShaders << "\n";
				//*outFile << "\t\tray_property light surfaceshaders 1\n";
				//ray_property object surface opdef:/Shop/v_arealight lightcolor 1 1 1 normalizearea 1 attenstart 0 doatten 0
				//ray_property light areashape "grid"
				//ray_property light areasize 1 1
				//ray_property light areamap ""
				//ray_property light areafullsphere 0
				//ray_property light surfaceshaders 1
				//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality 1 shadowI 1
				//ray_property light projection "perspective"
				//ray_property light zoom 1.20710676487 1.20710676487
				//ray_end	# }
				
				// for area lights you can set a geometry to create geometry lights
				//ray_geometry /obj/geolight1/switch3
	
				// we will check if the attribute called mtm_useLightGeometry is turned on
				// if yes, check if there is a geometry and get its name
				if(mtm_useLightGeometry)
				{
					logger.debug(MString("Found light geometry connection with object: ") + lightGeo->shortName);
					MString hname = makeHierarchyString(lightGeo->dagPath.fullPathName()) + "_0";
					*outFile << "\t\tray_geometry " << hname << "\n";
				}
	
				MString colorString = lightColorAsString(depFn);
				float shadowI = shadowColorIntensity(depFn);
	
				float mtm_samplingQuality = -1.0;
				getFloat(MString("mtm_samplingQuality"), depFn, mtm_samplingQuality);
				if( mtm_samplingQuality >= 0.0)
					*outFile << "\t\tray_property object samplingquality " << mtm_samplingQuality << "\n";
				
				// render the light geometry or not
				// ray_property object phantom 0
				bool mtm_renderLightGeometry = false;
				getBool(MString("mtm_renderLightGeometry"), depFn, mtm_renderLightGeometry);
				if(mtm_renderLightGeometry)
					*outFile << "\t\tray_property object phantom 0\n";
	
				bool mtm_areaReverse = false;
				getBool(MString("mtm_areaReverse"), depFn, mtm_areaReverse);
				bool mtm_areaSingleSided = false;
				getBool(MString("mtm_areaSingleSided"), depFn, mtm_areaSingleSided);
				bool mtm_normalizeArea = true;
				getBool(MString("mtm_normalizeArea"), depFn, mtm_normalizeArea);
				MString nArea(" normalizearea 1 ");
				if( !mtm_normalizeArea)
					nArea = MString(" normalizearea 0 ");
	
				//*outFile << "\t\tray_property object surface opdef:/Shop/v_arealight lightcolor " << colorString << nArea << "attenstart 0 doatten 0  singlesided " << mtm_areaSingleSided << " reverse " << mtm_areaReverse <<"\n";
	
				mtm_Material *mat = (mtm_Material *)&this->material;
				if( mat->shadingEngineNode != MObject::kNullObj)
				{
					MString sgName = makeGoodString(getObjectName(mat->shadingEngineNode));
					if( mat->lightShaderNet.shaderList.size() > 0)
					{
						MString lightShaderName = "light_" + sgName;
						MString shadowShaderName = "shadow_" + sgName;
						*outFile << "\t\t# found shading group " << sgName << "\n";
						logger.debug(MString(" ObjectSG light shader: ") + sgName);
						*outFile << "\t\tray_property light shader " << lightShaderName << "\n";
						*outFile << "\t\tray_property light shadow " << shadowShaderName;
						*outFile << "\n";
					}
				}


				//if(castRTShadows)
				//{
				//	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality 1 shadowI 1
				//	float mtm_shadowQuality = 1.0;
				//	getFloat(MString("mtm_shadowQuality"), depFn, mtm_shadowQuality);
				//	*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
				//}
	
				if(mtm_useLightGeometry)
				{
					*outFile << "\t\tray_property light areashape \"geo\"\n";
				}else{
					int lightShape = 1; // line:grid:disk:sphere
					getEnum(MString("mtm_areaType"), depFn, lightShape);
					MStringArray areaLightTypes;
					areaLightTypes.append(MString("line"));
					areaLightTypes.append(MString("grid"));
					areaLightTypes.append(MString("disk"));
					areaLightTypes.append(MString("sphere"));
					//ray_property light areashape "grid"
					*outFile << "\t\tray_property light areashape \"" << areaLightTypes[lightShape] << "\"\n";
				}
				//ray_property light areasize 1 1
				// areasize will be set with light scaling, so let this at 1,1 at the moment
				*outFile << "\t\tray_property light areasize 1 1\n";
				//ray_property light areamap ""
				*outFile << "\t\tray_property light areamap \"\"\n";						
				//ray_property light areafullsphere 0
				*outFile << "\t\tray_property light areafullsphere 0\n";	
	
				writeSpecDiffContribution(outFile, depFn);
	
				//ray_property light projection "perspective"
				*outFile << "\t\tray_property light projection \"perspective\"\n";
				//ray_property light zoom 1 1
				*outFile << "\t\tray_property light zoom 1.20710676487 1.20710676487\n";
				break;
			}
	//	case MFn::kVolumeLight:
	//		{
	//			MGlobal::displayInfo("exportInst: export kVolumeLight light: ");
	//			scn->write("\nray_start light");
	//			this->exportMatrix(scn);
	//			scn->write(MString("ray_property object name \"") + hierarchyString + "\"");
	//			scn->write("ray_property light surfaceshaders 0");
	//			//ray_end	# }
	//			scn->write("ray_end");
	//			break;
	//		}
	//	}
	
		};
	
		int tid = depFn.typeId().id();
		switch(tid)
		{
		case 0x106EF1:
			{
				logger.debug(MString("Detected mtm_envLight") + depFn.name());
	//// ALL
	//	//ray_property object samplingquality 1.37999999523
	//	//ray_property light projection "perspective"
	//	//ray_property light zoom 1.20710550585 1.20710550585
	//	//ray_property light envintensity 0.855980018635 0.883581288896 1.26999998093
	//	//ray_property object surface opdef:/Shop/v_arealight lightcolor 0.85598 0.883581 1.27 envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//
	//// DIRECT
	//	//ray_property light areashape "env"
	//	//ray_property light areamap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//	//ray_property light areafullsphere 1
	//	//ray_property light raybackground 0
	//	//ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1
	//	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter shadowI 1
	//// AO
	//	//ray_property light raybackground 0
	//	//ray_property light shader opdef:/Shop/v_gilight light_color 1 1 1 istyle "opacity" doraysamples 1 doadaptive 1 domaxdist 0 maxdist 10 cone_angle 82.7 objmask "*" envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic" envtint 0.85598 0.883581 1.27 envnull "/obj/envlight1"
	//// RTB
	//	//ray_property light areashape "env"
	//	//ray_property light areamap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic"
	//	//ray_property light areafullsphere 1
	//	//ray_property light raybackground 1
	//	//ray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1
	//	//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter shadowI 1
	
				int envtype = 0; // 0 = direct, 1 = AO, 2 = raytraceBG
				getEnum(MString("envtype"), depFn, envtype);
	
				if( envtype == 0)
					logger.info("Exporting direct envlight.");
				if( envtype == 1)
					logger.info("Exporting AO envlight.");
				if( envtype == 2)
					logger.info("Exporting FullIrradiance envlight.");
				if( envtype == 3)
					logger.info("Exporting RTBG envlight.");
	
				MColor lightColor(1,1,1);
				getColor(MString("lightColor"), depFn, lightColor);
				float intensity = 1.0;
				getFloat(MString("intensity"), depFn, intensity);
				MString colorString = MString("") + lightColor.r * intensity + " "  + lightColor.g * intensity + " "  + lightColor.b * intensity + " ";
	
				float shadowI = 1.0;
				getFloat(MString("shadowI"), depFn, shadowI);
	
				float envint = 1.0;
				//getColor(MString("envintensity"), depFn, envintensity);
				getFloat(MString("intensity"), depFn, envint);
				MString envintensity = MString("") + envint + " " + envint + " " + envint;
				*outFile << "\t\tray_property light envintensity " << envintensity << "\n";
	
				// not a dyanmic attribute, its built in and has a different name
				float mtm_samplingQuality = -1.0;
				getFloat(MString("samplingquality"), depFn, mtm_samplingQuality);
				if( mtm_samplingQuality >= 0.0)
					*outFile << "\t\tray_property object samplingquality " << mtm_samplingQuality << "\n";
				
				if(envtype == 0 || envtype == 3)
					*outFile << "\t\tray_property light areashape \"env\"\n";
	
				// no surface shaders for AO
				//if(envtype == 0 || envtype == 2)
				//	outFile << "\t\tray_property light surfaceshaders " << (int)mtm_surfaceShaders << "\n";
				//else
				//	outFile << "\t\tray_property light surfaceshaders 0\n";
	
				MString envmap = "";
				getString(MString("envmap"), depFn, envmap); 
				if( envmap.length() > 0)
					envmap = " envmap \"" + envmap + "\"";
				else
					envmap = "";
	
				if(envtype == 0 || envtype == 3)
				{
					//MString areamap = "";
					//getString(MString("areamap"), depFn, areamap); 
					if( envmap.length() > 0)
						*outFile << "\t\tray_property light areamap " << envmap << "\n";
				}
	
				if(envtype == 0 || envtype == 1 || envtype == 2)
					*outFile << "\t\tray_property light raybackground 0\n";
				if(envtype == 3)
					*outFile << "\t\tray_property light raybackground 1\n";
	
	
				if(envtype == 0 || envtype == 3)
				{
					bool areafullsphere = true;
					getBool(MString("areafullsphere"), depFn, areafullsphere);
					if(areafullsphere)
						*outFile << "\t\tray_property light areafullsphere 1\n";
					else
						*outFile << "\t\tray_property light areafullsphere 0\n";
				}
	
				//ray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality 1 shadowI 1
				float mtm_shadowQuality = 1.0;
				getFloat(MString("mtm_shadowQuality"), depFn, mtm_shadowQuality);
	
				MString envtint = " 0 0 0 ";
				getColor(MString("envtint"), depFn, envtint);
	
				if(envtype == 0 || envtype == 3)
				{
					*outFile << "\t\tray_property object surface opdef:/Shop/v_arealight lightcolor " << colorString << envmap <<"\n";
					*outFile << "\t\tray_property light shader opdef:/Shop/v_asadlight lightcolor 1 1 1\n";
	
					bool castshadow = true;
					getBool(MString("castshadow"), depFn, castshadow);
					if(castshadow)
					{
						*outFile << "\t\tray_property light shadow opdef:/Shop/v_rayshadow shadowtype filter bias 0.05 quality " << mtm_shadowQuality << " shadowI " << shadowI << "\n";
					}
				}else{
					// Ambient Occlusion via gilight
					// This is the method with the old v_gilight and lightTemplate workflow, much better at the moment for me...
					// envlight style:
					//		ray_property light shader opdef:/Shop/v_gilight light_color 1 1 1 istyle "opacity" doraysamples 1 doadaptive 1 domaxdist 0 maxdist 10 cone_angle 82.7 objmask "*" envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic" envtint 0.85598 0.883581 1.27 envnull "/obj/envlight1"
					// lightTemplate full irradiance style:
					//		ray_property light shader opdef:/Shop/v_gilight istyle "full" samples 9 doadaptive 1 cone_angle 59.7999992371 domaxdist 1 maxdist 12.8999996185 envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic" envtint 1 0.399267464876 0.333000004292
					// lightTemplate ao style:
					//		ray_property light shader opdef:/Shop/v_gilight samples 9 doadaptive 1 cone_angle 59.7999992371 domaxdist 1 maxdist 12.8999996185 envmap "C:/PROGRA~1/SIDEEF~1/HOUDIN~1.775/houdini/pic/default.pic" envtint 1 0.399267464876 0.333000004292
	
					float coneangle = 45.0;
					getFloat(MString("coneangle"), depFn, coneangle);
					float maxdist = 10.0;
					getFloat(MString("maxdist"), depFn, maxdist);
					bool domaxdist = false;
					getBool(MString("domaxdist"), depFn, domaxdist);
					bool doadaptive = false;
					getBool(MString("doadaptive"), depFn, doadaptive);
					bool doraysamples = false;
					getBool(MString("doraysamples"), depFn, doraysamples);
					int samples = 32;
					getInt(MString("samples"), depFn, samples);
					MString objMask("*");
					getString(MString("objmask"), depFn, objMask);
					MString style(" istyle \"opacity\"");
					if(envtype == 2)
						style = " istyle \"full\"";
					*outFile << "\t\tray_property light shader opdef:/Shop/v_gilight "<< style <<" light_color " << colorString << " doraysamples " << doraysamples << " doadaptive "<< doadaptive;
					*outFile << " domaxdist " << domaxdist << " maxdist " << maxdist << " cone_angle " << coneangle << " objmask \""<< objMask << "\"" << envmap << " background 1 1 1 ";
					*outFile << " samples " << samples << "\n"; //envtint " << envtint << " envnull \"" << this->objHierarchyName <<"\"\n";
				}
				writeSpecDiffContribution(outFile, depFn);
	
				//ray_property light projection "perspective"
				*outFile << "\t\tray_property light projection \"perspective\"\n";
				//ray_property light zoom 1 1
				*outFile << "\t\tray_property light zoom 1.20710676487 1.20710676487\n";
	
				break;
			}
		}
	}catch(char *errorMsg){
		logger.error(errorMsg);
		return false;	
	}
	*outFile << "\tray_end\n\n";
	return true;
}

// for shadow map renderings a light acts as a camera
bool mtm_MayaObject::writeSmapLightInstance(std::ofstream *outFile, float scaleFactor)
{
	logger.debug(MString("exportInst: export shadowMap lightcam") + this->shortName);

	//MFnSpotLight spotFn(this->mobject);
	//float aperture = 35.0;
	////getFloat(MString("horizontalFilmAperture"), cameraFn, aperture);
	//// aperture is needed to calculate the field of view. Aperture is mesured in millimeters
	//// so I need horizontal aperture maya (inches) * milimeterToInces (2.54)
	//// camera horizontalFilmAperture
	//// aperture is in inces in maya so multiply by 25.4 so we get millimeters, same like focalLength
	//// aperture = tools.getFloatAttr("horizontalFilmAperture", self.dependencyNode, 2.5 ) * 25.4
	//// Ratio of the focal length to the aperture of the camera. It is used to determine the field of view of the camera.
	//// zoom = focalLength / aperture
	//float focalLength = 35.0f;
	////getFloat(MString("focalLength"), cameraFn, focalLength);
	//// focalLength / aperture is tan(alpha)
	//float cameraZoom = focalLength / aperture;
	//float coneAngle = 35.0f;
	//getFloat(MString("coneAngle"), spotFn, coneAngle);
	//logger.debug(MString("------->> coneAngle ") + coneAngle);
	////float rad = DegToRad(coneAngle);
	//cameraZoom = 1.0f/(tan(coneAngle/2.0f) * 2.0f);
	//outFile << "\tray_property camera projection \"perspective\"\n";
	//outFile << "\tray_property camera zoom " << cameraZoom << "\n";
	//// check for orthographic or perspective projection
	////bool isOrtho = false;
	////getBool(MString("orthographic"), cameraFn, isOrtho);
	////if( isOrtho )
	////{
	////	float orthographicWidth;
	////	getFloat(MString("orthographicWidth"), cameraFn, orthographicWidth);
	////	//ray_property camera orthowidth 25.1415729523
	////	outFile << "\tray_property camera orthowidth " << orthographicWidth << "\n";
	////	outFile << "\tray_property camera projection \"ortho\"\n";
	////}else{
	////	//ray_property camera projection "perspective"
	////	outFile << "\tray_property camera projection \"perspective\"\n";
	////	//ray_property camera zoom 1.20710555939
	////	outFile << "\tray_property camera zoom " << cameraZoom << "\n";
	////}
	//
	//float nearClip = .1f;
	//float farClip = 10000.0f;
	//getFloat(MString("mtm_nearClip"), spotFn, nearClip);
	//getFloat(MString("mtm_farClip"), spotFn, farClip);
	////ray_property camera clip 0.10000000149 32024.9707031
	//outFile << "\tray_property camera clip " << nearClip << " " << farClip << "\n"; 

	//// lightTransform inverse
	//writeTMatrixList(outFile, this->transformMatrices, true, scaleFactor);
	//outFile << "\n\n";
	return true;
}

bool mtm_MayaObject::writeCamInstance( std::ofstream *outFile, float scaleFactor, bool doDof)
{
	logger.debug(MString("writeCamInstance: export cam") + this->shortName);
	
	try{
		MFnCamera cameraFn(this->mobject);
		float aperture;
		if(!getFloat(MString("horizontalFilmAperture"), cameraFn, aperture))
			throw("problem reading horizontalFilmAperture");

		aperture *= 25.4f;
		// aperture is needed to calculate the field of view. Aperture is mesured in millimeters
		// so I need horizontal aperture maya (inches) * milimeterToInces (2.54)
		// camera horizontalFilmAperture
		// aperture is in inces in maya so multiply by 25.4 so we get millimeters, same like focalLength
		// aperture = tools.getFloatAttr("horizontalFilmAperture", self.dependencyNode, 2.5 ) * 25.4
		// Ratio of the focal length to the aperture of the camera. It is used to determine the field of view of the camera.
		// zoom = focalLength / aperture
		float focalLength;
		if(!getFloat(MString("focalLength"), cameraFn, focalLength))
			throw("problem reading focalLength");

		float cameraZoom = focalLength / aperture;

		// check for orthographic or perspective projection
		bool isOrtho = false;
		if(!getBool(MString("orthographic"), cameraFn, isOrtho))
			throw("problem reading orthographic");

		if( isOrtho )
		{
			float orthographicWidth;
			if(!getFloat(MString("orthographicWidth"), cameraFn, orthographicWidth))
				throw("problem reading orthographicWidth");

			//ray_property camera orthowidth 25.1415729523
			*outFile << "\tray_property camera orthowidth " << orthographicWidth << "\n";
			*outFile << "\tray_property camera projection \"ortho\"\n";
		}else{
			//ray_property camera projection "perspective"
			*outFile << "\tray_property camera projection \"perspective\"\n";
			//ray_property camera zoom 1.20710555939
			*outFile << "\tray_property camera zoom " << cameraZoom << "\n";
		}
		
		// Depth of field turned on?
		if( doDof )
		{
			// this is not really necessary, this way you can turn on DOF only for a special cameras
			// at the moment it is ignored, because you expect it to work despite of the camera settings
			bool camDoDof = true;
			if(!getBool(MString("depthOfField"), cameraFn, camDoDof))
				throw("problem reading depthOfField");
			float focusDistance = 10.0f * scaleFactor;
			float fStop = 5.0f;
			if( !getFloat(MString("focusDistance"), cameraFn, focusDistance))
				throw("problem reading focusDistance");

			focusDistance = focusDistance;
			if( !getFloat(MString("fStop"), cameraFn, fStop))
				throw("problem reading fStop");

			float focusRegionScale = 1.0;
			if( !getFloat(MString("focusRegionScale"), cameraFn, focusRegionScale))
				throw("problem reading focusRegionScale");
			
			// focal length is not 135 mm but 0.135 maybe thats a global unit setting... well I just tested it and indeed
			// this is dependent from global unit settings. By default one maya unit is one meter in houdini, so I have to 
			// multiply it by 0.001 to get the correct millimeter amount
			focalLength = focalLength / 100.0f / scaleFactor * focusRegionScale;
			*outFile << "\tray_property camera focal " << focalLength << "\n";
			*outFile << "\tray_property camera focus " << focusDistance << "\n";
			*outFile << "\tray_property camera fstop " << fStop << "\n";
		}
		float nearClip = .01f;
		float farClip = 1000000.0f;
		if(!getFloat(MString("nearClipPlane"), cameraFn, nearClip))
			throw("problem reading nearClipPlane");
		if(!getFloat(MString("farClipPlane"), cameraFn, farClip))
			throw("problem reading farClipPlane");

		//ray_property camera clip 0.10000000149 32024.9707031
		*outFile << "\tray_property camera clip " << nearClip << " " << farClip << "\n"; 

		// camera transform
		writeTMatrixList(outFile, this->transformMatrices, true, scaleFactor);
	}catch(char *errorMsg){
		logger.error(errorMsg);
		return false;	
	}

	*outFile << "\n\n";
	return true;
}

// mantra instance definition:
//  ray_start object
//	ray_transform 10 0 0 0 0 10 0 0 0 0 10 0 0 0 0 1
//	ray_mtransform 10 0 0 0 0 10 0 0 0 0 10 0 0 0 0 1
//	ray_geometry /obj/box_object1/xform1
//	ray_geometry /obj/box_object1/xform1-1
//	ray_property object name "/obj/box_object1"
//   ray_end

bool mtm_MayaObject::writeGeoInstance( std::ofstream *outFile)
{
	MStatus stat;
	if( !this->visible )
		return true;
	if( !this->supported)
		return true;

	logger.debug(MString("exportInst: export obj") + this->shortName);

	//ray_start object
	*outFile << "\tray_start object\n";
	writeTMatrixList(outFile, this->transformMatrices);

	std::vector<MString> hnames = this->hierarchyNames;

	// if this object is a instanced one, it has a original object where I get the path from
	if( this->origObject != NULL)
	{
		logger.progress(MString("object is instance and has origObject ") + this->origObject->shortName);
		hnames = this->origObject->hierarchyNames;
	}
	
	//if( this->origId > -1)
	//{
	//	logger.progress(MString("object is instance and has origObject ") + getObjectPtr(this->origId)->name);
	//	hnames = getObjectPtr(this->origId)->hierarchyNames;
	//}

	for( int hId = 0; hId < hnames.size(); hId++)
	{
		*outFile << "\t\tray_geometry " << hnames[hId] << "\n";
	}
	MString hname = makeHierarchyString(this->dagPath.fullPathName()) + "_" + this->instanceNumber;
	*outFile << "\t\tray_property object name \"" << hname << "\"\n";

	mtm_Material *mat = (mtm_Material *)&this->material;
	if( mat->shadingEngineNode != MObject::kNullObj)
	{
		MString sgName = makeGoodString(getObjectName(mat->shadingEngineNode));
		if( mat->surfaceShaderNet.shaderList.size() > 0)
		{
			MString surfaceShaderName = "surface_" + sgName;
			*outFile << "\t\t# found shading group " << sgName << "\n";
			logger.debug(MString(" ObjectSG surface shader: ") + sgName);
	//			//ray_property object surface opdef:/Shop/c_mantraSurface   
			*outFile << "\t\tray_property object surface " << surfaceShaderName;
	//			sg.surfaceShader.writeNodeShaderInstCall(outFile);
			*outFile << "\n";
		}
		if( mat->displacementShaderNet.shaderList.size() > 0)
		{
			// Displacement bounds
			float displaceBound = 1.0;
			MFnDependencyNode depFn(this->mobject);
			if (getFloat(MString("mtm_displacebound"), depFn, displaceBound))
			{
				*outFile << "\t\tray_property object displacebound " << displaceBound << "\n";
			}else{
				MVector bboxscale(1.0,1.0,1.0);
				if (getVector(MString("boundingBoxScale"), depFn, bboxscale))
				{
					*outFile << "\t\tray_property object displacebound " << bboxscale.length() << "\n";
				}
			}
			MString displaceShaderName = "displace_" + sgName;
			logger.debug(MString(" ObjectSG displacement shader: ") + displaceShaderName);
			//ray_property object surface opdef:/Shop/c_mantraSurface   
			*outFile << "\t\tray_property object surface " << displaceShaderName << "_displacement ";
			*outFile << "\n";
		}
	}

	if( this->linkedLights.size() > 0)
	{
		//	ray_property object lightmask "/obj/redlight"
		MString llinks = "* ";
		for( uint lId = 0; lId < this->linkedLights.size(); lId++)
		{
			MString lname = "";
			if( this->lightExcludeList )
				lname += "^";
			lname += makeHierarchyString(this->linkedLights[lId].fullPathName());
			lname += "_0";
			llinks += " " + lname;
		}
		*outFile << "\t\tray_property object lightmask \"" << llinks << "\"\n";
	}

	MFnDependencyNode depFn(this->mobject);
	bool doSubdiv = false;
	if (getBool(MString("mtm_renderAsSubdiv"), depFn, doSubdiv))
	{
		if( doSubdiv )
			*outFile << "\t\tray_property object rendersubd 1\n";
	}
	bool primVis = true;
	if (getBool(MString("primaryVisibility"), depFn, primVis))
	{
		if( !primVis )
			*outFile << "\t\tray_property object phantom 1\n";
	}
	// motionbur based on motion vectors
	if( this->geometryMotionblur )
	{
		*outFile << "\t\tray_property object velocityblur 1\n";
		float velocityScale = 0.0416666666667;
		//float velocityScale = 1.0;
		*outFile << "\t\tray_property object velocityscale " << velocityScale << "\n";
	}

	*outFile << "\tray_end\n\n";
	return true;

}
