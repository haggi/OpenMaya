#ifndef P_SAMPLER_INFH
#define P_SAMPLER_INFH

// here I encounter a little problem: By default the particleSampler node as an output name for all per particle attributes.
// These per particle attributes will be defined in geometry, what means the shader will be given a bound attribute.
// That means that the particle sampler has the same attribute as geometry bound attribute and as output attribute. And because
// an bound attriute cannot - or should not - be modified (as much as I know), all per particle attributes will be written with a 
// prefix "p_". This way I can seperate inputs and outputs.

// By default the particles will have Cd as geometry color

void ParticleSamplerInfo(   

   // geometry attributes
   float p_ageNormalized;
   float p_vectorRed;
   float p_vectorGreen;
   float p_vectorBlue;
   float p_opacityPP;
   float p_opacity;
   float p_particleId;
   float p_finalLifespanPP;
   float p_lifespanPP;
   float p_lifespan;
   float p_age;
   float p_birthTime;
   float p_parentU;
   float p_parentV;
   float p_collisionU;
   float p_collisionV;
   float p_radiusPP;
   float p_radius;
   float p_userScalar1PP;
   float p_userScalar2PP;
   float p_userScalar3PP;
   float p_userScalar4PP;
   float p_userScalar5PP;
   vector p_rgbPP;
   vector p_incandescencePP;
   vector p_incandescence;
   vector p_acceleration;
   vector p_force;
   vector p_position;
   vector p_worldPosition;
   vector p_birthPosition;
   vector p_birthWorldPosition;
   vector p_velocity;
   vector p_worldVelocity;
   vector p_userVector1PP;
   vector p_userVector2PP;
   vector p_userVector3PP;
   vector p_userVector4PP;
   vector p_userVector5PP; 

   // normal inputs
   float outUvType;
   float normalizationMethod;
   float normalizationValue;
   float inverseOutUv;

   vector outColor;
   vector outTransparency;
   vector outIncandescence;
   float ageNormalized;
   float colorRed;
   float colorGreen;
   float colorBlue;
   float opacityPP;
   float opacity;
   float particleId;
   float finalLifespanPP;
   float lifespanPP;
   float lifespan;
   float age;
   float birthTime;
   float parentU;
   float parentV;
   float collisionU;
   float collisionV;
   float radiusPP;
   float radius;
   float userScalar1PP;
   float userScalar2PP;
   float userScalar3PP;
   float userScalar4PP;
   float userScalar5PP;
   vector rgbPP;
   vector incandescencePP;
   vector incandescence;
   vector acceleration;
   vector force;
   vector position;
   vector worldPosition;
   vector birthPosition;
   vector birthWorldPosition;
   vector velocity;
   vector worldVelocity;
   vector userVector1PP;
   vector userVector2PP;
   vector userVector3PP;
   vector userVector4PP;
   vector userVector5PP; 
   )
{
	outTransparency = 0.0;
	if(  isbound("p_opacityPP") )
		outTransparency = 1.0 - p_opacityPP;
}

#endif