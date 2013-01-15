#include "krayUtils.h"
#include "utilities/tools.h"

void MMatrixToAMatrix(MMatrix& mayaMatrix, Kray::Matrix4x4& krayResultMatrix)
{
	MMatrix krayMatrix = mayaMatrix;
	//double a = mayaMatrix[0][3];
	//double b = mayaMatrix[1][3];
	//double c = mayaMatrix[2][3];
	//krayMatrix[0][3] =  krayMatrix[3][0];
	//krayMatrix[1][3] = -krayMatrix[3][1];
	//krayMatrix[2][3] = -krayMatrix[3][2];
	//krayMatrix[3][0] = a;
	//krayMatrix[3][1] = b;
	//krayMatrix[3][2] = c;

	rowToColumn(mayaMatrix, krayMatrix);
	//krayMatrix[0][3] *=  1.0;
	//krayMatrix[1][3] *= -1.0;
	//krayMatrix[2][3] *= -1.0;

	double mdata[16];
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			mdata[i * 4 + k] = krayMatrix[i][k];
	krayResultMatrix.copyTransposed(mdata);
}
