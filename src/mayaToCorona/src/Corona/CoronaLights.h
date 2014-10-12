#ifndef CORONA_LIGHTS_H
#define CORONA_LIGHTS_H

#include "CoronaCore/api/Api.h"
#include "CoronaCore/api/ApiInterfaces.h"

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
		const Corona::Spectrum diffuse(lightColor.r(), lightColor.g(), lightColor.b());
		Corona::Spectrum sColor(shadowColor.r(), shadowColor.g(), shadowColor.b());
		Corona::BsdfComponents bsdf;
		const Corona::Pos P = context.getPosition();
		Corona::Pos rndPos(context.generateRandom() * lightRadius, context.generateRandom() * lightRadius, context.generateRandom() * lightRadius);
		Corona::Pos PP = LP + rndPos;
        float distance;
        const Corona::Dir toLight = (PP - P).getNormalized(distance);
		distance *= distFactor;
		if (doShadows)
			transport = context.shadowTransmission(PP, Corona::RAY_NORMAL) + sColor;
		else
			transport = Corona::Spectrum(1.0, 1.0, 1.0);
		float dummy;
		float decay = 1.0;
		if( decayType == 1) // linear decay
			decay = 1.0/distance;
		if( decayType == 2) // quadratic decay
			decay = 1.0/(distance * distance);
		if( decayType == 3) // cubic decay
			decay = 1.0/(distance * distance * distance);

		context.forwardBsdfCos(toLight, bsdf, dummy);
        const float dotSurface = absDot(context.getShadingNormal(), toLight);
		return bsdf * diffuse * (dotSurface * Corona::PI) * lightIntensity * decay;
	}
};

class SpotLight : public Corona::Abstract::LightShader, public Corona::Noncopyable 
{
public:
	Corona::Pos LP;
	Corona::Dir LD;
	Corona::Rgb lightColor;
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
		
		float dropOff = dropoff;
		float coneAngle = angle * 0.5;
		const Corona::Spectrum diffuse(lightColor.r(), lightColor.g(), lightColor.b());
		Corona::Spectrum sColor(shadowColor.r(), shadowColor.g(), shadowColor.b());
		Corona::BsdfComponents bsdf;
		const Corona::Pos P = context.getPosition();
		Corona::Pos rndPos(context.generateRandom() * lightRadius, context.generateRandom() * lightRadius, context.generateRandom() * lightRadius);
		Corona::Pos PP = LP + rndPos;
		float distance;
        const Corona::Dir toLight = (PP - P).getNormalized(distance);
		distance *= distFactor;
		const float cosAngle = acos(Corona::Utils::max(0.f, -dot(toLight, LD)));
		float dummy;
		if (doShadows)
			transport = context.shadowTransmission(PP, Corona::RAY_NORMAL) + sColor;
		else
			transport = Corona::Spectrum(1.0, 1.0, 1.0);

		context.forwardBsdfCos(toLight, bsdf, dummy);

		float angularValue = 1.0;
		if( penumbraAngle > 0.0)
		{
			float startAngle = coneAngle;
			float endAngle = coneAngle + penumbraAngle * 2;
			if( cosAngle > startAngle )
			{
				angularValue = (endAngle - cosAngle)/(penumbraAngle * 2.0);
				coneAngle += penumbraAngle * 2;
			}
		}
		if( penumbraAngle < 0.0)
		{
			float startAngle = coneAngle - abs(penumbraAngle) * 2.0;
			float endAngle = coneAngle;
			if( cosAngle > startAngle )
			{
				angularValue = (endAngle - cosAngle)/(abs(penumbraAngle) * 2.0);
			}
		}

		if( dropoff > 0.0)
		{
			float fac = cosAngle/coneAngle;
			angularValue *= 1.0 - pow(1.0f - fac, dropoff);
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

        const float dotSurface = absDot(context.getShadingNormal(), toLight);
		return bsdf * diffuse * (dotSurface * Corona::PI) * lightIntensity * decay * angularValue;
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
		const Corona::Spectrum diffuse(lightColor.r(), lightColor.g(), lightColor.b());
		Corona::Spectrum sColor(shadowColor.r(), shadowColor.g(), shadowColor.b());
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
			transport = context.shadowTransmission(dirLightPos, Corona::RAY_NORMAL) + sColor;
		else
			transport = Corona::Spectrum(1.0, 1.0, 1.0);

		float dummy;
		context.forwardBsdfCos(toLight, bsdf, dummy);
        const float dotSurface = absDot(context.getShadingNormal(), toLight);
		return bsdf * diffuse * (dotSurface * Corona::PI) * lightIntensity;
	}
};

#endif