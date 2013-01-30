//#include "krayRenderer.h"

#include "../mtkr_common/mtkr_mayaObject.h"
#include "../mtkr_common/mtkr_mayaScene.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MColor.h>
#include "krayRenderer.h"
#include "krayUtils.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "utilities/logging.h"

static Logging logger;

namespace krayRender{

		void KrayRenderer::defineLigths()
		{
			for( size_t lId = 0; lId < this->mtkr_scene->lightList.size(); lId++)
			{
				mtkr_MayaObject *obj = (mtkr_MayaObject *)this->mtkr_scene->lightList[lId];
				logger.debug(MString("Define kray light: ") + obj->shortName);

				if( obj->mobject.hasFn(MFn::kPointLight))
					this->definePointLight(obj);
				if( obj->mobject.hasFn(MFn::kDirectionalLight))
					this->defineDirectionalLight(obj);
			}
		}

		void KrayRenderer::definePointLight(mtkr_MayaObject *obj)
		{
			logger.debug(MString("Define kray point light: ") + obj->shortName);
			MFnDependencyNode lightFn(obj->mobject);
			MColor lightColor(1,1,1);
			float intensity = 1.0f;
			getColor(MString("color"), lightFn, lightColor);
			getFloat(MString("intensity"), lightFn, intensity);
			lightColor *= intensity;
			Kray::Vector col;
			col.setRgb(lightColor.r, lightColor.g, lightColor.b);
			MMatrix matrix = obj->transformMatrices[0];// * this->mtkr_renderGlobals->sceneScaleMatrix * this->mtkr_renderGlobals->sceneRotMatrix;
			//MVector lpos;
			//Kray::Matrix4x4 lightMatrix;
			//MMatrixToAMatrix(matrix, lightMatrix);
			Kray::Vector lPos;
			Kray::AxesHpb lRot;
			MatrixToRotPos(matrix, lPos, lRot);
			this->pro->lightAddNamed_point(obj->shortName.asChar(), lPos, col); // add light source to scene
		}

		void KrayRenderer::defineDirectionalLight(mtkr_MayaObject *obj)
		{
			logger.debug(MString("Define kray directional light: ") + obj->shortName);
			MFnDependencyNode lightFn(obj->mobject);
			MColor lightColor(1,1,1);
			float intensity = 1.0f;
			getColor(MString("color"), lightFn, lightColor);
			getFloat(MString("intensity"), lightFn, intensity);
			lightColor *= intensity;
			Kray::Vector pos, col;
			col.setRgb(lightColor.r, lightColor.g, lightColor.b);
			MMatrix matrix = obj->transformMatrices[0];
			MVector lightRot(0, 0, 1);
			lightRot *= obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneScaleMatrix * this->mtkr_renderGlobals->sceneRotMatrix;
			Kray::Vector lDir(lightRot.x,lightRot.y,lightRot.z);			
			//Kray::Matrix4x4 lightMatrix;
			//MMatrixToAMatrix(matrix, lightMatrix);
			//Kray::Vector lPos(lightMatrix);
			this->pro->lightAddNamed_parallel(lightFn.name().asChar(), lDir, col);
		}

		void KrayRenderer::defineSpotLight(mtkr_MayaObject *obj)
		{
			logger.debug(MString("Define kray spot light: ") + obj->shortName);
			MFnDependencyNode lightFn(obj->mobject);
			MColor lightColor(1,1,1);
			float intensity = 1.0f;
			getColor(MString("color"), lightFn, lightColor);
			getFloat(MString("intensity"), lightFn, intensity);
			lightColor *= intensity;
			Kray::Vector pos, col;
			col.setRgb(lightColor.r, lightColor.g, lightColor.b);
			MMatrix matrix = obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneScaleMatrix * this->mtkr_renderGlobals->sceneRotMatrix;
			//MVector lpos;
			//Kray::Matrix4x4 lightMatrix;
			//MMatrixToAMatrix(matrix, lightMatrix);
			//Kray::Vector lPos(lightMatrix);
			Kray::Vector lPos;
			Kray::AxesHpb lRot;
			MatrixToRotPos(matrix, lPos, lRot);
			//void lightAddNamed_softSpot(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5,double d6,double d7);
		}

		void KrayRenderer::defineAreaLight(mtkr_MayaObject *obj)
		{
			logger.debug(MString("Define kray area light: ") + obj->shortName);
			MFnDependencyNode lightFn(obj->mobject);
			MColor lightColor(1,1,1);
			float intensity = 1.0f;
			getColor(MString("color"), lightFn, lightColor);
			getFloat(MString("intensity"), lightFn, intensity);
			lightColor *= intensity;
			Kray::Vector rot, col;
			col.setRgb(lightColor.r, lightColor.g, lightColor.b);
			MMatrix matrix = obj->transformMatrices[0];
			//MVector lpos;
			//Kray::Matrix4x4 lightMatrix;
			//MMatrixToAMatrix(matrix, lightMatrix);
			//Kray::Vector lPos(lightMatrix);
			Kray::Vector lPos;
			Kray::AxesHpb lRot;
			MatrixToRotPos(matrix, lPos, lRot);

			//this->pro->lightAddNamed_parallel(obj->shortName.asChar(), rot, col);
		}

 //void lightAddNamed_adaptive(const char* name,const Vector& v3,const Vector& v4,const Vector& v5,const Symbol& t6,double d7,int d8,int d9);
 //void lightAddNamed_coneAdaptive(const char* name,const Vector& v3,const Vector& v4,const Vector& v5,const Symbol& t6,double d7,int d8,int d9,double d10,double d11);
 //void lightAddNamed_dirPoint(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5);
 //void lightAddNamed_fade(const char* name,const Symbol& l3);
 //void lightAddNamed_fadedPoint(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5);
 //void lightAddNamed_fadePoint(const char* name,const VarLenVector& v3,const Vector& v4);
 //void lightAddNamed_line(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5);
 //void lightAddNamed_linear(const char* name,const Vector& v3,const Vector& v4,const Vector& v5);
 //void lightAddNamed_name(const char* name,const Symbol& l3);
 //void lightAddNamed_parallel(const char* name,const VarLenVector& v3,const Vector& v4);
 //void lightAddNamed_phySky_adaptive(const char* name,const Symbol& phySkyObject,double distance,double limit,int rmin,int rmax);
 //void lightAddNamed_phySky_parallel(const char* name,const Symbol& phySkyObject);
 //void lightAddNamed_phySky_point(const char* name,const Symbol& phySkyObject,double distance);
 //void lightAddNamed_point(const char* name,const VarLenVector& v3,const Vector& v4);
 //void lightAddNamed_projector1(const char* name,const VarLenVector& v3,const VarLenAxes& a4,const BitmapSymbol& b5);
 //void lightAddNamed_rectangle(const char* name,const VarLenVector& v3,const VarLenAxes& a4,double d5,double d6,const Symbol& t7,double d8,int d9,int d10);
 //void lightAddNamed_rectangleSpot(const char* name,const VarLenVector& v3,const VarLenAxes& a4,double d5,double d6,const Symbol& t7,double d8,int d9,int d10,double d11,double d12,const Vector& v13);
 //void lightAddNamed_shadowmapPoint(const char* name,const Symbol& s3,const Vector& v4);
 //void lightAddNamed_sharpSpot(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5,double d6);
 //void lightAddNamed_softSpot(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5,double d6,double d7);
 //void lightAddNamed_sphere(const char* name,const VarLenVector& v3,double radius,const Symbol& t5,double variance,int min_rays,int max_rays);
 //void lightAddNamed_spot(const char* name,const VarLenVector& v3,const VarLenVector& v4,const Vector& v5,double d6,double d7);

};