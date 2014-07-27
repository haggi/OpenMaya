#include "Corona.h"
#include <maya/MPlugArray.h>
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "CoronaOSLMap.h"
#include <maya/MFnAttribute.h>

#include "CoronaMap.h"

static Logging logger;

static std::vector<Corona::NativeMtlData> dataArray;
static std::vector<MObject> shaderArray;
static std::vector<MString> helperNodeArray;

bool CoronaRenderer::doesHelperNodeExist(MString helperNode)
{
	for (uint hId = 0; hId < helperNodeArray.size(); hId++)
	{
		if (helperNodeArray[hId] == helperNode)
			return true;
	}
	return false;
}

MString CoronaRenderer::createPlugHelperNodeName(MPlug& plug, bool outType)
{
	ATTR_TYPE at = getPlugAttrType(plug);
	MString plugHelperTypename = "rgbtof";
	if (!outType)
		plugHelperTypename = "ftorgb";
	if (at == ATTR_TYPE_VECTOR)
	{
		plugHelperTypename = "vectof";
		if (!outType)
			plugHelperTypename = "ftovec";
	}
	MPlug tmpPlug = plug;
	MString plugname = pystring::replace(plug.name().asChar(), ".", "_").c_str();
	MString helperNodeName = plugname + "_" + plugHelperTypename;
	return helperNodeName;
}

MString CoronaRenderer::createPlugHelperNodeName(const char *attrName, MObject& node, bool outType)
{
	MFnDependencyNode depFn(node);
	MPlug plug = depFn.findPlug(attrName);
	return createPlugHelperNodeName(plug, outType);
}

void CoronaRenderer::createPlugHelperNode(MPlug plug, bool outType = false)
{
	MString helperNodeName = createPlugHelperNodeName(plug, outType);
	ATTR_TYPE at = getPlugAttrType(plug);
	MString attrName = getAttributeNameFromPlug(plug);
	bool success;

	MString plugHelperTypename = "rgbtof";
	if (at == ATTR_TYPE_VECTOR)
		plugHelperTypename = "vectof";
	MString shaderName = "colorToFloat";
	if (!outType)
		shaderName = "floatToColor";

	MString inAttName = "inColor";
	if (at == ATTR_TYPE_VECTOR)
	{
		shaderName = "vectorToFloat";
		inAttName = "inVector";
		if (!outType)
			shaderName = "floatToVector";
	}

	// if the helper is not a out plug helper (vector -> float or color -> float)
	// we have an in plug helper (float -> vector, float -> color) and these should be initialized
	if (!outType)
	{
		MFnDependencyNode depFn(plug.node());
		float fval0 = 0;
		float fval1 = 0;
		float fval2 = 0;
		if (shaderName == "floatToColor")
		{
			MColor value = getColorAttr(attrName.asChar(), depFn);
			fval0 = value.r;
			fval1 = value.g;
			fval2 = value.b;
		}
		if (shaderName == "floatToVector")
		{
			MVector value = getVectorAttr(attrName.asChar(), depFn);
			fval0 = value.x;
			fval1 = value.y;
			fval2 = value.z;
		}
		this->oslRenderer.shadingsys->Parameter("inX", OSL::TypeDesc::TypeFloat, &fval0);
		this->oslRenderer.shadingsys->Parameter("inY", OSL::TypeDesc::TypeFloat, &fval1);
		this->oslRenderer.shadingsys->Parameter("inZ", OSL::TypeDesc::TypeFloat, &fval2);
	}

	logger.debug(MString("Creating helpernode ") + shaderName + " with name " + helperNodeName);
	success = this->oslRenderer.shadingsys->Shader("surface", shaderName.asChar(), helperNodeName.asChar());
	if (!success)
	{
		logger.debug(MString("Failed to register shader ") + shaderName);
	}

	MString nodeName = getObjectName(plug.node());
	MString attName = getAttributeNameFromPlug(plug);
	// we only create the connections between a previous defined output node and the helper node
	// we cannot yet define the connection between a helper node and the attribute itself because the current node is not yet defined.
	if (outType)
	{
		logger.debug(MString("Connecting outType helper node ") + nodeName + "." + attName + " --> " + helperNodeName + "." + inAttName);
		success = this->oslRenderer.shadingsys->ConnectShaders(nodeName.asChar(), attName.asChar(), helperNodeName.asChar(), inAttName.asChar());
	}
	//else
	//	success = this->oslRenderer.shadingsys->ConnectShaders(helperNodeName.asChar(), inAttName.asChar(), nodeName.asChar(), attName.asChar());
	helperNodeArray.push_back(helperNodeName);
}

void CoronaRenderer::createOSLHelperNodes(ShadingNode& snode)
{
	MFnDependencyNode depFn(snode.mobject);
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute sa = snode.inputAttributes[i];
		// at the moment our helper nodes convert rgb->3float, 3folat->rgb, vector->3float and 3float->rgb 
		if ((sa.type != "color") && (sa.type != "vector"))
			continue;
		MPlug p = depFn.findPlug(sa.name.c_str());
		// we only need helper nodes if child attributes are connected.
		// If we have parent connections AND child connections, the parent connections win.
		MPlugArray pa;
		p.connectedTo(pa, true, false);
		if (pa.length() > 0)
			continue;
		if (p.numConnectedChildren() == 0)
			continue;

		logger.debug(MString("Found connected child input attribute: ") + p.name());
		MString typeName = "ftorgb";
		if (sa.type == "vector")
			typeName = "ftovec";
		//MString inHelperNodeName = createPlugHelperNodeName(p, false);
		//bool helperNodeExists = doesHelperNodeExist(inHelperNodeName);
		//if (!helperNodeExists)
		//{
		//	logger.debug(MString("Inattribute Helper node does not yet exist: ") + inHelperNodeName + " creating a new one");

		//	MPlug tmp = p;
		//	while (tmp.isChild())
		//		tmp = tmp.parent();
		//	MPlug parentPlug = tmp;
		//	createPlugHelperNode(parentPlug, false);
		//}

		/// ----------------------- input helper is created, now we search for the input attribute
		for (uint chId = 0; chId < p.numChildren(); chId++)
		{
			if (!p.child(chId).isConnected())
				continue;

			if (chId > 2)
			{
				logger.error(MString("Dest plug ") + p.name() + " child id > 2 - it not a color or vector.");
				continue;
			}
			MPlug connectedDestPlug = p.child(chId);

			const char *inAttributes[] = { "inX", "inY", "inZ" };
			const char *outAttributes[] = { "outX", "outY", "outZ" };
			std::string inAttribute = inAttributes[chId];
			std::string outAttribute; 

			MPlugArray plugArray;
			connectedDestPlug.connectedTo(plugArray, true, false);
			if (plugArray.length() == 0)
			{
				logger.error(MString("Dest plug ") + connectedDestPlug.name() + " is connected but has no input plug");
				continue;
			}
			MPlug connectedPlug = plugArray[0];
			logger.error(MString("Source plug ") + connectedPlug.name());
				
			outAttribute = getAttributeNameFromPlug(connectedPlug).asChar();
			MObject connectedNode = connectedPlug.node();
			MPlug tmp = connectedPlug;
			while (tmp.isChild())
				tmp = tmp.parent();
			MString outHelperNodeName = createPlugHelperNodeName(tmp, true);
			std::string outNode = getObjectName(connectedNode).asChar();

			if (connectedPlug.isChild())
			{
				MPlug tmp = connectedPlug;
				while (tmp.isChild())
					tmp = tmp.parent();
				MPlug parentPlug = tmp;
				
				int childId = getChildId(connectedPlug);
				if ((childId < 0) || (childId > 2))
				{
					logger.error(MString("Source plug ") + p.name() + " child id is wrong (<0 or >2): " + childId);
					continue;
				}

				outAttribute = outAttributes[childId];
				if(!doesHelperNodeExist(outHelperNodeName))
					createPlugHelperNode(parentPlug, true);
				outNode = outHelperNodeName.asChar();
			}

			MString inHelperNodeName = createPlugHelperNodeName(p, false);
			bool helperNodeExists = doesHelperNodeExist(inHelperNodeName);
			if (!helperNodeExists)
			{
				logger.debug(MString("Inattribute Helper node does not yet exist: ") + inHelperNodeName + " creating a new one");

				MPlug tmp = p;
				while (tmp.isChild())
					tmp = tmp.parent();
				MPlug parentPlug = tmp;
				createPlugHelperNode(parentPlug, false);
			}

			std::string inNode = inHelperNodeName.asChar();
			logger.debug(MString("Connecting helper nodes: ") + outNode.c_str() + "." + outAttribute.c_str() + " --> " + inNode.c_str() + "." + inAttribute.c_str());
			this->oslRenderer.shadingsys->ConnectShaders(outNode, outAttribute, inNode, inAttribute);
		}
	}
}


Corona::Abstract::Map *CoronaRenderer::getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn)
{
	helperNodeArray.clear();
	size_t numNodes = sn.shaderList.size();
	MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
	MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
	OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shaderGroupName.asChar());
	
	MObject thisMObject = depFn.object();
	MString outPlugName;
	MString connectedObjectName = getObjectName(getOtherSideSourceNode(attributeName, thisMObject, true, outPlugName));

	logger.debug(MString("getOslTexMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = sn.shaderList[shadingNodeId];
		logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		createOSLHelperNodes(sn.shaderList[shadingNodeId]);
		this->createOSLShadingNode(sn.shaderList[shadingNodeId]);

		if (snode.fullName == connectedObjectName.asChar())
		{
			createOSLHelperNodes(sn.shaderList[sn.shaderList.size() - 1]);
			logger.debug(MString("connected node found: ") + snode.fullName + " search output attr.");
			
			for (size_t outId = 0; outId < snode.outputAttributes.size(); outId++)
			{
				ShaderAttribute& sa = snode.outputAttributes[outId];
				if (MString(sa.name.c_str()) == outPlugName)
				{
					logger.debug(MString("connected out attr found: ") + sa.name.c_str() + " ");
					MString destParam;
					MString sourceParam = outPlugName;
					MString sourceNode = connectedObjectName;
					if (sa.type == "color")
					{
						// lets see if we have a color helper node
						MString helperNodeName = createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - color attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (doesHelperNodeExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
						}
						destParam = "inColor";
					}
					if (sa.type == "float")
					{
						destParam = "inFloat";
					}
					if (sa.type == "int")
					{
						destParam = "inInt";
					}
					if (sa.type == "bool")
					{
						destParam = "inBool";
					}
					if (sa.type == "vector")
					{
						destParam = "inVector";
						MString helperNodeName = createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - vector attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (doesHelperNodeExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
						}
					}

					logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
					bool success = this->oslRenderer.shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
					logger.debug(MString("connecting ") + sourceNode + "." + sourceParam + " -> " + OSLInterfaceName + "." + destParam);
					success = this->oslRenderer.shadingsys->ConnectShaders(sourceNode.asChar(), sourceParam.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
					break;
				}
			}
			break;
		}
	}
	if (!this->oslRenderer.shadingsys->ShaderGroupEnd())
	{
		logger.debug("Problem finishing shader group");
	}
	std::string serialized;
	this->oslRenderer.shadingsys->getattribute(shaderGroup.get(), "pickle", serialized);
	logger.debug(MString("Serialized: ") + serialized.c_str());

	OSLMap *oslMap = new OSLMap;
	oslMap->coronaRenderer = this;
	oslMap->shaderGroup = shaderGroup;

	if (attributeName == "bump")
	{
		oslMap->bumpType = OSLMap::NONE;
		// we only support direct bumpmap connections
		MPlug ncPlug = depFn.findPlug("normalCamera");
		if (ncPlug.isConnected())
		{
			MPlugArray pa;
			ncPlug.connectedTo(pa, true, false);
			if (pa.length() > 0)
			{
				for (uint pId = 0; pId < pa.length(); pId++)
				{
					if (pa[pId].node().hasFn(MFn::kBump) || pa[pId].node().hasFn(MFn::kBump3d))
					{
						MFnDependencyNode bumpFn(pa[pId].node());
						MPlug interpPlug = bumpFn.findPlug("bumpInterp");
						if (interpPlug.asInt() == 0)
							oslMap->bumpType = OSLMap::BUMP;
						if (interpPlug.asInt() == 1)
							oslMap->bumpType = OSLMap::NORMALTANGENT;
						if (interpPlug.asInt() == 2)
							oslMap->bumpType = OSLMap::NORMALOBJECT;
					}
				}
			}
		}
	}

	return oslMap;
}

void CoronaRenderer::defineAttribute(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com, ShadingNetwork& sn)
{
	Corona::Abstract::Map *texmap = NULL;
	Corona::Rgb rgbColor(0.0);
	logger.debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if( isConnected(attributeName.asChar(), depFn, true, true))
	{
		logger.debug(MString("it is connected"));
		texmap = getOslTexMap(attributeName, depFn, sn);
		//size_t numNodes = sn.shaderList.size();
		//MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
		//MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
		//OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shaderGroupName.asChar()); 		
		//// we only need to define the shading nodes from start to the connected node
		//MObject thisMObject = depFn.object();
		//MString connectedObjectName = getObjectName(getOtherSideNode(attributeName, thisMObject));
		//MString outPlugName;
		//getOtherSideNode(attributeName, thisMObject, outPlugName);
		//
		//logger.debug(MString("defineColorOrMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
		//for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
		//{
		//	ShadingNode snode = sn.shaderList[shadingNodeId];
		//	logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		//	this->createOSLShadingNode(sn.shaderList[shadingNodeId]);

		//	if( snode.fullName == connectedObjectName.asChar())
		//	{
		//		logger.debug(MString("connected node found: ") + snode.fullName + " search output attr.");
		//		
		//		for( size_t outId = 0; outId < snode.outputAttributes.size(); outId++)
		//		{
		//			ShaderAttribute& sa = snode.outputAttributes[outId];
		//			if( MString(sa.name.c_str()) == outPlugName )
		//			{
		//				logger.debug(MString("connected out attr found: ") + sa.name.c_str() + " ");
		//				MString destParam;
		//				if( sa.type == "color" )
		//				{
		//					destParam = "inColor";
		//				}
		//				if( sa.type == "float" )
		//				{
		//					destParam = "inFloat";
		//				}
		//				if( sa.type == "int" )
		//				{
		//					destParam = "inInt";
		//				}
		//				if( sa.type == "bool" )
		//				{
		//					destParam = "inBool";
		//				}
		//				if( sa.type == "vector" )
		//				{
		//					destParam = "inVector";
		//				}
		//				logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
		//				bool success = this->oslRenderer.shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
		//				logger.debug(MString("connecting ") + connectedObjectName + "." + outPlugName + " -> " + OSLInterfaceName + "." + destParam);
		//				success = this->oslRenderer.shadingsys->ConnectShaders(connectedObjectName.asChar(), outPlugName.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
		//				break;
		//			}
		//		}
		//		break;
		//	}
		//}
		//if(!this->oslRenderer.shadingsys->ShaderGroupEnd())
		//{
		//	logger.debug("Problem finishing shader group");
		//}
		//OSLMap *oslMap = new OSLMap;
		//oslMap->coronaRenderer = this;
		//oslMap->shaderGroup = shaderGroup;
		//texmap = oslMap;
	}else{
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_COLOR )
		{
			MColor col = getColorAttr(attributeName.asChar(), depFn);
			rgbColor = Corona::Rgb(col.r,col.g,col.b);		
		}
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_FLOAT )
		{
			float f = getFloatAttr(attributeName.asChar(), depFn, 0.0f);
			rgbColor = Corona::Rgb(f, f, f);		
		}
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::Abstract::Map **bumpMap)
{
	Corona::Abstract::Map *texmap = NULL;
	if( isConnected("normalCamera", depFn, true, false))
	{
		logger.debug(MString("normal camera is connected"));
		texmap = getOslTexMap(attributeName, depFn, sn);
		logger.debug("Bump connected");
		*bumpMap = texmap;
	}
}

void CoronaRenderer::defineColor(MString& attributeName, MFnDependencyNode& depFn, Corona::Rgb& com)
{
	MColor col(0,0,0);
	getColor(attributeName, depFn, col);
	Corona::Rgb rgbColor = Corona::Rgb(col.r,col.g,col.b);
	com = rgbColor;
}

void CoronaRenderer::defineFloat(MString& attributeName, MFnDependencyNode& depFn, float& com)
{
	float f = 0.0f;
	getFloat(attributeName, depFn, f);
	com = f;
}

bool CoronaRenderer::assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj)
{
	int index = -1;
	for( size_t i = 0; i < shaderArray.size(); i++)
	{
		if( shaderArray[i] == shadingGroup)
		{
			index = i;
			break;
		}
	}
	if( index > -1)
	{
		logger.info(MString("Reusing material data."));
		Corona::IMaterial *mat = dataArray[index].createMaterial();
		obj->instance->addMaterial(Corona::IMaterialSet(mat));
		return true;
	}
	return false;
}

void CoronaRenderer::clearMaterialLists()
{
	shaderArray.clear();
	dataArray.clear();
}

void CoronaRenderer::defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{
	Corona::NativeMtlData data;
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
		
	if( obj->shadingGroups.length() > 0)
	{
		
		if(assingExistingMat(obj->shadingGroups[0], obj))
			return;

		Material mat(obj->shadingGroups[0]);
		size_t numNodes = mat.surfaceShaderNet.shaderList.size();
		
		if( numNodes > 0)
		{
			// give me the last node in the node list, this should be the surface shader
			ShadingNode ss = mat.surfaceShaderNet.shaderList.back();
			//ShadingNode ss = mat.surfaceShaderNet.shaderList[ mat.surfaceShaderNet.shaderList.size() - 1];
			logger.debug(MString("Found surface shader ") + ss.fullName);
			MColor colorVal;
			
			Corona::Rgb rgbColor;
			Corona::ColorOrMap com;

			MFnDependencyNode depFn(ss.mobject);
			if( ss.typeName == "CoronaSurface")
			{
				MColor overrideColor(1,1,1);
				if( obj->attributes != NULL)
					if( obj->attributes->hasColorOverride)
						overrideColor = obj->attributes->colorOverride;
				
				//this->defineColorOrMap(MString("diffuse"), depFn, data.components.diffuse);
				this->defineAttribute(MString("diffuse"), depFn, data.components.diffuse, mat.surfaceShaderNet);

				//this->defineColorOrMap(MString("translucency"), depFn, data.components.translucency);				
				this->defineAttribute(MString("translucency"), depFn, data.components.translucency, mat.surfaceShaderNet);
				
				//this->defineColorOrMap(MString("reflectivity"), depFn, data.components.reflect);
				this->defineAttribute(MString("reflectivity"), depFn, data.components.reflect, mat.surfaceShaderNet);

				const Corona::BrdfLobeType brdfs[] = {Corona::BRDF_ASHIKHMIN, Corona::BRDF_FAKE_WARD, Corona::BRDF_PHONG, Corona::BRDF_WARD};
				int id;
				getEnum(MString("brdfType"), depFn, id);
				data.reflect.brdfType = brdfs[id];

				//this->defineFloatOrMap(MString("reflectionGlossiness"), depFn, data.reflect.glossiness);
				this->defineAttribute(MString("reflectionGlossiness"), depFn, data.reflect.glossiness, mat.surfaceShaderNet);
				//this->defineFloatOrMap(MString("fresnelIor"), depFn, data.reflect.fresnelIor);	
				this->defineAttribute(MString("fresnelIor"), depFn, data.reflect.fresnelIor, mat.surfaceShaderNet);
				//this->defineFloatOrMap(MString("anisotropy"), depFn, data.reflect.anisotropy);	
				this->defineAttribute(MString("anisotropy"), depFn, data.reflect.anisotropy, mat.surfaceShaderNet);
				//this->defineFloatOrMap(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation);	
				this->defineAttribute(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation, mat.surfaceShaderNet);
				
				//this->defineColorOrMap(MString("refractivity"), depFn, data.components.refract);
				this->defineAttribute(MString("refractivity"), depFn, data.components.refract, mat.surfaceShaderNet);
				//-1
				//this->defineFloatOrMap(MString("refractionIndex"), depFn, data.refract.ior);				
				this->defineAttribute(MString("refractionIndex"), depFn, data.refract.ior, mat.surfaceShaderNet);
				//this->defineFloatOrMap(MString("refractionGlossiness"), depFn, data.refract.glossiness);	
				this->defineAttribute(MString("refractionGlossiness"), depFn,  data.refract.glossiness, mat.surfaceShaderNet);

				// -- round corners -- 
				float rcRadius = 0.0001;
				getFloat(MString("roundCornersRadius"), depFn, rcRadius);
				data.roundedCorners.radius = rcRadius * this->mtco_renderGlobals->scaleFactor;
				getInt(MString("roundCornersSamples"), depFn, data.roundedCorners.samples);

				int glassMode = 0;
				getEnum(MString("glassMode"), depFn, glassMode);
				Corona::GlassMode glassModes[] = {Corona::GLASS_ONESIDED, Corona::GLASS_TWOSIDED, Corona::GLASS_HYBRID};
				data.refract.glassMode = glassModes[glassMode];

				// --- volume ----
				this->defineColor(MString("attenuationColor"), depFn, data.volume.attenuationColor);				
				this->defineFloat(MString("attenuationDist"), depFn, data.volume.attenuationDist);				
				this->defineColor(MString("volumeEmissionColor"), depFn, data.volume.emissionColor);				
				this->defineFloat(MString("volumeEmissionDist"), depFn, data.volume.emissionDist);				

				// ---- emission ----
				//this->defineColorOrMap(MString("emissionColor"), depFn, data.emission.color);	
				this->defineAttribute(MString("emissionColor"), depFn, data.emission.color, mat.surfaceShaderNet);
				
				// ---- ies profiles -----
				MStatus stat;
				MPlug iesPlug = depFn.findPlug("mtco_mat_iesProfile", &stat);
				if( stat )
				{
					//data.emission.gonioDiagram
					MString iesFile = iesPlug.asString();
					if( iesFile.length() > 4 )
					{
						Corona::IesParser iesParser;
						//std::ifstream input(iesFile.asChar());
						Corona::FileReader input;
						Corona::String fn(iesFile.asChar());
						input.open(fn);
						if( !input.failed() )
						{
						
							try {

								const double rm[4][4] = {
									{1.0, 0.0, 0.0, 0.0}, 
									{0.0, 0.0, 1.0, 0.0}, 
									{0.0,-1.0, 0.0, 0.0}, 
									{0.0, 0.0, 0.0, 1.0} 							
								};
								MMatrix zup(rm);

								MMatrix tm = zup * obj->transformMatrices[0];
								Corona::AnimatedAffineTm atm;
								setAnimatedTransformationMatrix(atm, tm);
								
								const Corona::IesLight light = iesParser.parseIesLight(input);
								data.emission.gonioDiagram = light.distribution;
								Corona::Matrix33 m(atm[0].extractRotation());
							
								data.emission.emissionFrame = Corona::AnimatedMatrix33(m);

							} catch (Corona::Exception& ex) {
								logger.error(MString(ex.getMessage().cStr()));
							}
						}else{
							logger.error(MString("Unable to read ies file .") + iesFile);
						}
					}
				}

				bool disableSampling = false;
				getBool("emissionDisableSampling", depFn, disableSampling);
				data.emission.disableSampling = disableSampling;

				bool sharpnessFake = false;
				getBool("emissionSharpnessFake", depFn, sharpnessFake);
				data.emission.sharpnessFake = sharpnessFake;
				
				MVector point(0,0,0);
				getPoint(MString("emissionSharpnessFakePoint"), depFn, point);
				data.emission.sharpnessFakePoint = Corona::AnimatedPos(Corona::Pos(point.x, point.y, point.z));			

				defineBump(MString("bump"), depFn, mat.surfaceShaderNet, &data.bump);

				shaderArray.push_back(obj->shadingGroups[0]);
				dataArray.push_back(data);

			}else if(ss.typeName == "lambert"){
				getColor("color", depFn, colorVal);
				data.components.diffuse.setColor(Corona::Rgb(colorVal.r,colorVal.g,colorVal.b));
			}else{
				data.components.diffuse.setColor(Corona::Rgb(.2, .2, 1.0));
			}
		}else{
			data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
		}

		MFnDependencyNode sn;

	}else{
		data.components.diffuse.setColor(Corona::Rgb(.7, .7, .7));
	}
		
	Corona::IMaterial *mat = data.createMaterial();
	obj->instance->addMaterial(Corona::IMaterialSet(mat));
}


void CoronaRenderer::createOSLShadingNode(ShadingNode& snode)
{
	MFnDependencyNode depFn(snode.mobject);	
	
	// We cannot create a parameter and connect it in one go because for connections we need a shader. 
	// So we first get all input parameters, then we create the shader and last we connect the connected inputs.
	for( uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.inputAttributes[i];
		if( sa.type == "string" )
		{
			const OIIO::string_ref p = getString(sa.name.c_str(), depFn).asChar();
			this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeString, &p);
		}
		if( sa.type == "float" )
		{
			float value = getFloatAttr(sa.name.c_str(), depFn, 0.0f);
			this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeFloat, &value);
		}

		// if we have connected children, we always have a floatToColor node with name node_att_floatToColor
		if( sa.type == "color" )
		{
			float color[3];
			MString attrName = sa.name.c_str();
			getColor(sa.name.c_str(), depFn, color);
			if( sa.name == "color" )
				attrName = "inColor";
			this->oslRenderer.shadingsys->Parameter(attrName.asChar(), OSL::TypeDesc::TypeColor, color);
		}
		if( sa.type == "int" )
		{
			int value = getIntAttr(sa.name.c_str(), depFn, 0);
			this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeInt, &value);
		}
		if( sa.type == "bool" )
		{
			int value = (int)getBoolAttr(sa.name.c_str(), depFn, false);
			this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeInt, &value);
		}
		// if we have connected children, we always have a floatToColor node with name node_att_floatToVector
		if (sa.type == "vector")
		{
			float vec[3];
			MVector vector;
			MString attrName = sa.name.c_str();
			getVector(MString(sa.name.c_str()), depFn, vector);
			if (sa.name == "vector")
				attrName = "inVector";
			vec[0] = vector.x;
			vec[1] = vector.y;
			vec[2] = vector.z;
			this->oslRenderer.shadingsys->Parameter(attrName.asChar(), OSL::TypeDesc::TypeVector, vec);
		}
	}

	logger.debug(MString("Register shader type") + snode.typeName + " named " + snode.fullName );
	bool success;
	success = this->oslRenderer.shadingsys->Shader("surface", snode.typeName.asChar(), snode.fullName.asChar());
	if( !success )
	{
		logger.debug(MString("Failed to register shader ") + snode.fullName );
	}


	// create connections
	// we have to possibilites: either the node is directly connected, or a component is connected
	// there is no way to connect components directly in OSL so if we have a connected component we have created a 
	// helper node previously to handle the component connection.
	for( uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		if (isConnected(snode.inputAttributes[i].name.c_str(), snode.mobject, true))
		{
			std::string destAttr = snode.inputAttributes[i].name;
			std::string destNode = snode.fullName.asChar();
			std::string sourceAttr = "";
			std::string sourceNode = "";
			MPlugArray pa;
			getConnectedInPlugs(snode.mobject, pa);
			for (uint pId = 0; pId < pa.length(); pId++)
			{
				MPlug connectedInPlug = pa[pId];
				if (getAttributeNameFromPlug(connectedInPlug) != snode.inputAttributes[i].name.c_str())
					continue;

				// if we have a valid child connection, then we have a helper node
				// so we handle here only direct connections
				if (!connectedInPlug.isChild())
				{
					MPlugArray cpa;
					connectedInPlug.connectedTo(cpa, true, false);
					if (cpa.length() > 0)
					{
						sourceAttr = getAttributeNameFromPlug(cpa[0]).asChar();
						sourceNode = getObjectName(cpa[0].node()).asChar();						
					}
				}
			}
			
			MString helperNodeName = createPlugHelperNodeName(snode.inputAttributes[i].name.c_str(), snode.mobject, false);
			if (doesHelperNodeExist(helperNodeName))
			{
				sourceNode = helperNodeName.asChar();
				sourceAttr = "outputValue";
			}
			if (destAttr == "color")
				destAttr = "inColor";
			if (destAttr == "vector")
				destAttr = "inVector";
			
			logger.debug(MString("Connect  ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
			success = this->oslRenderer.shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
		}
	}
	
}

