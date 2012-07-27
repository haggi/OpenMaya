#ifndef MIA_MAT_X_H
#define MIA_MAT_X_H

#define VOP_SHADING
#define VOP_SURFACE

#include "voptype.h"
#include "voplib.h"
#include "utils.h"

    //struct {
    //    
    //    vector "diffuse_result",
    //    vector "diffuse_raw",
    //    vector "diffuse_level",
    //    
    //    vector "spec_result",
    //    vector "spec_raw",
    //    vector "spec_level",
    //    
    //    vector "refl_result",
    //    vector "refl_raw",
    //    vector "refl_level",
    //    
    //    vector "refr_result",
    //    vector "refr_raw",
    //    vector "refr_level",
    //    
    //    vector "tran_result",
    //    vector "tran_raw",
    //    vector "tran_level",
    //    
    //    vector "indirect_result",
    //    vector "indirect_raw",
    //    vector "indirect_post_ao",
    //    vector "ao_raw",
    //    
    //    vector "add_result",
    //    
    //    vector  "opacity_result",
    //    vector  "opacity_raw",
    //    float "opacity"
    //}


	//"mia_material_x" (
    //    float  "diffuse_weight"      default 1.0, #: min 0.0 max 1.0
    //    vector   "diffuse"             default 0.5 0.5 0.5,
    //    float  "diffuse_roughness"   default 0.0, #: min 0.0 max 1.0

    //    float  "reflectivity"        default 0.6, #: min 0.0 max 1.0
    //    vector   "refl_vector"          default 1.0 1.0 1.0,
    //    float  "refl_gloss"          default 1.0, #: min 0.0 max 1.0 softmin 0.15
    //    int "refl_gloss_samples"  default 8,   #: min 0 softmax 128
    //    int "refl_interpolate"    default off,
    //    int "refl_hl_only"        default off,
    //    int "refl_is_metal"       default off,

    //    float  "transparency"        default 0.0, #: min 0.0 max 1.0
    //    vector   "refr_vector"          default 1.0 1.0 1.0,
    //    float  "refr_gloss"          default 1.0, #: min 0.0 max 1.0
    //    float  "refr_ior"            default 1.4, #: min 0.1 softmax 2.0
    //    int "refr_gloss_samples"  default 8,   #: min 0 softmax 128
    //    int "refr_interpolate"    default off,
    //    int "refr_translucency"   default off,
    //    vector   "refr_trans_vector"    default 0.7 0.6 0.5,
    //    float  "refr_trans_weight"   default 0.5, #: min 0.0 max 1.0

    //    float  "anisotropy"          default 1.0, #: min 0.1 softmax 10
    //    float  "anisotropy_rotation" default 0.0, #: softmin 0 softmax 1
    //    int "anisotropy_channel"  default -1,  #: min -4  softmax 64

    //    int "brdf_fresnel"         default off,
    //    float  "brdf_0_degree_refl"   default 0.2, #: min 0.0 max 1.0
    //    float  "brdf_90_degree_refl"  default 1.0, #: min 0.0 max 1.0
    //    float  "brdf_curve"           default 5.0, #: min 0.1 softmax 20
    //    int "brdf_conserve_energy" default on,

    //    # Interpolation settings
    //    
    //    int "intr_grid_density"   default 2,    #: min 0 max 5 enum "2 (double):1 (same as rendering):1/2 (half resolution):1/3 (third resolution):1/4 (quarter resolution):1/5 (fifth resolution)"
    //    int "intr_refl_samples"   default 2,    #: min 0 softmax 5
    //    int "intr_refl_ddist_on"  default off,
    //    float  "intr_refl_ddist"     default 0.0,
    //    int "intr_refr_samples"   default 2,    #: min 0 softmax 5
    //    int "single_env_sample"   default off,

    //    # Reflection/Refraction optimizations & falloffs

    //    int  "refl_falloff_on"       default off,
    //    float   "refl_falloff_dist"     default 0.0,
    //    int  "refl_falloff_vector_on" default off,
    //    vector    "refl_falloff_vector"    default 0 0 0,
    //    int  "refl_depth"            default 5,    #: min 0 softmax 10
    //    float   "refl_cutoff"           default 0.01, #: min 0.0 max 1.0

    //    int  "refr_falloff_on"       default off,
    //    float   "refr_falloff_dist"     default 0.0,
    //    int  "refr_falloff_vector_on" default off,
    //    vector    "refr_falloff_vector"    default 0 0 0,
    //    int  "refr_depth"            default 5,    #: min -1 softmax 10
    //    float   "refr_cutoff"           default 0.01, #: min 0.0 max 1.0

    //    # Factors

    //    float   "indirect_multiplier"   default 1.0,  #: min 0 softmax 1.0
    //    float   "fg_quality"            default 1.0,  #: min 0 softmax 10.0
    //    float   "fg_quality_w"          default 1.0,  #: min 0 softmax 1.0

    //    # Built in AO
    //    
    //    int  "ao_on"          default off,
    //    int  "ao_samples"     default 16,          #: min 0 softmax 128
    //    float   "ao_distance"    default 10.0,
    //    vector    "ao_dark"        default 0.2 0.2 0.2,
    //    vector    "ao_ambient"     default 0.0 0.0 0.0,
    //    int  "ao_do_details"  default 1,           #: min 0 max 2 enum "Off:On:With vector bleed"

    //    # Options

    //    int  "thin_walled"            default off,
    //    int  "no_visible_area_hl"     default on,
    //    int  "skip_inside_refl"       default on,
    //    int  "do_refractive_caustics" default off,
    //    int  "backface_cull"          default off,
    //    int  "propagate_alpha"        default off,    

    //    # Other effects
    //    
    //    float   "hl_vs_refl_balance"     default 1.0,   #: min 0.0 softmax 1.0
    //    float   "cutout_opacity"         default 1.0,   #: min 0.0 max 1.0
    //    vector    "additional_vector"       default 0 0 0 0,
    //    shader   "bump",
    //    int  "no_diffuse_bump"        default off,

    //    int     "mode"                default 4,	#: property "lightmode"
    //    array light "lights",				#: property "lightlink"
    //    
    //    # Additional bump handling
    //    int     "bump_mode"			  default 5, 
    //    vector      "overall_bump"        default 0 0 0,
    //    vector      "standard_bump"       default 0 0 0,
    //    
    //    # Input/output flags
    //    int     "multiple_outputs"    default on,

    //    # Base reflection
    //    float  "refl_base"               default 0.0, #: min 0.0 max 1.0
    //    vector   "refl_base_vector"         default 1.0 0.3 0.0,
    //    float  "refl_base_gloss"         default 1.0, #: min 0.0 max 1.0
    //    int "refl_base_gloss_samples" default 8,   #: min 0 softmax 128
    //    vector  "refl_base_bump"          default 0.0 0.0 0.0
    //)


    //float compute_fresnel(float theta_1; float Y;)
    //{
    //    float yy = Y * Y;
    //    float cos_t = cos(theta_1);
    //    float g = sqrt(yy - 1 + cos_t*cos_t);
    //    float F1 = (yy*cos_t-g)/(yy*cos_t+g);
    //    float F2 = (cos_t-g)/(cos_t+g);
    //    float F = .5 * (F1*F1 + F2*F2);
    //    return F;
    //}

//vector specular_highlight(
//        //vector In;
//        //vector Nf;
//        //vector V;
//        //float anisotropy_rotation;
//        //float  refl_roughness_u, refl_roughness_v;
//		)
//{
//    vector Nn = normalize(N);
//    vector In = normalize(I);
//    vector Nf = frontface(Nn, In);
//    vector V = -In;
//	float anisotropy_rotation = 0.0;
//	float  refl_roughness_u = 0.1, refl_roughness_v = .1;
//	vector xdir=0; vector ydir=0;
//	vector uv = set(s, t, 0);
//	matrix uvxform = ident();
//	rotate( uvxform, radians(360*anisotropy_rotation), Nf);
//	uv *= invert(uvxform);
//	--vop_computeTangents("uv", Nn, uv, xdir, ydir, Nf, uv, 2);
//
//	vector highlights = 0;
//
//	/* We have three specular higlihgts of diminushing roughness but increasing brightness */
//	 float component_coefs[] = {0.5, 1.0, 1.5};
//
//	illuminance( P, Nf, PI * 0.5 )
//	{
//		vector Ln = normalize(L);
//		float dot_ln = dot(Ln , Nf);
//		float dot_vn = dot(V , Nf);
//
//		if( dot_vn*dot_ln>0.0 )
//		{
//			vector Hn = normalize(V + Ln);
//			float dot_hn2 = min(sq(dot(Hn , Nf)), 1.0);
//
//			if( dot_hn2>0.0 )
//			{
//				shadow(Cl);
//
//				/* precompute this to get it out of the loop below */
//				float k1_devider = 1 / (sqrt(dot_vn * dot_ln) * 4.0 * PI);  
//				float smooth_step_ln = smooth( 0.0, 0.2, dot_ln );
//
//				 int i=0;
//				 float roughness_coef = 1.0;
//
//				for( i=0; i<3; i++ )
//				{
//					// Compute the highlight due to this light source.
//					float k1 = (refl_roughness_u * refl_roughness_v * roughness_coef * roughness_coef ) * k1_devider;
//					float k2 = sq(dot(Hn , xdir) * refl_roughness_u * roughness_coef) + sq(dot(Hn , ydir) * refl_roughness_v * roughness_coef);
//					vector c = k1 * exp(-k2 / dot_hn2) * dot_ln * smooth_step_ln;
//					// Accumulate highlights.
//					highlights += Cl * c * component_coefs[i];
//					roughness_coef *= 0.5;
//				}
//			}
//		}
//	}
//	return highlights;
//}



void mia_material_x(
	vector diffuseColor;
    float  diffuse_weight;
    float  diffuse_roughness;

    float  reflectivity;
	//vector refl_vector;
    float  refl_gloss;
    int    refl_gloss_samples;
    //int    refl_interpolate;
    int	   refl_hl_only;
    int    refl_is_metal;

	int	  brdf_fresnel;
	float brdf_0_degree_refl;
	float brdf_90_degree_refl;
	float brdf_curve;

	float refr_ior;
	vector 	refr_color;
	float transparency;
	float refr_gloss;
	int refr_gloss_samples;

	int refl_falloff_on;
	float refl_falloff_dist;
	int refl_falloff_color_on;
	vector refl_falloff_color;

	int refr_falloff_on;
	float refr_falloff_dist;
	int refr_falloff_color_on;
	vector refr_falloff_color;
	
	int refl_depth;
	int refr_depth;

	vector result;
	vector tran_result;
	vector opacity_result;
	int outMessage;
	)
{
    VOPvector ii = normalize(I);
	VOPnormal nn = normalize(N);
	vector one = {1,1,1};
    vector nf = vop_frontface(nn, ii);	
	//diffuse weight 0 == lambert shading 1==orenNayar
	vector diffuseIllum = vop_lighting("oren", nf, ii, {0,0,0}, {0,0,0}, {1,1,1}, {0,0,0}, diffuse_roughness, 0.0, 0);
	diffuseIllum *= diffuseColor * diffuse_weight;

	//vector V = -I;
	//vector H = normalize(V+L);
	//float base = 1.0 - dot(V,H);
	//float exponential = pow( base, 5.0);
	//float ff = exponential + refl_gloss * (1.0 - exponential);

    float kr = 1.0, kt = 1.0;
	float refraction_ior  = refr_ior;
	if(brdf_fresnel > 0)
	{
	    float eta = (dot(ii , nn) < 0.0) ? 1.0 / refraction_ior : refraction_ior;
		fresnel(ii, nn, eta, kr, kt);
	}else{
		// because if they are the same, we get 0 as result for kr
		if( brdf_0_degree_refl != brdf_90_degree_refl)
		{
			float facingRatio = 1.0 - dot(nn, ii);
			facingRatio = pow(facingRatio, brdf_curve);
			kr = smooth(brdf_0_degree_refl, brdf_90_degree_refl, facingRatio);
		}
	}
	vector reflectedColor = set(1,1,1);
	vector uvw = set(s, t, 0);
	vector ambientColor = set(0,0,0);
	vector diffuseCol = set(0,0,0);
	vector specularColor = set(1,1,1);
	vector attenuationColor = 0;
    float urough = 1.0/pow(2.0, 3.0 * refl_gloss);
	float vrough = urough;
	int tstyle = 0;
	int doAttenuation = 0;
	float attenuationDist = 0;
	vector spec1 = vop_lighting("anisotropic", nf, ii, uvw, ambientColor, diffuseCol, specularColor, urough, vrough, tstyle);
	//urough = urough * .03;
	//vrough = urough;
	//vector spec2 = vop_lighting("anisotropic", nf, ii, uvw, ambientColor, diffuseCol, specularColor, urough, vrough, tstyle);
	vector spec = spec1 * kr;
	//vector spec = specular_highlight();
	if(refl_is_metal > 0)
		spec *= diffuseColor;
	
	reflectivity *= kr;
	if( reflectivity > 0.0)
	{
		float reflAngle = 0.0;
		if( refl_gloss >= 1.0)
		{
			reflAngle = 0.0;
			refl_gloss_samples = 1;
		}else{
			reflAngle = degrees((1.0 - pow(clamp(refl_gloss, 0.0, 1.0), 0.2)) * PI / 2.0);
		}
		attenuationColor = refl_falloff_color;
		doAttenuation = refl_falloff_color_on;
		attenuationDist = refl_falloff_dist;
		if( refl_falloff_on == 0)
			attenuationDist = -1.0;
		getReflectionA( ii, nn, reflAngle, refl_gloss_samples, reflectivity, reflectedColor, refl_depth, attenuationDist, attenuationColor, doAttenuation);
		//getReflection( ii, nn, reflAngle, refl_gloss_samples, reflectivity, reflectedColor);

		// metallic reflection will receive a tint from the base diffuse color
		if(refl_is_metal > 0)
			reflectedColor *= diffuseColor;

		transparency *= (1.0 - reflectivity);
	}else{
		reflectedColor *= 0.0;
	}	

	vector refractedColor = refr_color;
	float refractedAlpha = 0;
	if( transparency > 0.0)
	{
		float refrAngle = 0.0;
		if( refr_gloss >= 1.0)
		{
			refrAngle = 0.0;
			refr_gloss_samples = 1;
		}else{
			refrAngle = degrees((1.0 - pow(clamp(refr_gloss, 0.0, 1.0), 0.2)) * PI / 2.0);
		}

		attenuationColor = refr_falloff_color;
		doAttenuation = refr_falloff_color_on;
		attenuationDist = refr_falloff_dist;
		if( refr_falloff_on == 0)
			attenuationDist = -1.0;
		getRefractionA( ii, nn, refr_ior, refrAngle, refr_gloss_samples, refr_depth, attenuationDist, attenuationColor, doAttenuation, refractedColor, refractedAlpha);
		refractedColor *= transparency;
	}
	result = diffuseIllum * (1.0 - reflectivity) * (1.0 - transparency) + reflectedColor + refractedColor + spec;
	//kr = smooth(brdf_0_degree_refl, brdf_90_degree_refl, facingRatio);
	tran_result = 0;
	opacity_result = 1;
	outMessage = 0;
}

#endif