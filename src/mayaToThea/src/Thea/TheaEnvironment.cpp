#include "Thea.h"
#include <maya/MFnDependencyNode.h>
#include "mayaScene.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include <SDK/XML/sdk.xml.environment.h>
#include <SDK/Integration/sdk.basics.h>
#include "world.h"

static Logging logger;

void TheaRenderer::defineIBLNode(TheaSDK::XML::EnvironmentOptions::IBLMap& iblMap, const char *attName)
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	MObject fileNodeObject = getConnectedFileTextureObject(MString(attName), depFn);
	if(fileNodeObject != MObject::kNullObj)
	{
		MFnDependencyNode fileNode(fileNodeObject);
		MString fileName;
		getString(MString("fileTextureName"), fileNode, fileName); 
		if( fileName.length() > 1)
		{	
			iblMap.bitmapFilename = fileName.asChar();
			float intensity = 0.0f, rotation = 0.0f;
			getFloat("mtth_file_iblIntensity", fileNode, intensity);
			iblMap.intensity = intensity;
			getFloat("mtth_file_iblRotation", fileNode, rotation);
			iblMap.rotation = rotation;
			int wrapping = 0;
			getEnum(MString("mtth_file_iblWrapping"), fileNode, wrapping);
			iblMap.wrapping = (TheaSDK::IBLWrapping)wrapping;
			iblMap.enabled = true;
			return;
		}
	}
	iblMap.enabled = false;
}

TheaSDK::Normal3D TheaRenderer::getSunDirection()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	TheaSDK::Normal3D sunDir;

	MObjectArray nodeList;
	getConnectedInNodes(MString("sunLightConnection"), getRenderGlobalsNode(), nodeList);
	if( nodeList.length() > 0)	
	{
		MVector lightDir(0,0,1);
		MFnDagNode sunDagNode(nodeList[0]);
		//lightDir *= this->mtth_renderGlobals->globalConversionMatrix.inverse();
		lightDir *= sunDagNode.transformationMatrix();
		lightDir *= renderGlobals->globalConversionMatrix;
		lightDir.normalize();
		return TheaSDK::Normal3D(lightDir.x,lightDir.y,lightDir.z);
	}
	float sunDirX = 0.0f, sunDirY = 0.0f, sunDirZ = 1.0f;
	MPlug sunDirPlug = depFn.findPlug("sunDirection");
	if (!sunDirPlug.isNull())
	{
		sunDirX = sunDirPlug.child(0).asFloat();
		sunDirY = sunDirPlug.child(1).asFloat();
		sunDirZ = sunDirPlug.child(2).asFloat();
	}
	return TheaSDK::Normal3D(sunDirX, sunDirY, sunDirZ);
}

void TheaRenderer::defineEnvironment()
{
	MFnDependencyNode gFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	if( renderGlobals->exportSceneFile )
	{	
		TheaSDK::XML::EnvironmentOptions env;	
		int illumination = getEnumInt("illumination", gFn);
		switch (illumination)
		{
		case 0:
			break;
		case 1: // dome light
			{
				env.illumination = TheaSDK::DomeIllumination;
				MColor bg = getColorAttr("backgroundColor", gFn);
				TheaSDK::Rgb bgColor(bg.r, bg.g, bg.b);
				env.backgroundColor = bgColor;
			}
			break;
		case 2: // ibl
			{
				env.illumination = TheaSDK::IBLIllumination;
				defineIBLNode(env.illuminationMap, "illuminationMap");
				defineIBLNode(env.backgroundMap, "backgroundMap");
				defineIBLNode(env.reflectionMap, "reflectionMap");
				defineIBLNode(env.refractionMap, "refractionMap");
			}
			break;
		case 3: // physical sky
			{
				env.illumination = TheaSDK::PhysicalSkyIllumination;
				env.turbidity = getFloatAttr("turbidity", gFn, 2.5f);
				env.ozone = getFloatAttr("ozone", gFn, 0.35f);
				env.waterVapor = getFloatAttr("waterVapor", gFn, 2.0f);
				env.turbidityCoefficient = getFloatAttr("turbidityCoefficient", gFn, .046f);
				env.wavelengthExponent = getFloatAttr("wavelengthExponent", gFn, 1.3f);
				env.albedo = getFloatAttr("albedo", gFn, .5f);

				env.location = getStringAttr("location", gFn, "").asChar();

				env.timezone = getIntAttr("timezone", gFn, 0); // from -12 to +12.
				env.date = getStringAttr("date", gFn, "").asChar(); // format dd/mm/yy.
				env.localtime = getStringAttr("localtime", gFn, "").asChar(); // format hh/mm/ss.

				env.latitude = getFloatAttr("latitude", gFn, .0f);
				env.longitude = getFloatAttr("longitude", gFn, .0f);

				env.sunDirection = this->getSunDirection();
				env.sunPolarAngle = getFloatAttr("sunPolarAngle", gFn, -1.0f); // in degrees.
				env.sunAzimuth = getFloatAttr("sunAzimuth", gFn, -1.0f); // in degrees.
			}
			break;
		default:
			break;				
		};

		if (illumination > 0)
		{
			this->sceneXML.setEnvironmentOptions(env);
		}

	}else{
		if (TheaSDK::SetRgbParameter(TheaSDK::GetGlobalSettings(),"Background Color",TheaSDK::Rgb(0.1f,0.1f,0.6f))==false)
			return;
		if (TheaSDK::SetStringParameter(TheaSDK::GetGlobalSettings(),"Background Type","Background Color")==false)
			return;

		if (TheaSDK::SetRealParameter(TheaSDK::GetGlobalSettings(),"./Sun/Polar Angle (deg)",45)==false)
			return ;
		if (TheaSDK::SetRealParameter(TheaSDK::GetGlobalSettings(),"./Sun/Azimuth (deg)",45)==false)
			return;
		if (TheaSDK::SetStringParameter(TheaSDK::GetGlobalSettings(),"Illumination","Physical Sky")==false)
			return;
		if (TheaSDK::SetIntegerParameter(TheaSDK::GetRootObject(),"GenerateSun",2)==false)
			return;
	}
}

