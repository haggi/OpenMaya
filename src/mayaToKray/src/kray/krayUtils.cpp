#include "krayUtils.h"
#include "utilities/tools.h"

void MMatrixToAMatrix(MMatrix& mayaMatrix, Kray::Matrix4x4& krayMatrix)
{
	MMatrix rowMatrix;
	rowToColumn(mayaMatrix, rowMatrix);
	double mdata[16];
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			mdata[i * 4 + k] = rowMatrix[i][k];
	krayMatrix.copyTransposed(mdata);
}
