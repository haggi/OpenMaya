#include "krayUtils.h"
#include "utilities/tools.h"
#include <maya/MEulerRotation.h>
#include <maya/MPlug.h>

#include "krayRenderer.h"

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

void MatrixToRotPos(MMatrix& mayaMatrix, Kray::Vector& kpos, Kray::AxesHpb& krot)
{
	MMatrix posMatrix = mayaMatrix;
	MMatrix rotMatrix = mayaMatrix;

	MVector rot;
	MPoint pos;
	posRotFromMatrix( rotMatrix, pos, rot);
	rotMatrix.setToIdentity();
	MTransformationMatrix tm(rotMatrix);
	MEulerRotation euler(rot.x, rot.y, rot.z, MEulerRotation::kXYZ);
	tm.rotateBy(euler, MSpace::kWorld);
	rotMatrix = tm.asMatrix();

	Kray::Matrix4x4 mPosMatrix;
	Kray::Matrix4x4 mRotMatrix;
		
	//posMatrix[3][0] = -posMatrix[3][0];
	//posMatrix[3][1] = -posMatrix[3][1];
	MMatrixToAMatrix(posMatrix, mPosMatrix);
	MMatrixToAMatrix(rotMatrix, mRotMatrix);

	kpos = Kray::Vector(mPosMatrix);
	krot = Kray::AxesHpb(mRotMatrix);
}

bool getConnectedFileTexture(MFnDependencyNode& depFn, MString& attribute, MString& fileName)
{
	MStatus stat;
	MPlug thisPlug = depFn.findPlug(attribute, &stat);
	if( !stat )
		return false;
	MObject otherSideNode = getOtherSideNode(thisPlug);
	if( !otherSideNode.hasFn(MFn::kFileTexture))
		return false;
	MFnDependencyNode fileNode(otherSideNode);
	MPlug ftn = fileNode.findPlug("fileTextureName", &stat);
	if(!stat)
		return false;
	
	fileName = ftn.asString();
	return true;
}