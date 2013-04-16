#include "Lux.h"

#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MGlobal.h>
#include "utilities/tools.h"

void setZUp(MMatrix& matrix)
{
	MMatrix YtoZ;
	YtoZ.setToIdentity();
	YtoZ[0][0] = 1;
	YtoZ[0][1] = 0;
	YtoZ[0][2] = 0;
	YtoZ[0][3] = 0;
	
	YtoZ[1][0] = 0;
	YtoZ[1][1] = 0;
	YtoZ[1][2] = 1;
	YtoZ[1][3] = 0;
	
	YtoZ[2][0] = 0;
	YtoZ[2][1] = -1;
	YtoZ[2][2] = 0;
	YtoZ[2][3] = 0;
	
	YtoZ[3][0] = 0;
	YtoZ[3][1] = 0;
	YtoZ[3][2] = 0;
	YtoZ[3][3] = 1;	

	if (MGlobal::isYAxisUp())
	{
		//MTransformationMatrix tMatrix(matrix);
		//tMatrix.rotateBy( MEulerRotation::MEulerRotation(DegToRad(90), 0, 0), MSpace::kTransform);
		//MVector oTrans = tMatrix.getTranslation(MSpace::kTransform);
		//oTrans = MVector( oTrans[0], -oTrans[2], oTrans[1] );
		//tMatrix.setTranslation(oTrans, MSpace::kTransform);
		//matrix = tMatrix.asMatrix();
		matrix *= YtoZ;
	}
}

void setZUp(MMatrix& matrix, float *fm)
{
	setZUp(matrix);
	int i = 0;
	for( int r = 0; r < 4; r++)
		for( int c = 0; c < 4; c++)
			fm[i++] = matrix[r][c];
}

void setZUp(MPoint& point)
{
	MMatrix matrix;
	matrix.setToIdentity();
	matrix[3][0] = point.x;
	matrix[3][1] = point.y;
	matrix[3][2] = point.z;
	setZUp(matrix);
	point.x = matrix[3][0];
	point.y = matrix[3][1];
	point.z = matrix[3][2];
}

void setZUp(MVector& point)
{
	MMatrix matrix;
	matrix.setToIdentity();
	matrix[3][0] = point.x;
	matrix[3][1] = point.y;
	matrix[3][2] = point.z;
	setZUp(matrix);
	point.x = matrix[3][0];
	point.y = matrix[3][1];
	point.z = matrix[3][2];
}
