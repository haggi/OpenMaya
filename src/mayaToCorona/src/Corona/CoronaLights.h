#ifndef CORONA_LIGHTS_H
#define CORONA_LIGHTS_H

#include "CoronaCore/api/Api.h"
#include "CoronaCore/api/ApiInterfaces.h"
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

class PointLight : public Corona::Abstract::LightShader, public Corona::Noncopyable 
{
public:
	Corona::Pos LP;
	Corona::Rgb lightColor;
	float	lightIntensity;
	int		decayType;
	float	distFactor;
	float	lightRadius;
	float	lightAngle;
	bool	doShadows;
	Corona::Rgb shadowColor;

	PointLight()
	{
		LP = Corona::Pos(0,0,0);
		lightColor = Corona::Rgb(1,1,1);
		lightIntensity = 1.0f;
		decayType = 0;// no decay
		distFactor = 1.0f;
		lightRadius = 0.0f;
		lightAngle = 0.0f;
		doShadows = true;
		shadowColor = Corona::Rgb(0, 0, 0);
	}

	virtual Corona::BsdfComponents getIllumination(Corona::IShadeContext& context, Corona::Spectrum& transport) const
	{
		
		const Corona::Spectrum diffuse(Corona::rgb2Radiance(Corona::Rgb(lightColor.r(), lightColor.g(), lightColor.b())));
		Corona::Spectrum sColor(Corona::rgb2Radiance(Corona::Rgb(shadowColor.r(), shadowColor.g(), shadowColor.b())));
		Corona::BsdfComponents bsdf;
		const Corona::Pos P = context.getPosition();
		Corona::Pos rndPos(context.generateRandom() * lightRadius, context.generateRandom() * lightRadius, context.generateRandom() * lightRadius);
		Corona::Pos PP = LP + rndPos;
        float distance;
        const Corona::Dir toLight = (PP - P).getNormalized(distance);
		distance *= distFactor;
		if (doShadows)
			transport = context.shadowTransmission(PP, Corona::RAY_NORMAL);
		else
			transport = Corona::Spectrum::WHITE;
		float dummy;
		float decay = 1.0;
		if( decayType == 1) // linear decay
			decay = 1.0/distance;
		if( decayType == 2) // quadratic decay
			decay = 1.0/(distance * distance);
		if( decayType == 3) // cubic decay
			decay = 1.0/(distance * distance * distance);

		context.forwardBsdfCos(toLight, bsdf, dummy);
        //const float dotSurface = absDot(context.getShadingNormal(), toLight);
		//return bsdf * diffuse * (dotSurface * Corona::PI) * lightIntensity * decay;
		return bsdf * diffuse * Corona::PI * lightIntensity * decay;
	}
};

class SpotLight : public Corona::Abstract::LightShader, public Corona::Noncopyable 
{
public:
	Corona::Pos LP;
	Corona::Dir LD;
	Corona::Rgb lightColor;
	Corona::AffineTm lightWorldInverseMatrix;
	float	lightIntensity;
	int		decayType;
	float	angle;
	float	penumbraAngle;
	float	dropoff;
	float	distFactor;
	float	lightRadius;
	float	lightAngle;
	bool	doShadows;
	Corona::Rgb shadowColor;
	Corona::ColorOrMap lightColorMap;

	SpotLight()
	{
		LP = Corona::Pos(0,0,0);
		lightColor = Corona::Rgb(1,1,1);
		lightIntensity = 1.0f;
		decayType = 0;// no decay
		angle = 45.0f;
		penumbraAngle = 0.0f;
		dropoff = 0.0f;
		distFactor = 1.0f;
		lightRadius = 0.0f;
		lightAngle = 0.0f;
		doShadows = true;
		shadowColor = Corona::Rgb(0, 0, 0);
	}

	virtual Corona::BsdfComponents getIllumination(Corona::IShadeContext& context, Corona::Spectrum& transport) const
	{
		Corona::Rgb lcol(1,1,1);
		float outAlpha;
		Corona::Pos lightUvs;
		Corona::Pos origUvCoords = context.getMapCoords(0);

		float dropOff = dropoff;
		float coneAngle = angle * 0.5;
		//		const Corona::Spectrum diffuse(Corona::rgb2Radiance(Corona::Rgb(lightColor.r(), lightColor.g(), lightColor.b())));
		Corona::Spectrum sColor(Corona::rgb2Radiance(Corona::Rgb(shadowColor.r(), shadowColor.g(), shadowColor.b())));
		Corona::BsdfComponents bsdf;
		const Corona::Pos P = context.getPosition();
		Corona::Pos rndPos(context.generateRandom() * lightRadius, context.generateRandom() * lightRadius, context.generateRandom() * lightRadius);
		Corona::Pos PP = LP + rndPos;
		float distance;
        const Corona::Dir toLight = (PP - P).getNormalized(distance);
		distance *= distFactor;
		const float cos = Corona::Utils::max(0.f, -dot(toLight, LD));
		const float cosAngle = acos(cos);
		float dummy;
		if (doShadows)
			transport = context.shadowTransmission(PP, Corona::RAY_NORMAL);
		else
			transport = Corona::Spectrum::WHITE;


		context.forwardBsdfCos(toLight, bsdf, dummy);

		float angularValue = 1.0;
		if( penumbraAngle != 0.0)
		{
			
			angularValue = smoothstep(coneAngle, coneAngle + penumbraAngle, cosAngle);
			if (penumbraAngle > 0.0)
			{
				angularValue = 1.0 - angularValue;
				coneAngle += penumbraAngle;
			}
		}

		if( dropoff > 0.0)
		{
			float fac = cosAngle/coneAngle;
			angularValue *= pow(cos, dropoff);
		}

		if( cosAngle > coneAngle)
			return bsdf * Corona::Spectrum::BLACK;


		float decay = 1.0;
		if( decayType == 1) // linear decay
			decay = 1.0/distance;
		if( decayType == 2) // quadratic decay
			decay = 1.0/(distance * distance);
		if( decayType == 3) // cubic decay
			decay = 1.0/(distance * distance * distance);

		// the idea: simply transform the hit pos to camera space, then we use the x and z coordinates to 
		// as uv coords
		Corona::Pos lightLocalPos = this->lightWorldInverseMatrix.transformPoint(P);
		lightUvs.x() = lightLocalPos.x();
		lightUvs.z() = lightLocalPos.z();

		context.overrideMapCoords(lightUvs);
		
		Corona::Matrix33 base = context.bumpBase(0);
		Corona::Dir T = base.tangent();

		lcol = lightColorMap.getRgb(context, nullptr);
		context.restoreMapCoords(origUvCoords);
		const Corona::Spectrum diffuse(Corona::rgb2Radiance(lcol));

        const float dotSurface = absDot(context.getShadingNormal(), toLight);
		return bsdf * diffuse * Corona::PI * lightIntensity * decay * angularValue;
	}
};

class DirectionalLight : public Corona::Abstract::LightShader, public Corona::Noncopyable 
{
public:
	Corona::Pos LP;
	Corona::Dir LD;
	Corona::Rgb lightColor;
	Corona::Dir LT;
	Corona::Dir LBT;
	float	lightIntensity;
	float	lightRadius;
	float	lightAngle;
	bool	doShadows;
	Corona::Rgb shadowColor;


	DirectionalLight()
	{
		LP = Corona::Pos(0,0,0);
		lightColor = Corona::Rgb(1,1,1);
		lightIntensity = 1.0f;
		lightRadius = 0.0f;
		lightAngle = 0.0f;
		doShadows = true;
		shadowColor = Corona::Rgb(0, 0, 0);
	}

	virtual Corona::BsdfComponents getIllumination(Corona::IShadeContext& context, Corona::Spectrum& transport) const
	{
		const Corona::Spectrum diffuse(Corona::rgb2Radiance(Corona::Rgb(lightColor.r(), lightColor.g(), lightColor.b())));
		Corona::Spectrum sColor(Corona::rgb2Radiance(Corona::Rgb(shadowColor.r(), shadowColor.g(), shadowColor.b())));
		Corona::BsdfComponents bsdf;
		const Corona::Pos P = context.getPosition();
		const Corona::Dir tl = LD * -1.0;
		const Corona::Dir toLight = (tl).getNormalized();
		const Corona::Dir rndT = LT * (context.generateRandom() - 0.5) * 2.0;
		const Corona::Dir rndBT = LBT * (context.generateRandom() - 0.5) * 2.0;
		const Corona::Dir newTo = (rndT + rndBT);
		float blendFac = lightAngle/180.0f;
		const Corona::Dir finalDir = (newTo * blendFac + (1.0 - blendFac) * toLight).getNormalized();
		Corona::Pos dirLightPos = P + finalDir * 10.0;
		if (doShadows)
			transport = context.shadowTransmission(dirLightPos, Corona::RAY_NORMAL);
		else
			transport = Corona::Spectrum::WHITE;

		float dummy;
		context.forwardBsdfCos(toLight, bsdf, dummy);
		//const float dotSurface = absDot(context.getShadingNormal(), toLight);
		//return bsdf * diffuse * (dotSurface * Corona::PI) * lightIntensity;
		return bsdf * diffuse * Corona::PI * lightIntensity;
	}
};

#endif