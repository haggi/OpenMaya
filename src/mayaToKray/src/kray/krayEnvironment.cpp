
#include "kraysdk/symbol/bitmap.h"
#include "../mtkr_common/mtkr_mayaObject.h"
#include "../mtkr_common/mtkr_renderGlobals.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MTransformationMatrix.h>

#include "krayRenderer.h"
#include "krayUtils.h"

#include "utilities/tools.h"
#include "utilities/logging.h"

static Logging logger;

namespace krayRender{

	void KrayRenderer::defineEnvironment()
	{
	//environment  	colorFade, 	<rgb> color, 	<double> amount;
	//	fade, 	<double> amount;
	//	gVolume, 	<texture> t2, 	<int> max_recurse, 	<double> adaptive_threshold, 	<double> max_distance, 	<double> probe_per_length, 	<double> photon_per_lenght, 	<double> photon_radius;
	//	phySky, 	<sharedObject> s2, 	<int> flags;
	//	vEffect, 	<double> intensity, 	<double> start, 	<double> step1, 	<double> step2, 	<int> n;
	//	volume, 	<texture> t2, 	<int> max_recurse, 	<double> adaptive_threshold, 	<double> max_distance, 	<double> probe_per_length, 	<double> photon_per_lenght, 	<double> photon_radius; 

		switch( this->mtkr_renderGlobals->environmentType )
		{
		case 0: // color fade
			{}
		case 1: // fade
			{}
		case 2: // gVolume
			{}
		case 3: // physSky
			{}
		case 4: // vEffekt
			{}
		case 5: // volume
			{}
		}

	}

	void KrayRenderer::defineBackground()
	{
	//background  	bitmap, 	<bitmap> b2, 	<xyz> ax, 	<xyz> az;
	//				bitmap2, 	<bitmap> b2, 	<bitmap> b3, 	<hpb> a4;
	//				color, 	<rgba> color;
	//				directionsMap;
	//				lightMap, 	<bitmap> b2, 	<hpb> a3;
	//				phySky, 	<sharedObject> s2;
	//				phySkyEnv, 	<sharedObject> s2;
	//				sky, 	<rgba> horizon, 	<rgba> zenith, 	<rgba> nadir, 	<xyz> zenith_direction;
	//				sky2, 	<rgba> horizon_zenith, 	<rgba> horizon_nadir, 	<rgba> zenith, 	<rgba> nadir, 	<xyz> zenith_direction, 	<double> skyGamma, 	<double> groundGamma;
	//				sphereMap, 	<bitmap> b2, 	<hpb> a3;
	//				toneMap;
	//				toneMapperFilter, 	<toneMapper> t2; 

		// background types

		switch( this->mtkr_renderGlobals->backgroundType )
		{
		case 0:
			{
			// constant
			Kray::Vector bgColor;
			bgColor.setRgb(this->mtkr_renderGlobals->environmentColor.r, this->mtkr_renderGlobals->environmentColor.g, this->mtkr_renderGlobals->environmentColor.b);
			this->pro->background_color(bgColor);
			break;
			}
		case 1:
			{
			// phySky
			Kray::Symbol psky(*this->pro, "psky");
			MVector sunDirV = this->mtkr_renderGlobals->sunDir;
			MVector orientV = this->mtkr_renderGlobals->orientation;
			Kray::Vector sunDir(sunDirV.x, sunDirV.y, sunDirV.z);			
			Kray::AxesHpb orientationH(orientV.x, orientV.y, orientV.z);	
			Kray::Axes orientation(orientationH);

			this->pro->phySky(psky, sunDir, this->mtkr_renderGlobals->turbidity, this->mtkr_renderGlobals->exposure, orientation);
			this->pro->phySkyParam_solidAngle(psky, this->mtkr_renderGlobals->solidAngle);
			this->pro->phySkyParam_sunIntensity(psky, this->mtkr_renderGlobals->sunIntensity);
			this->pro->phySkyParam_sunSpotAngle(psky, this->mtkr_renderGlobals->sunSpotAngle);
			this->pro->background_phySky(psky);
			break;
			}
		case 2:
			{
			// phySky2
			Kray::Symbol psky(*this->pro, "psky");
			MVector sunDirV = this->mtkr_renderGlobals->sunDir;
			MVector orientV = this->mtkr_renderGlobals->orientation;
			Kray::Vector sunDir(sunDirV.x, sunDirV.y, sunDirV.z);			
			Kray::AxesHpb orientationH(orientV.x, orientV.y, orientV.z);	
			Kray::Axes orientation(orientationH);
			Kray::Vector groundAlbedo;
			this->pro->phySky2(psky, sunDir, this->mtkr_renderGlobals->turbidity, this->mtkr_renderGlobals->exposure, orientation, groundAlbedo);
			this->pro->background_phySky(psky);
			break;
			}
		case 3:
			{
			// sky
			Kray::Vector zenithDir(this->mtkr_renderGlobals->zenithDir.x, this->mtkr_renderGlobals->zenithDir.y, this->mtkr_renderGlobals->zenithDir.z);
			Kray::Vector nadir, horizonColor, zenithColor;
			horizonColor.setRgb(this->mtkr_renderGlobals->gradientHorizon.r, this->mtkr_renderGlobals->gradientHorizon.g, this->mtkr_renderGlobals->gradientHorizon.b);
			zenithColor.setRgb(this->mtkr_renderGlobals->gradientZenit.r, this->mtkr_renderGlobals->gradientZenit.g, this->mtkr_renderGlobals->gradientZenit.b);
			nadir.setRgb(this->mtkr_renderGlobals->nadir.r, this->mtkr_renderGlobals->nadir.g, this->mtkr_renderGlobals->nadir.b);
			this->pro->background_sky(horizonColor, zenithColor, nadir, zenithDir);
			//prot.background_sky(Kray::Vector(0.4,0.8,0.9),Kray::Vector(0,0,0),Kray::Vector(1,1,1),Kray::Vector(0,1,0)); // gradient background
			break;
			}
		case 4:
			{
			Kray::Vector zenithDir(this->mtkr_renderGlobals->zenithDir.x, this->mtkr_renderGlobals->zenithDir.y, this->mtkr_renderGlobals->zenithDir.z);
			Kray::Vector nadir, horizon_zenith, horizon_nadir, zenith;
			horizon_zenith.setRgb(this->mtkr_renderGlobals->gradientHorizon.r, this->mtkr_renderGlobals->gradientHorizon.g, this->mtkr_renderGlobals->gradientHorizon.b);
			horizon_nadir.setRgb(this->mtkr_renderGlobals->gradientZenit.r, this->mtkr_renderGlobals->gradientZenit.g, this->mtkr_renderGlobals->gradientZenit.b);
			nadir.setRgb(this->mtkr_renderGlobals->nadir.r, this->mtkr_renderGlobals->nadir.g, this->mtkr_renderGlobals->nadir.b);
			this->pro->background_sky2(horizon_zenith, horizon_nadir, zenith, nadir, zenithDir, this->mtkr_renderGlobals->skyGamma, this->mtkr_renderGlobals->groundGamma);
			break;
			}
		case 5:
			{
			// bitmap
			MString imagePath;
			if(getConnectedFileTexturePath(MString("environmentMap"), MString("krayGlobals"), imagePath))
			{
			//Kray::BitmapSymbol bmp(imagePath.asChar(), *this->pro);
			//Kray::Vector ax, az;
			//this->pro->background_bitmap(bmp, ax, az);
			}else{
				logger.error("Could not set fileTextureNode.");
			}
			break;
			}
		case 6:
			{
			// bitmap2
			Kray::Axes axes;
			MString imagePath1, imagePath2;
			if(getConnectedFileTexturePath(MString("environmentMap"), MString("krayGlobals"), imagePath1))
			{
				if(getConnectedFileTexturePath(MString("environmentMap2"), MString("krayGlobals"), imagePath1))
				{
					Kray::BitmapSymbol bmp1(imagePath1.asChar(), *this->pro);
					Kray::BitmapSymbol bmp2(imagePath2.asChar(), *this->pro);
					this->pro->background_bitmap2(bmp1, bmp2, axes);
				}else{
					logger.error("Could not set fileTextureNode.");
				}
			}else{
				logger.error("Could not set fileTextureNode.");
			}
			break;
			}
		case 7:
			// directions map
			this->pro->background_directionsMap();
			break;
		
		case 8:
			// light map
		//	Kray::Axes axes;
		//	Kray::BitmapSymbol lmap("../Bitmaps/kray-logo.png", *this->pro);
		//	this->pro->background_lightMap(lmap, axes);
			break;
		
		case 9:
			{
			// sphere map
			MString imagePath;
			if(getConnectedFileTexturePath(MString("environmentMap"), MString("krayGlobals"), imagePath))
			{
				Kray::BitmapSymbol bmp(imagePath.asChar(), *this->pro);
				Kray::Axes axes;
				this->pro->background_sphereMap(bmp, axes);
			}else{
				logger.error("Could not set spherical fileTextureNode.");
			}
			break;
			}
		}		
	}
}
