#include "Thea.h"
#include "utilities/tools.h"

void TheaRenderer::matrixToTransform(MMatrix& m, TheaSDK::Transform& t)
{
	MPoint rot, scale, pos;
	posRotSclFromMatrix(m, pos, rot, scale);
	TheaSDK::Transform tfinal = t;
	TheaSDK::Transform tn;
	t.r11 = m[0][0];
	t.r12 = m[0][1];
	t.r13 = m[0][2];
	t.r21 = m[1][0];
	t.r22 = m[1][1];
	t.r23 = m[1][2];
	t.r31 = m[2][0];
	t.r32 = m[2][1];
	t.r33 = m[2][2];
	t.tx = m[3][0];
	t.ty = m[3][1];
	t.tz = m[3][2];

	t.r11 = m[0][0];
	t.r12 = m[1][0];
	t.r13 = m[2][0];
	t.r21 = m[0][1];
	t.r22 = m[1][1];
	t.r23 = m[2][1];
	t.r31 = m[0][2];
	t.r32 = m[1][2];
	t.r33 = m[2][2];

	//tfinal = tn.Translate(pos.x, pos.y, pos.z);
	//tfinal = tfinal * tn.RotateX(rot.x);
	//tfinal = tfinal * tn.RotateY(rot.y);
	//tfinal = tfinal * tn.RotateZ(rot.z);
	//tfinal = tfinal * tn.Scale(scale.x, scale.y, scale.z);
	//t = tfinal;
}