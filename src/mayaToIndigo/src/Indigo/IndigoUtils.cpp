#include "Indigo.h"
#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MIntArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshVertex.h>
#include <maya/MQuaternion.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "../mtin_common/mtin_mayaObject.h"
#include "mayaScene.h"
#include "world.h"

static Logging logger;


void IndigoRenderer::createTransform(const Indigo::SceneNodeModelRef& model, std::shared_ptr<mtin_MayaObject> obj)
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	if (renderGlobals->doMb)
	{
		MPoint pos, oscale, rot;
		for( uint mId = 0; mId < obj->transformMatrices.size(); mId++)
		{
			double time = 0.0;
		
			if( mId > 0)
				time = (double)mId/((double)(obj->transformMatrices.size() - 1));

			MMatrix m = obj->transformMatrices[mId];
			m = m * renderGlobals->globalConversionMatrix;
			getMatrixComponents(m, pos, rot, oscale);
			MMatrix scale;
			scale.setToIdentity();
			MMatrix mt = m.transpose();

			scale[0][0] = oscale.x;
			scale[1][1] = oscale.y;
			scale[2][2] = oscale.z;

			
			MTransformationMatrix tm(m);
			MTransformationMatrix origTm(obj->transformMatrices[mId]);
			double x, y, z, w;
			tm.getRotationQuaternion(x, y, z, w, MSpace::kWorld);
			origTm.getRotationQuaternion(x, y, z, w, MSpace::kWorld);

			Indigo::Vec3d v(x, -z, y);
			Indigo::AxisAngle axis(v, w);	

			if(v.length() <= 0.0)
				axis = Indigo::AxisAngle(Indigo::Vec3d(0, 0, 1), 0.0);

			Indigo::KeyFrame posKf(time, Indigo::Vec3d(pos.x, pos.y, pos.z), axis);
			model->keyframes.push_back(posKf);

			Indigo::MatrixRotation matRot(scale[0][0],scale[1][0],scale[2][0], scale[0][2],scale[1][2],scale[2][2] ,scale[0][1],scale[1][1],scale[2][1]);
			model->rotation = new Indigo::MatrixRotation(matRot);
		}
	}else{
		MPoint pos, scale, rot;
		MMatrix m = obj->transformMatrices[0];
		m = m * renderGlobals->globalConversionMatrix;
		getMatrixComponents(m, pos, rot, scale);
		Indigo::KeyFrame posKf(0.0, Indigo::Vec3d(pos.x, pos.y, pos.z), Indigo::AxisAngle().identity());
		model->keyframes.push_back(posKf);
	    Indigo::MatrixRotation matRot(m[0][0],m[1][0],m[2][0], m[0][1],m[1][1],m[2][1] ,m[0][2],m[1][2],m[2][2]);
	    model->rotation = new Indigo::MatrixRotation(matRot);
	}
}


//mit mb
//
//	// double PosA, PosB;
//dobule AxisA, AngleA, AxisB, AngleB;
//
//// Konvertiert Rotationsmatrix in Axis/Angle (Maya hat vermutlich eine passende API Funktion, falls nicht is weiter unten der C4D API Quellcode der funktion).
//MatrixToRotAxis(MatrixA, &AxisA, &AngleA);
//MatrixToRotAxis(MatrixB, &AxisB, &AngleB);
//
//Indigo::Vector<Indigo::KeyFrame>& keyframes = model->keyframes;
//
//// Motionstep A.
//keyframes.push_back(Indigo::KeyFrame(0.0, PosA, Indigo::AxisAngle(AxisA, AngleA)));
//
//// Motionstep B.
//keyframes.push_back(Indigo::KeyFrame(0.0, PosB, Indigo::AxisAngle(AxisB, AngleB)));
//
//// Eventueller Scale, da Indigo::KeyFrames im Moment keinen Scale Parameter haben.
//// Y und Z sind in diesm fall vertauscht, da in C4D Y oben ist und Indigo in Indigo Z.
//model->rotation = Reference<Indigo::Rotation>(new Indigo::MatrixRotation(
//        Len(MatrixA.v1),  0,                0,
//        0,                Len(MatrixA.v3),  0,
//        0,                0,                Len(MatrixA.v2)));
//
//
//// C4D MatrixToRotAxis Funktion.
//void MatrixToRotAxis(const Matrix &mm, Vector *v, Real *w)
//{
//    Matrix m = mm;
//    // MatrixVectoren MUESSEN normiert sein!!!
//    m.v1=!m.v1;
//    m.v2=!m.v2;
//    m.v3=!m.v3;
//
//    // Winkel berechnen
//    *w = ACos((m.v1.x+m.v2.y+m.v3.z-RCO 1.0)/RCO 2.0);
//
//    // Achse berechnen
//    v->x= m.v2.z-m.v3.y;
//    v->y= m.v3.x-m.v1.z;
//    v->z= m.v1.y-m.v2.x;
//    *v = !(*v);
//
//    if (*v==0.0)
//    {
//        *v = Vector(0.0,1.0,0.0);
//        *w = 0.0;
//    }
//};
//
//keyframes.push_back(Indigo::KeyFrame(1.0, PosB, Indigo::AxisAngle(AxisB, AngleB)));
