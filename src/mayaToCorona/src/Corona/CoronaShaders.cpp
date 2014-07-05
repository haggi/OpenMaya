#include "Corona.h"
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

void CoronaRenderer::defineAttribute(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com, ShadingNetwork& sn)
{
	Corona::Abstract::Map *texmap = NULL;
	Corona::Rgb rgbColor(0.0);
	logger.debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if( isConnected(attributeName.asChar(), depFn))
	{
		logger.debug(MString("it is connected"));
		size_t numNodes = sn.shaderList.size();
		MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
		MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
		OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shaderGroupName.asChar()); 		
		// we only need to define the shading nodes from start to the connected node
		MObject thisMObject = depFn.object();
		MString connectedObjectName = getObjectName(getOtherSideNode(attributeName, thisMObject));
		MString outPlugName;
		getOtherSideNode(attributeName, thisMObject, outPlugName);
		
		logger.debug(MString("defineColorOrMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
		for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
		{
			ShadingNode snode = sn.shaderList[shadingNodeId];
			logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
			this->createOSLShadingNode(sn.shaderList[shadingNodeId]);

			if( snode.fullName == connectedObjectName.asChar())
			{
				logger.debug(MString("connected node found: ") + snode.fullName + " search output attr.");
				
				for( size_t outId = 0; outId < snode.outputAttributes.size(); outId++)
				{
					ShaderAttribute& sa = snode.outputAttributes[outId];
					if( MString(sa.name.c_str()) == outPlugName )
					{
						logger.debug(MString("connected out attr found: ") + sa.name.c_str() + " ");
						MString destParam;
						if( sa.type == "color" )
						{
							destParam = "inColor";
						}
						if( sa.type == "float" )
						{
							destParam = "inFloat";
						}
						if( sa.type == "int" )
						{
							destParam = "inInt";
						}
						if( sa.type == "bool" )
						{
							destParam = "inBool";
						}
						if( sa.type == "vector" )
						{
							destParam = "inVector";
						}
						logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
						bool success = this->oslRenderer.shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
						logger.debug(MString("connecting ") + connectedObjectName + "." + outPlugName + " -> " + OSLInterfaceName + "." + destParam);
						success = this->oslRenderer.shadingsys->ConnectShaders(connectedObjectName.asChar(), outPlugName.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
						break;
					}
				}
				break;
			}
		}
		if(!this->oslRenderer.shadingsys->ShaderGroupEnd())
		{
			logger.debug("Problem finishing shader group");
		}
		OSLMap *oslMap = new OSLMap;
		oslMap->coronaRenderer = this;
		oslMap->shaderGroup = shaderGroup;
		texmap = oslMap;
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

void CoronaRenderer::defineBump(MString& attributeName, MFnDependencyNode& depFn, Corona::Abstract::Map *bumpMap)
{
	Corona::Abstract::Map *texmap = NULL;
	if( isConnected("normalCamera", depFn))
	{
		logger.debug("Bump connected");
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


				defineBump(MString("bump"), depFn, data.bump);

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

		// if we 
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
	}

	logger.debug(MString("Register shader type") + snode.typeName + " named " + snode.fullName );
	bool success;
	if( snode.typeName == "bump2d")
		success = this->oslRenderer.shadingsys->Shader("displacement", snode.typeName.asChar(), snode.fullName.asChar());
	else
		success = this->oslRenderer.shadingsys->Shader("surface", snode.typeName.asChar(), snode.fullName.asChar());
	if( !success )
	{
		logger.debug(MString("Failed to register shader ") + snode.fullName );
	}

	
	for( uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		if(snode.inputAttributes[i].connected)
		{
			logger.debug(MString("Attribute ") + snode.inputAttributes[i].name.c_str() + " is connected from " + snode.inputAttributes[i].connectedAttrName.c_str());
			std::string sourceShader = snode.inputAttributes[i].connectedNodeName;
			std::string sourceParam = snode.inputAttributes[i].connectedAttrName;
			std::string destShader = snode.fullName.asChar();
			std::string destParam = snode.inputAttributes[i].name;
			if( destParam == "color" )
				destParam = "inColor";
			logger.debug(MString("Connect  ") + sourceShader.c_str() + "." + sourceParam.c_str() + " --> " + destShader.c_str() + "." + destParam.c_str());
			success = this->oslRenderer.shadingsys->ConnectShaders(sourceShader.c_str(), sourceParam.c_str(), destShader.c_str(), destParam.c_str());
			if( !success )
			{
				logger.debug(MString("Failed to connect shaders "));
			}
		}
	}
	
}

//void CoronaRenderer::defineOSLMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
//{

	//logger.debug(MString("\n\n----------------- defineOSLMaterial -----------------------"));		
	//for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	//{
	//	MObject shadingGroup = obj->shadingGroups[sgId];
	//	MString shadingGroupName = getObjectName(shadingGroup);
	//	logger.debug(MString("defineOSLMaterial from shadingGroup: ") + shadingGroupName);		
	//	Material material(shadingGroup);
	//	int numNodes = (int)material.surfaceShaderNet.shaderList.size();
	//	if( numNodes > 0)
	//	{
	//		ShadingNode ss = material.surfaceShaderNet.shaderList.back();
	//		if( ss.fullName != "CoronaOSLMat")
	//			continue;

	//		Corona::NativeMtlData data;

	//		logger.debug(MString("CoronaOSLMat shader: ") + ss.fullName);

	//		OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shadingGroupName.asChar()); 
	//		
	//		for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
	//		{
	//			ShadingNode sn = material.surfaceShaderNet.shaderList[shadingNodeId];
	//			logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + sn.fullName);
	//			//this->createCoronaShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId]);
	//			this->createOSLShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId]);
	//		}
	//		if(!this->oslRenderer.shadingsys->ShaderGroupEnd())
	//		{
	//			logger.debug("Problem finishing shader group");
	//		}
	//		OSLMap *oslMap = new OSLMap;
	//		oslMap->coronaRenderer = this;
	//		oslMap->shaderGroup = shaderGroup;
	//		data.components.diffuse.setMap(oslMap);
	//		//getColor("color", depFn, colorVal);
	//		//data.components.diffuse.setColor(Corona::Rgb(colorVal.r,colorVal.g,colorVal.b));
	//		Corona::IMaterial *mat = data.createMaterial();
	//		obj->instance->addMaterial(Corona::IMaterialSet(mat));
	//	}
	//}
	//logger.debug(MString("----------------- defineOSLMaterial done-------------------\n\n"));		
//}