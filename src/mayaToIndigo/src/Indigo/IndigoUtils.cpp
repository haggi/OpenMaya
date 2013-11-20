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
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "../mtin_common/mtin_mayaObject.h"
#include "../mtin_common/mtin_mayaScene.h"

static Logging logger;


void IndigoRenderer::createTransform(Indigo::Vector<Indigo::KeyFrame>& keyFrames, mtin_MayaObject *obj)
{
	MPoint pos, scale, rot;
	for( uint mId = 0; mId < obj->transformMatrices.size(); mId++)
	{
		double time = 0.0;
		
		if( mId > 0)
			time = (double)mId/((double)(obj->transformMatrices.size() - 1));

		MMatrix m = obj->transformMatrices[mId];
		getMatrixComponents(m, pos, rot, scale);
		MTransformationMatrix tm(m);
		double x, y, z, w;
		tm.getRotationQuaternion(x, y, z, w, MSpace::kWorld);
		Indigo::AxisAngle axis(Indigo::Vec3d(x, y, z), w);	
		Indigo::KeyFrame posKf(time, Indigo::Vec3d(pos.x, pos.y, pos.z), axis);
		keyFrames.push_back(posKf);
	}
	//Indigo::MatrixRotation matRot(m[0][0],m[1][0],m[2][0], m[0][1],m[1][1],m[2][1] ,m[0][2],m[1][2],m[2][2]); 
	//model->rotation = new Indigo::MatrixRotation(matRot);
}
