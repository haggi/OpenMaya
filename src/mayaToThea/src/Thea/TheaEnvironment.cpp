#include "Thea.h"
#include <maya/MFnDependencyNode.h>
#include "../mtth_common/mtth_mayaScene.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include <SDK/XML/sdk.xml.environment.h>
#include <SDK/Integration/sdk.basics.h>


void TheaRenderer::defineIBLNode(TheaSDK::XML::EnvironmentOptions::IBLMap& iblMap, const char *attName)
{
	MObject fileNodeObject = getConnectedFileTextureObject(MString(attName),MFnDependencyNode(this->mtth_renderGlobals->renderGlobalsMobject));
	if(fileNodeObject != MObject.kNullObj)
	{
		MFnDependencyNode fileNode(fileNodeObject);
		MString fileName;
		getString(MString("textureFileName"), fileNode, fileName); 
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

void TheaRenderer::defineEnvironment()
{

	if( this->mtth_renderGlobals->exportSceneFile )
	{	
		TheaSDK::XML::EnvironmentOptions env;		
		switch(this->mtth_renderGlobals->illumination)
		{
		case 0:
			break;
		case 1: // dome light
			{
				env.illumination = TheaSDK::DomeIllumination;
				TheaSDK::Rgb bgColor(this->mtth_renderGlobals->backgroundColor.r,this->mtth_renderGlobals->backgroundColor.g,this->mtth_renderGlobals->backgroundColor.b);
				env.backgroundColor = bgColor;
			}
			break;
		case 2: // physical sky
			{
				env.illumination = TheaSDK::PhysicalSkyIllumination;
				env.turbidity = this->mtth_renderGlobals->turbidity;
				env.ozone = this->mtth_renderGlobals->ozone;
				env.waterVapor = this->mtth_renderGlobals->waterVapor;
				env.turbidityCoefficient = this->mtth_renderGlobals->turbidityCoefficient;
				env.wavelengthExponent = this->mtth_renderGlobals->wavelengthExponent;
				env.albedo = this->mtth_renderGlobals->albedo;	

				env.location = this->mtth_renderGlobals->location.asChar();
				env.latitude = this->mtth_renderGlobals->latitude;
				env.longitude = this->mtth_renderGlobals->longitude;
				env.timezone = this->mtth_renderGlobals->timezone; // from -12 to +12.
				env.date = this->mtth_renderGlobals->date.asChar(); // format dd/mm/yy.
				env.localtime = this->mtth_renderGlobals->localtime.asChar(); // format hh/mm/ss.

				env.sunDirection = TheaSDK::Normal3D(this->mtth_renderGlobals->sunDirection.x,this->mtth_renderGlobals->sunDirection.y,this->mtth_renderGlobals->sunDirection.z);
				env.sunPolarAngle = this->mtth_renderGlobals->sunPolarAngle; // in degrees.
				env.sunAzimuth = this->mtth_renderGlobals->sunAzimuth; // in degrees.
			}
			break;
		case 3: // ibl
			{
				env.illumination = TheaSDK::IBLIllumination;
				defineIBLNode(env.illuminationMap, "illuminationMap");
				defineIBLNode(env.backgroundMap, "backgroundMap");
				defineIBLNode(env.reflectionMap, "reflectionMap");
				defineIBLNode(env.refractionMap, "refractionMap");
			}
			break;
		default:
		};
		if(this->mtth_renderGlobals->illumination > 0)
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

