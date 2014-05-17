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


#include "CoronaMap.h"

static Logging logger;

static std::vector<Corona::NativeMtlData> dataArray;
static std::vector<MObject> shaderArray;


// check if an oslNode is connected to an attribute, if yes try to create a oslMap
bool CoronaRenderer::isOSLConnected(MString attributeName, MFnDependencyNode& depFn)
{
	MStatus stat = MS::kSuccess;
	MPlug attrPlug = depFn.findPlug(attributeName, &stat);
	if( !stat )
		return false;
	MObject otherSideNode = getOtherSideNode(attrPlug);
	if( otherSideNode == MObject::kNullObj)
		return false;
	MFnDependencyNode otherSideFn(otherSideNode);
	if(otherSideFn.typeName() == "CoronaOSL")
		return true;
	return false;
}

void CoronaRenderer::defineOSLShaders(Corona::IInstance* instance, mtco_MayaObject *obj)
{
				//OSLMap *oslMap = new OSLMap;
				//oslMap->coronaRenderer = this;
				//data.components.diffuse.setMap(oslMap);
}


void CoronaRenderer::defineColorOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com)
{
	MapLoader loader;
	//mtco_MapLoader loader;
	Corona::Abstract::Map *texmap = NULL;
	MColor col(0,0,0);
	MString fileTexturePath = "";
	getColor(attributeName, depFn, col);
	Corona::Rgb rgbColor = Corona::Rgb(col.r,col.g,col.b);
	MObject fileTextureObject;
	if(getConnectedFileTexturePath(attributeName, depFn.name(), fileTexturePath, fileTextureObject))
	{
		//MFnDependencyNode ftn(fileTextureObject);
		//MColor gain, offset;
		//getColor("colorGain", ftn, gain);
		//getColor("colorOffset", ftn, offset);
		//loader.colorGain = gain;
		//loader.colorOffset = offset;

		if( textureFileSupported(fileTexturePath))
		{
			texmap = loader.loadBitmap(fileTexturePath.asChar());
		}else{
			texmap = NULL;
			logger.error(MString("File texture extension is not supported: ") + fileTexturePath);
			col = MColor(1, 0, 1);
		}
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineFloatOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com)
{
	MapLoader loader;
	Corona::Abstract::Map *texmap = NULL;
	float f;
	MString fileTexturePath = "";
	getFloat(attributeName, depFn, f);
	Corona::Rgb rgbColor = Corona::Rgb(f,f,f);
	MObject fileTextureObject;
	if(getConnectedFileTexturePath(attributeName, depFn.name(), fileTexturePath, fileTextureObject))
	{
		if( textureFileSupported(fileTexturePath))
			texmap = loader.loadBitmap(fileTexturePath.asChar());
		else{
			texmap = NULL;
			logger.error(MString("File texture extension is not supported: ") + fileTexturePath);
			rgbColor.r() = 1.0;
			rgbColor.g() = 0.0;
			rgbColor.b() = 1.0;
		}
	}else{
		texmap = NULL;
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
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

		if( mat.surfaceShaderNet.shaderList.size() > 0)
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
				

				if(isOSLConnected(MString("diffuse"), depFn))
				{
				}else{
					this->defineColorOrMap(MString("diffuse"), depFn, data.components.diffuse);				
				}

				this->defineColorOrMap(MString("translucency"), depFn, data.components.translucency);				
				
				this->defineColorOrMap(MString("reflectivity"), depFn, data.components.reflect);	

				const Corona::BrdfLobeType brdfs[] = {Corona::BRDF_ASHIKHMIN, Corona::BRDF_FAKE_WARD, Corona::BRDF_PHONG, Corona::BRDF_WARD};
				int id;
				getEnum(MString("brdfType"), depFn, id);
				data.reflect.brdfType = brdfs[id];
				this->defineFloatOrMap(MString("reflectionGlossiness"), depFn, data.reflect.glossiness);
				this->defineFloatOrMap(MString("fresnelIor"), depFn, data.reflect.fresnelIor);	
				this->defineFloatOrMap(MString("anisotropy"), depFn, data.reflect.anisotropy);	
				this->defineFloatOrMap(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation);	
				
				this->defineColorOrMap(MString("refractivity"), depFn, data.components.refract);
				//-1
				this->defineFloatOrMap(MString("refractionIndex"), depFn, data.refract.ior);				
				this->defineFloatOrMap(MString("refractionGlossiness"), depFn, data.refract.glossiness);	

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
				this->defineColorOrMap(MString("emissionColor"), depFn, data.emission.color);	
				
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

void CoronaRenderer::createCoronaShadingNode(ShadingNode& snode)
{
	logger.debug(MString("createCoronaShadingNode: ") + snode.fullName);
	MFnDependencyNode depFn(snode.mobject);

	if( snode.typeName == "file")
	{
		logger.debug(MString("Found file node ") + snode.fullName);
		MString fileTextureName = "";
		getString(MString("fileTextureName"), depFn, fileTextureName);
		std::string tfin = fileTextureName.asChar();
		logger.debug(MString("found texture file name: ") + tfin.c_str());		
		const OIIO::string_ref p = fileTextureName.asChar();
		this->oslRenderer.shadingsys->Parameter("fileTextureName", OSL::TypeDesc::TypeString, &p);
		bool success = this->oslRenderer.shadingsys->Shader("surface", "file", snode.fullName.asChar());
		if( !success )
		{
			logger.debug(MString("Failed to register shader ") + snode.fullName );
		}

		//// testing only -- connecting shaders
		for( uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			if(snode.inputAttributes[i].connected)
			{
				logger.debug(MString("Attribute ") + snode.inputAttributes[i].name.c_str() + " is connected from " + snode.inputAttributes[i].connectedAttrName.c_str());
				std::string sourceShader = snode.inputAttributes[i].connectedNodeName;
				std::string sourceParam = snode.inputAttributes[i].connectedAttrName;
				std::string destShader = snode.fullName.asChar();
				std::string destParam = snode.inputAttributes[i].name;
				logger.debug(MString("Connect  ") + sourceShader.c_str() + "." + sourceParam.c_str() + " --> " + destShader.c_str() + "." + destParam.c_str());
				success = this->oslRenderer.shadingsys->ConnectShaders(sourceShader.c_str(), sourceParam.c_str(), destShader.c_str(), destParam.c_str());
				if( !success )
				{
					logger.debug(MString("Failed to connect shaders "));
				}
			}
		}
	}
	if( snode.typeName == "place2dTexture")
	{
		logger.debug(MString("Found place2dTexture node ") + snode.fullName);
		float repeatU = 1.0f;
		getFloat("repeatU", depFn, repeatU);
		this->oslRenderer.shadingsys->Parameter("repeatU", OSL::TypeDesc::TypeFloat, &repeatU);
		float repeatV = 1.0f;
		getFloat("repeatV", depFn, repeatV);
		this->oslRenderer.shadingsys->Parameter("repeatV", OSL::TypeDesc::TypeFloat, &repeatV);
		bool success = this->oslRenderer.shadingsys->Shader("surface", "place2dTexture", snode.fullName.asChar());
		if( !success )
		{
			logger.debug(MString("Failed to register shader ") + snode.fullName );
		}
	}
	if( snode.typeName == "CoronaOSL")
	{
		logger.debug(MString("Found CoronaOSL node ") + snode.fullName);
		bool success = this->oslRenderer.shadingsys->Shader("surface", "CoronaOSL", snode.fullName.asChar());
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
}

void CoronaRenderer::defineOSLMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{

	logger.debug(MString("\n\n----------------- defineOSLMaterial -----------------------"));		
	for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MObject shadingGroup = obj->shadingGroups[sgId];
		MString shadingGroupName = getObjectName(shadingGroup);
		logger.debug(MString("defineOSLMaterial from shadingGroup: ") + shadingGroupName);		
		Material material(shadingGroup);
		int numNodes = (int)material.surfaceShaderNet.shaderList.size();
		if( numNodes > 0)
		{
			ShadingNode ss = material.surfaceShaderNet.shaderList.back();
			if( ss.fullName != "CoronaOSLMat")
				continue;

			Corona::NativeMtlData data;

			logger.debug(MString("CoronaOSLMat shader: ") + ss.fullName);

			OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shadingGroupName.asChar()); 
			
			for( int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++ )
			{
				ShadingNode sn = material.surfaceShaderNet.shaderList[shadingNodeId];
				logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + sn.fullName);
				this->createCoronaShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId]);
			}
			if(!this->oslRenderer.shadingsys->ShaderGroupEnd())
			{
				logger.debug("Problem finishing shader group");
			}
			OSLMap *oslMap = new OSLMap;
			oslMap->coronaRenderer = this;
			oslMap->shaderGroup = shaderGroup;
			data.components.diffuse.setMap(oslMap);
			//getColor("color", depFn, colorVal);
			//data.components.diffuse.setColor(Corona::Rgb(colorVal.r,colorVal.g,colorVal.b));
			Corona::IMaterial *mat = data.createMaterial();
			obj->instance->addMaterial(Corona::IMaterialSet(mat));
		}
	}
	logger.debug(MString("----------------- defineOSLMaterial done-------------------\n\n"));		
}