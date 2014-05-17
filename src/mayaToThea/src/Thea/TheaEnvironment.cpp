#include "Thea.h"
#include "../mtth_common/mtth_mayaScene.h"

void TheaRenderer::defineEnvironment()
{

	if (TheaSDK::SetRgbParameter(TheaSDK::GetGlobalSettings(),"Background Color",TheaSDK::Rgb(0.1f,0.1f,0.6f))==false)
		return;
	if (TheaSDK::SetStringParameter(TheaSDK::GetGlobalSettings(),"Background Type","Background Color")==false)
		return;

	if( this->mtth_renderGlobals->currentFrame == 2.0 )
	{
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

